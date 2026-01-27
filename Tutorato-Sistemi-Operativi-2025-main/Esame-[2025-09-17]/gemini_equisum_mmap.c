#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>

// COSTANTI DEL PROBLEMA
#define M 12 // Dimensione fissa di ogni vettore        
#define BUFFER_SIZE 10  // Dimensione della coda intermedia
#define NUM_VERIFIERS 3 // Numero fisso di verificatori

// Funzioni per il controllo dell'output
#define LOCK_IO()   flockfile(stdout)
#define UNLOCK_IO() funlockfile(stdout)

// ===================== STRUTTURE DATI =====================

// Record: il pacchetto di dati che viaggia tra i thread
// Contiene dati grezzi dei file
typedef struct {
    unsigned char dati[M];
    int file_id;    
    int vector_idx; 
} Record;

// SharedData: la struttura "MONITOR" che contiene le risorse condivise
typedef struct {
    // --- 1. CODA INTERMEDIA ---
    // Serve per passare dati da lettori a verificatori
    Record buffer[BUFFER_SIZE];
    int in; // Indice dove scrivere il prossimo dato
    int out; // Indice dove leggere il prossimo dato
    int count; // Quanti elementi ci sono nel buffer
    bool buffer_closed; // Diventa TRUE quando i lettori hanno finito
    
    // Sincronizzazione Coda Intermedia
    pthread_mutex_t mtx_buffer;
    pthread_cond_t cond_buf_not_empty;
    pthread_cond_t cond_buf_not_full;

    // Slot Finale
    // Serve per passare i risultati dei verificatori al main
    Record slot_finale;
    bool slot_full;
    bool slot_closed;

    // Sincronizzazione Slot Finale
    pthread_mutex_t mtx_slot;
    pthread_cond_t cond_slot_empty;
    pthread_cond_t cond_slot_full;

    // Contatori per gestione chiusura automatica
    int readers_active; // Quanti lettori sono ancora vivi
    int verifiers_active; // Quanti verificatori sono ancora vivi
    pthread_mutex_t mtx_counts; // Protegge questi contatori

} SharedData;

// ===================== HELPER =====================
// Funzione per stampare il vettore
void print_vec(unsigned char *v) {
    for (int i = 0; i < M; i++) {
        printf("%u", v[i]);
        if (i < M - 1) printf(", ");
    }
}

// ===================== GESTIONE MONITOR =====================

// Inizializzo tutti i mutex e le condition
void init_shared(SharedData *s, int n_readers) {
    // Coda intermedia
    s->in = 0; s->out = 0; s->count = 0; s->buffer_closed = false;
    pthread_mutex_init(&s->mtx_buffer, NULL);
    pthread_cond_init(&s->cond_buf_not_empty, NULL);
    pthread_cond_init(&s->cond_buf_not_full, NULL);

    // Slot finale
    s->slot_full = false; s->slot_closed = false;
    pthread_mutex_init(&s->mtx_slot, NULL);
    pthread_cond_init(&s->cond_slot_empty, NULL);
    pthread_cond_init(&s->cond_slot_full, NULL);

    // Contatori
    s->readers_active = n_readers;
    s->verifiers_active = NUM_VERIFIERS;
    pthread_mutex_init(&s->mtx_counts, NULL);
}

// ----------------- FUNZIONI BUFFER -----------------

// ----------------- PRODUTTORE -----------------
// Inserisce un record nella coda intermedia. Se è piena, aspetta
void buffer_put(SharedData *s, Record r) {
    pthread_mutex_lock(&s->mtx_buffer);
    
    // Aspetto finchè è vuoto E non è ancora chiuso
    while (s->count == BUFFER_SIZE && !s->buffer_closed)
        pthread_cond_wait(&s->cond_buf_not_full, &s->mtx_buffer);
    if (!s->buffer_closed) {
        // SEZIONE CRITICA: scrivo nel buffer
        s->buffer[s->in] = r;
        s->in = (s->in + 1) % BUFFER_SIZE;
        s->count++;

        // Informo i consumatori del nuovo dato in più
        pthread_cond_signal(&s->cond_buf_not_empty);
    }
    pthread_mutex_unlock(&s->mtx_buffer);
}

// ----------------- CONSUMATORE -----------------
// Preleva un record. Ritorna true se ha letto, false se è vuota o chiusa
bool buffer_get(SharedData *s, Record *r) {
    pthread_mutex_lock(&s->mtx_buffer);
    // Aspetto finchè è vuoto e non ancora chiuso
    while (s->count == 0 && !s->buffer_closed)
        pthread_cond_wait(&s->cond_buf_not_empty, &s->mtx_buffer);
    
    bool res = false;
    // Se c'è qualcosa da leggere(count > 0), leggo! 
    if (s->count > 0) {
        *r = s->buffer[s->out];
        s->out = (s->out + 1) % BUFFER_SIZE;
        s->count--;

        // Segnalo che si è liberato un posto ai produttori
        pthread_cond_signal(&s->cond_buf_not_full);
        res = true;
    }
    // Se count == 0 e buffer_closed == true, allora res rimane false (segnale di fine)

    pthread_mutex_unlock(&s->mtx_buffer);
    return res;
}

// Chiude la coda intermedia: sveglia tutti quelli che dormono
void buffer_close(SharedData *s) {
    pthread_mutex_lock(&s->mtx_buffer);
    s->buffer_closed = true;
    pthread_cond_broadcast(&s->cond_buf_not_empty);
    pthread_cond_broadcast(&s->cond_buf_not_full);
    pthread_mutex_unlock(&s->mtx_buffer);
}

// ----------------- FUNZIONI SLOT FINALE -----------------

void slot_put(SharedData *s, Record r) {
    pthread_mutex_lock(&s->mtx_slot);
    
    // Aspetto se è pieno(il main non ha ancora letto)
    while (s->slot_full && !s->slot_closed)
        pthread_cond_wait(&s->cond_slot_empty, &s->mtx_slot);
    if (!s->slot_closed) {
        s->slot_finale = r;
        s->slot_full = true;
        pthread_cond_signal(&s->cond_slot_full); // Sveglia main
    }
    pthread_mutex_unlock(&s->mtx_slot);
}

bool slot_get(SharedData *s, Record *r) {
    pthread_mutex_lock(&s->mtx_slot);
    // Aspetto se è vuoto (i verificatori non hanno trovato nulla)
    while (!s->slot_full && !s->slot_closed)
        pthread_cond_wait(&s->cond_slot_full, &s->mtx_slot);
    bool res = false;
    if (s->slot_full) {
        *r = s->slot_finale;
        s->slot_full = false; // Ho letto, ora è vuoto
        pthread_cond_signal(&s->cond_slot_empty); // Sveglia verificatori
        res = true;
    }
    pthread_mutex_unlock(&s->mtx_slot);
    return res;
}

void slot_close(SharedData *s) {
    pthread_mutex_lock(&s->mtx_slot);
    s->slot_closed = true;
    pthread_cond_broadcast(&s->cond_slot_full);
    pthread_cond_broadcast(&s->cond_slot_empty);
    pthread_mutex_unlock(&s->mtx_slot);
}

// ===================== THREADS =====================

// Strutture per passare argomenti multipli a pthread_create
typedef struct { 
    SharedData *S; 
    char *filename; 
    int id; 
} ReaderArgs;

typedef struct { 
    SharedData *S;
    int id; 
} VerifierArgs;

// ----------------- THREAD LETTORE -----------------
void *reader_thread(void *arg) {
    ReaderArgs *a = (ReaderArgs *)arg;
    
    LOCK_IO(); printf("[READER-%d] file '%s'\n", a->id, a->filename); UNLOCK_IO();

    // 1. Apertura File
    int fd = open(a->filename, O_RDONLY);
    if (fd == -1) { perror("open"); return NULL; } // Gestione errore base
    
    struct stat sb; fstat(fd, &sb); // Recupero dimensione file
    size_t fsize = sb.st_size;
    
    // 2. Mappatura in Memoria (MMAP)
    // Usiamo un puntatore map che punta ai dati del file
    unsigned char *map = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd); // Il descrittore non serve più una volta mappato

    int num_vec = fsize / M; // Numero totale di vettori nel file

    // 3. Ciclo di lettura  
    for (int i = 0; i < num_vec; i++) {
        Record r;

        // Copia di memoria grezza: dall'area mappata (file) alla struct locale
        memcpy(r.dati, map + (i*M), M);
        r.file_id = a->id; r.vector_idx = i + 1;

        LOCK_IO();
        printf("[READER-%d] vettore candidato n.%d: ", a->id, i + 1);
        print_vec(r.dati);
        printf("\n");
        UNLOCK_IO();

        // Invio alla Coda Intermedia (si blocca se piena)
        buffer_put(a->S, r);
    }
    // Pulizia mmap
    if (map != MAP_FAILED) munmap(map, fsize);

    LOCK_IO(); printf("[READER-%d] terminazione con %d vettori letti\n", a->id, num_vec); UNLOCK_IO();

    // Logica Last Man Standing per i lettori
    // Ogni lettore che finisce decrementa il contatore.
    pthread_mutex_lock(&a->S->mtx_counts);
    a->S->readers_active--;
    // Se sono l'ultimo lettore rimasto, devo chiudere la coda.
    if (a->S->readers_active == 0) buffer_close(a->S); // Sblocco i verificatori
    pthread_mutex_unlock(&a->S->mtx_counts);

    return NULL;
}

// ----------------- THREAD VERIFICATORE -----------------
void *verifier_thread(void *arg) {
    VerifierArgs *a = (VerifierArgs *)arg;
    Record r;
    int count = 0;

    // 1. Ciclo di Prelievo
    // buffer_get ritorna true finché ci sono dati. 
    // Ritorna false solo quando la coda è vuota E buffer_close() è stato chiamato dall'ultimo lettore.
    while (buffer_get(a->S, &r)) {
        count++;
        int p = 0, d = 0;
        for (int i=0; i<M; i++) { 
            if(i%2==0) p+=r.dati[i]; 
            else d+=r.dati[i]; 
        }

        LOCK_IO();
        printf("[VERIF-%d] verifico vettore: ", a->id);
        print_vec(r.dati);
        printf("\n");
        if (p == d) {
            printf("[VERIF-%d] si tratta di un vettore equisomma con somma %d!\n", a->id, p);
            UNLOCK_IO(); // Sblocco prima di put per evitare attese con lock output
            
            // 2. Trovato! Invio allo slot Finale (per il Main)
            slot_put(a->S, r);
        } else {
            printf("[VERIF-%d] non è un vettore equisomma (somma pari %d vs. dispari %d)\n", a->id, p, d);
            UNLOCK_IO();
        }
    }

    LOCK_IO(); printf("[VERIF-%d] terminazione con %d vettori verificati\n", a->id, count); UNLOCK_IO();

    // 3. Logica Last Man Standing per i verificatori
    pthread_mutex_lock(&a->S->mtx_counts);
    a->S->verifiers_active--;
    // Se sono l'ultimo verificatore, chiudo lo slot finale
    if (a->S->verifiers_active == 0) slot_close(a->S); // Questo sbloccherà il Main
    pthread_mutex_unlock(&a->S->mtx_counts);

    free(a); // Libero la memoria allocata nel main per gli argomenti
    return NULL;
}

// ----------------- MAIN -----------------
int main(int argc, char *argv[]) {
    // Controllo argomenti
    if (argc < 2) { printf("Uso: %s <f1> ...\n", argv[0]); return 1; }
    int n_readers = argc - 1; // Un reader per ogni file passato
    SharedData S;

    // 1. Inizializzazione Strutture Condivise
    init_shared(&S, n_readers);

    LOCK_IO();
    printf("[MAIN] creazione di %d thread lettori e %d thread verificatori\n", n_readers, NUM_VERIFIERS);
    UNLOCK_IO();

    pthread_t readers[n_readers], verifiers[NUM_VERIFIERS];
    ReaderArgs r_args[n_readers];

    // 2. Avvio Thread Lettori
    for (int i=0; i<n_readers; i++) {
        r_args[i].S = &S; r_args[i].filename = argv[i+1]; r_args[i].id = i+1;
        pthread_create(&readers[i], NULL, reader_thread, &r_args[i]);
    }

    // 3. Avvio Thread Verificatori
    for (int i=0; i<NUM_VERIFIERS; i++) {
        // Uso malloc per passare argomenti diversi ad ogni thread
        VerifierArgs *va = malloc(sizeof(VerifierArgs));
        va->S = &S; va->id = i+1;
        pthread_create(&verifiers[i], NULL, verifier_thread, va);
    }

    // 4. Ciclo Consumatore del Main
    Record r;
    int total = 0;

    // Il Main si comporta come un consumatore dello Slot Finale.
    // slot_get ritorna false solo quando l'ultimo verificatore chiama slot_close.
    while (slot_get(&S, &r)) {
        total++;
        LOCK_IO();
        printf("[MAIN] ricevuto nuovo vettore equisomma: ");
        print_vec(r.dati);
        printf("\n");
        UNLOCK_IO();
    }

    // 5. Attesa (Join) e Pulizia
    for(int i=0;i<n_readers;i++) pthread_join(readers[i], NULL);
    for(int i=0;i<NUM_VERIFIERS;i++) pthread_join(verifiers[i], NULL);

    LOCK_IO(); printf("[MAIN] terminazione con %d vettori equisomma trovati\n", total); UNLOCK_IO();
   
    // Distruzione mutex e cond (buona prassi)
    // (Omesse le singole chiamate a destroy per brevità, ma andrebbero qui)
    return 0;
}