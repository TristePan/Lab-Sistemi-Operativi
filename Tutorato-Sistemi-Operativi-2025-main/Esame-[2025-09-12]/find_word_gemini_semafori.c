#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_PATH 1024
#define MAX_SIZE 10

/* Struttura dati per il passaggio delle informazioni */
typedef struct {
    char path[MAX_PATH];
    int occ; /* Usato solo nel buffer di output */
} Record;

/* Buffer Condiviso con SEMAFORI POSIX */
typedef struct {
    Record buffer[MAX_SIZE];
    int in;  /* Indice inserimento */
    int out; /* Indice estrazione */
    int count; /* Numero elementi presenti (per controllo stato) */
    
    bool close; /* Flag di chiusura */
    int active_producers; /* Conteggio produttori attivi per questo buffer */

    pthread_mutex_t mutex;
    sem_t sem_empty; /* Semaforo: slot liberi */
    sem_t sem_full;  /* Semaforo: slot occupati (dati disponibili) */
} Shared_buffer;

/* --- Gestione Buffer --- */

void buffer_init(Shared_buffer *buff, int num_producers) {
    buff->in = 0;
    buff->out = 0;
    buff->count = 0;
    buff->close = false;
    buff->active_producers = num_producers;

    pthread_mutex_init(&buff->mutex, NULL);
    /* Inizializzazione semafori: 
       empty = MAX_SIZE (tutto libero), full = 0 (nessun dato) */
    sem_init(&buff->sem_empty, 0, MAX_SIZE);
    sem_init(&buff->sem_full, 0, 0);
}

void buffer_destroy(Shared_buffer *buff) {
    pthread_mutex_destroy(&buff->mutex);
    sem_destroy(&buff->sem_empty);
    sem_destroy(&buff->sem_full);
}

/* Funzione per chiudere il buffer e sbloccare chi attende */
void buffer_close(Shared_buffer *buff) {
    pthread_mutex_lock(&buff->mutex);
    buff->close = true;
    pthread_mutex_unlock(&buff->mutex);

    /* "Poison Pill": Sblocco forzato di eventuali thread in attesa.
       Faccio una post su entrambi i semafori per svegliare sia chi aspetta 
       spazio (empty) sia chi aspetta dati (full). */
    sem_post(&buff->sem_empty);
    sem_post(&buff->sem_full);
}

/* Chiamata dai produttori quando finiscono il lavoro */
void buffer_signal_end(Shared_buffer *buff) {
    pthread_mutex_lock(&buff->mutex);
    buff->active_producers--;
    bool last = (buff->active_producers == 0);
    pthread_mutex_unlock(&buff->mutex);

    if (last) {
        buffer_close(buff);
    }
}

void buffer_in(Shared_buffer *buff, Record r) {
    /* 1. Attesa semaforo spazio libero */
    sem_wait(&buff->sem_empty);

    pthread_mutex_lock(&buff->mutex);

    /* Controllo se il buffer è chiuso (mentre aspettavo) */
    if (buff->close) {
        pthread_mutex_unlock(&buff->mutex);
        sem_post(&buff->sem_empty); /* Propago lo sblocco */
        return;
    }

    /* 2. Inserimento */
    buff->buffer[buff->in] = r;
    buff->in = (buff->in + 1) % MAX_SIZE;
    buff->count++;

    pthread_mutex_unlock(&buff->mutex);

    /* 3. Segnalo dato disponibile */
    sem_post(&buff->sem_full);
}

bool buffer_out(Shared_buffer *buff, Record *r) {
    /* 1. Attesa semaforo dato disponibile */
    sem_wait(&buff->sem_full);

    pthread_mutex_lock(&buff->mutex);

    /* Se buffer chiuso e vuoto, finiamo */
    if (buff->close && buff->count == 0) {
        pthread_mutex_unlock(&buff->mutex);
        sem_post(&buff->sem_full); /* Propago lo sblocco agli altri consumatori se ce ne fossero */
        return false;
    }

    /* 2. Estrazione */
    *r = buff->buffer[buff->out];
    buff->out = (buff->out + 1) % MAX_SIZE;
    buff->count--;

    pthread_mutex_unlock(&buff->mutex);

    /* 3. Segnalo spazio libero */
    sem_post(&buff->sem_empty);
    return true;
}

/* --- Funzioni Ausiliarie --- */

int check_parola(const char *filename, const char *word) {
    FILE *file = fopen(filename, "r");
    if (!file) return -1;

    char buffer[256];
    int count = 0;

    /* Lettura semplice parola per parola. 
       Nota: strcasecmp è POSIX. */
    while (fscanf(file, "%255s", buffer) == 1) {
        /* Rimuovo eventuale punteggiatura finale grossolanamente se necessario,
           ma per semplicità usiamo strcasecmp diretto come da standard lab */
        if (strcasecmp(buffer, word) == 0) {
            count++;
        }
        /* Nota: per una ricerca più robusta "substring" si userebbe strcasestr,
           ma fscanf legge token separati da spazi. */
    }

    fclose(file);
    return count;
}

/* --- Thread Functions --- */

/* Argomenti per DIR thread */
typedef struct {
    Shared_buffer *proposte;
    const char *directory;
    int id; /* ID numerico per output (DIR-1, DIR-2...) */
} prod_args;

void *producer(void *arg) {
    prod_args *args = (prod_args *)arg;
    Record r;
    r.occ = 0;
    
    printf("[DIR-%d] scansione della cartella '%s'...\n", args->id, args->directory);

    DIR *dir = opendir(args->directory);
    if (!dir) {
        fprintf(stderr, "[DIR-%d] Errore apertura directory %s\n", args->id, args->directory);
        buffer_signal_end(args->proposte);
        pthread_exit(NULL);
    }

    struct dirent *entry;
    struct stat st;
    char fullpath[MAX_PATH];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", args->directory, entry->d_name);
        
        if (stat(fullpath, &st) == 0 && S_ISREG(st.st_mode)) {
            printf("[DIR-%d] trovato il file '%s' in '%s'\n", args->id, entry->d_name, args->directory);
            
            strncpy(r.path, fullpath, MAX_PATH - 1);
            r.path[MAX_PATH - 1] = '\0';
            buffer_in(args->proposte, r);
        }
    }

    closedir(dir);
    
    /* Importante: segnalo fine scansione */
    buffer_signal_end(args->proposte);
    pthread_exit(NULL);
}

/* Argomenti per SEARCH thread */
typedef struct {
    Shared_buffer *proposte;
    Shared_buffer *proposte_out;
    const char *word;
} ver_args;

void *verifier(void *arg) {
    ver_args *args = (ver_args *)arg;
    Record r_in;

    printf("[SEARCH] parola da cercare: '%s'\n", args->word);

    /* Leggo dal buffer 'proposte' finché non viene chiuso e svuotato */
    while (buffer_out(args->proposte, &r_in)) {
        
        int occorrenze = check_parola(r_in.path, args->word);

        if (occorrenze > 0) {
            printf("[SEARCH] il file '%s' contiene %d occorrenze\n", r_in.path, occorrenze);
            
            Record r_out;
            strncpy(r_out.path, r_in.path, MAX_PATH);
            r_out.occ = occorrenze;
            
            buffer_in(args->proposte_out, r_out);
        } else {
             /* Opzionale: stampa per file senza occorrenze come da esempio pdf */
             printf("[SEARCH] il file '%s' non contiene occorrenze\n", r_in.path);
        }
    }

    /* SEARCH ha finito: chiude il buffer di output verso il MAIN */
    buffer_signal_end(args->proposte_out);
    pthread_exit(NULL);
}

/* --- MAIN --- */

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <word> <dir-1> ... <dir-n>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *word_to_search = argv[1];
    int num_dirs = argc - 2;

    Shared_buffer buffer_path;     /* Buffer 1: DIR -> SEARCH */
    Shared_buffer buffer_results;  /* Buffer 2: SEARCH -> MAIN */

    /* Inizializzazione Buffer */
    /* buffer_path: N produttori (DIR) */
    buffer_init(&buffer_path, num_dirs);
    /* buffer_results: 1 produttore (SEARCH) */
    buffer_init(&buffer_results, 1);

    pthread_t *dir_threads = malloc(sizeof(pthread_t) * num_dirs);
    prod_args *dir_args = malloc(sizeof(prod_args) * num_dirs);
    pthread_t search_thread;

    /* Avvio Thread DIR */
    for (int i = 0; i < num_dirs; ++i) {
        dir_args[i].proposte = &buffer_path;
        dir_args[i].directory = argv[i + 2];
        dir_args[i].id = i + 1;
        
        if (pthread_create(&dir_threads[i], NULL, producer, &dir_args[i]) != 0) {
            perror("Errore creazione thread DIR");
            return EXIT_FAILURE;
        }
    }

    /* Avvio Thread SEARCH */
    ver_args ver_arg = { 
        .proposte = &buffer_path, 
        .proposte_out = &buffer_results, 
        .word = word_to_search 
    };
    if (pthread_create(&search_thread, NULL, verifier, &ver_arg) != 0) {
        perror("Errore creazione thread SEARCH");
        return EXIT_FAILURE;
    }

    /* --- IL MAIN AGISCE DA CONSUMATORE FINALE (Per evitare Deadlock) --- */
    Record res;
    int total_occurrences = 0;

    /* Questo ciclo termina solo quando SEARCH ha finito e chiuso il buffer_results */
    while (buffer_out(&buffer_results, &res)) {
        total_occurrences += res.occ;
        printf("[MAIN] con il file '%s' il totale parziale è di %d occorrenze\n", 
               res.path, total_occurrences);
    }

    printf("[MAIN] il totale finale è di %d occorrenze\n", total_occurrences);

    /* Attesa terminazione thread (Clean join) */
    for (int i = 0; i < num_dirs; ++i) {
        pthread_join(dir_threads[i], NULL);
    }
    pthread_join(search_thread, NULL);

    /* Pulizia memoria */
    buffer_destroy(&buffer_path);
    buffer_destroy(&buffer_results);
    free(dir_threads);
    free(dir_args);

    return EXIT_SUCCESS;
}