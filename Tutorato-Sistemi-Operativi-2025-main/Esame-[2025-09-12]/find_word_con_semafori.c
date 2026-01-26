#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <semaphore.h>

#define MAX_PATH 256
#define MAX_SIZE 10

typedef struct {
    char path[MAX_PATH];
    int occ;
}Record;

typedef struct {
    Record buffer[MAX_SIZE];
    int in;
    int out; 
    int count;
    
    bool close;
    
    pthread_mutex_t mutex;
    sem_t sem_empty; // Conta gli slot liberi
    sem_t sem_full; // Conta gli elementi disponibili
}Shared_buffer;

void buffer_init(Shared_buffer *buff) {
    buff -> in = 0;
    buff -> out = 0;
    buff -> count = 0;
    buff -> close = false;

    pthread_mutex_init(&buff -> mutex, NULL);

    sem_init(&buff -> sem_empty, 0, MAX_SIZE);
    sem_init(&buff -> sem_full, 0, 0);
}

void buffer_close(Shared_buffer *buff) {
    pthread_mutex_lock(&buff -> mutex);

    buff -> close = true;
    
    pthread_mutex_unlock(&buff -> mutex);

    sem_post(&buff -> sem_empty);
    sem_post(&buff -> sem_full);
}

void buffer_destroy(Shared_buffer *buff) {
    pthread_mutex_destroy(&buff->mutex);
    sem_destroy(&buff->sem_empty);
    sem_destroy(&buff->sem_full);
}

void buffer_in(Shared_buffer *buff, Record r) {
    // 1. Aspetto che sia spazio libero
    sem_wait(&buff -> sem_empty);
    
    pthread_mutex_lock(&buff -> mutex);

    // Controllo se il buffer è chiuso
    if(buff -> close) {
        pthread_mutex_unlock(&buff -> mutex);
        sem_post(&buff -> sem_empty); // Restituisco il gettone per non bloccare altri
        return;
    }

    // 2. Sezione critica: Inserimento senza bisogno di while/wait
    buff -> buffer[buff -> in] = r;
    buff -> in = (buff -> in + 1) % MAX_SIZE;
    buff -> count++;

    pthread_mutex_unlock(&buff -> mutex);
    
    // 3. Segnalo che c'è un nuovo dato disponibile (sveglai consumatore)
    sem_post(&buff -> sem_full);

}

// Da completare
bool buffer_out(Shared_buffer *buff, Record *r) {
    // 1. Aseptto che ci sia un dato
    sem_wait(&buff -> sem_full);
    
    pthread_mutex_lock(&buff -> mutex);

    // Controllo chiusura: se Chiuso e Vuoto, finiamo.
    if(buff -> count == 0 && buff -> close) {
        pthread_mutex_unlock(&buff -> mutex);

        // IMPORTANTE: "Passaparola". Se siamo in tanti consumatori e il buffer chiude,
        // devo svegliare il prossimo che potrebbe essere bloccato da sem_wait.
        sem_post(&buff -> sem_full);
        return false;
    }

    // Sezione critica: Estrazione
    *r = buff -> buffer[buff -> out];
    buff -> out = (buff -> out + 1) % MAX_SIZE;
    buff -> count--;

    pthread_mutex_unlock(&buff -> mutex);

    // 3. Segnalo che c'è uno slot libero (Sveglia produttore)
    sem_post(&buff -> sem_empty);

    return true;
}

/* ------------------------------------------------------------------------- */

bool read_dir(DIR *dir, const char *directory, Record *r) {
    struct dirent *entry;
    struct stat st;

    char path[MAX_PATH];

    while((entry = readdir(dir)) != NULL) {
        if(!strcmp(entry -> d_name, ".") || !strcmp(entry -> d_name, "..")) {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", directory, entry -> d_name);
        
        if(stat(path, &st) == 0 && S_ISREG(st.st_mode)) {
            strncpy(r -> path, path, MAX_PATH);
            r->path[MAX_PATH - 1] = '\0';   
            return true;
        }
    }

    return false;
}

int check_parola(const char *filename, const char *word) {
    FILE *file = fopen(filename, "r");

    if(!file) {
        perror("Errore apertura file \n");
        return -1;
    }

    char buffer[100];
    int count = 0;
    int len = strlen(word);

    while(fscanf(file, "%255s", buffer) == 1) {
        if(strcasecmp(buffer, word) == 0) {
            count ++;
        }
    }

    fclose(file);
    return count;
}

/* --------------------------------------------------------------------------------- */

/*-------------- PRODUCER --------------*/
typedef struct {
    Shared_buffer *proposte;
    const char *directory;
}prod_args;

void *producer(void *arg) {
    prod_args *args = (prod_args *)arg;
    Record r;
    r.occ = 0;
    
    DIR *dir = opendir(args -> directory);
    if(!dir) {
        perror("Errore apertura directory...\n");
        pthread_exit(NULL);
    }

    while(read_dir(dir, args -> directory, &r)) {
        buffer_in(args -> proposte, r);
    }

    closedir(dir);
    pthread_exit(NULL);
}

/*-------------- VERIFIER --------------*/

typedef struct {
    Shared_buffer *proposte;
    Shared_buffer *proposte_out;
    const char *word;
}ver_args;

void *verifier(void *arg) {
    ver_args *args = (ver_args *)arg;
    Record r;

    while(buffer_out(args -> proposte, &r)) {

        int occorrenze = check_parola(r.path, args -> word);

        if(occorrenze > 0) {
            r.occ = occorrenze;
            buffer_in(args -> proposte_out, r);
        }
    }

    pthread_exit(NULL);
}

/*-------------- CONSUMER --------------*/
// ...


int main(int argc, char *argv[]) {

    if(argc < 3) {
        printf("Uso: <word> <dir1> <dir2> ... <dir-n> ", argv[0]);
        return 1;
    }

    const char *word_to_search = argv[1];
    int N = argc - 2;

    Shared_buffer proposte, proposte_out;
    buffer_init(&proposte);
    buffer_init(&proposte_out);

    pthread_t thread_producers[N];
    pthread_t thread_verifier;

    prod_args prod_r[N];

    for(int i = 0; i < N; ++i) {
        prod_r[i].proposte = &proposte;
        prod_r[i].directory = argv[i + 2];
        
        if (pthread_create(&thread_producers[i], NULL, producer, &prod_r[i]) != 0) {
            perror("Errore creazione thread");
            return 1;
        }
    }

    ver_args ver_r = {
        .proposte = &proposte, 
        .proposte_out = &proposte_out, 
        .word = word_to_search
    };

    pthread_create(&thread_verifier, NULL, verifier, &ver_r);

    for(int i = 0; i < N; ++i) {
        pthread_join(thread_producers[i], NULL);
    }

    buffer_close(&proposte);

    pthread_join(thread_verifier, NULL);

    buffer_close(&proposte_out);

    // printf("Numero di occorenze: %d\n", r.occ);
    Record r;
    printf("--- Risultati per la parola '%s' ---\n", word_to_search);
    int total_files = 0;

    while(buffer_out(&proposte_out, &r)) {
        printf("File: %s | Occorrenze: %d\n", r.path, r.occ);
        total_files++;
    }

    if(total_files == 0) {
        printf("Nessuna occorrenza trovata.\n");
    }

    buffer_destroy(&proposte);
    buffer_destroy(&proposte_out);
    return 0;
}
