#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>

#define M 12            // Dimensione vettore (byte)
#define BUFFER_SIZE 10  // Dimensione buffer
#define NUM_VERIFIERS 3 // Numero consumatori

// ===================== RECORD =====================
// Il dato che viaggia nel buffer
typedef struct {
    unsigned char dati[M];
    int file_id;    // Per l'output richiesto: ID del file
    int vector_idx; // Per l'output richiesto: indice del vettore
} Record;

// ===================== BUFFER (MONITOR) =====================
// Questa è la struttura "stile codice semplice" che ti piaceva.
// Incapsula tutto: buffer, indici e sincronizzazione.
typedef struct {
    Record buffer[BUFFER_SIZE];
    int in;     // Testa (dove scrivere)
    int out;    // Coda (dove leggere)
    int count;  // Numero elementi presenti
    bool closed;// Flag per terminazione pulita (al posto delle poison pill)

    pthread_mutex_t mutex;
    pthread_cond_t not_empty; // Aspetta se vuoto
    pthread_cond_t not_full;  // Aspetta se pieno
} Shared_buffer;

// Inizializzazione
void buffer_init(Shared_buffer *b) {
    b->in = 0;
    b->out = 0;
    b->count = 0;
    b->closed = false;
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->not_empty, NULL);
    pthread_cond_init(&b->not_full, NULL);
}

// Distruzione
void buffer_destroy(Shared_buffer *b) {
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->not_empty);
    pthread_cond_destroy(&b->not_full);
}

// Chiusura del buffer: sveglia tutti i consumatori bloccati!
void buffer_close(Shared_buffer *b) {
    pthread_mutex_lock(&b->mutex);
    b->closed = true;
    pthread_cond_broadcast(&b->not_empty); // SVEGLIA TUTTI I CONSUMATORI
    pthread_cond_broadcast(&b->not_full);  // Sveglia eventuali produttori (non serve qui ma è corretto)
    pthread_mutex_unlock(&b->mutex);
}

// Inserimento (Produttore)
void buffer_in(Shared_buffer *b, Record r) {
    pthread_mutex_lock(&b->mutex);
    
    // Attesa finché il buffer è pieno E non è chiuso
    while (b->count == BUFFER_SIZE && !b->closed) {
        pthread_cond_wait(&b->not_full, &b->mutex);
    }

    if (b->closed) {
        pthread_mutex_unlock(&b->mutex);
        return; // Se chiuso, esco senza inserire
    }

    b->buffer[b->in] = r;
    b->in = (b->in + 1) % BUFFER_SIZE;
    b->count++;

    pthread_cond_signal(&b->not_empty); // C'è un dato in più!
    pthread_mutex_unlock(&b->mutex);
}

// Estrazione (Consumatore)
// Ritorna true se ha letto un dato, false se il buffer è chiuso e vuoto
bool buffer_out(Shared_buffer *b, Record *r) {
    pthread_mutex_lock(&b->mutex);

    // Attesa finché il buffer è vuoto E non è chiuso
    while (b->count == 0 && !b->closed) {
        pthread_cond_wait(&b->not_empty, &b->mutex);
    }

    // Se è vuoto E chiuso, abbiamo finito
    if (b->count == 0 && b->closed) {
        pthread_mutex_unlock(&b->mutex);
        return false;
    }

    *r = b->buffer[b->out];
    b->out = (b->out + 1) % BUFFER_SIZE;
    b->count--;

    pthread_cond_signal(&b->not_full); // C'è uno slot libero in più!
    pthread_mutex_unlock(&b->mutex);
    return true;
}

// ===================== HELPER =====================
// Funzione per controllare l'equisomma
bool check_equisum(unsigned char *v) {
    int pari = 0, dispari = 0;
    for (int i = 0; i < M; i++) {
        if (i % 2 == 0) pari += v[i];
        else            dispari += v[i];
    }
    return (pari == dispari);
}

// Argomenti per il Reader
typedef struct {
    Shared_buffer *buf;
    char *filename;
    int id;
} ReaderArgs;

// ===================== READER (Produttore) =====================
void *reader_thread(void *arg) {
    ReaderArgs *a = (ReaderArgs *)arg;
    int fd;
    struct stat sb;
    unsigned char *map;

    printf("[READER-%d] Apro %s\n", a->id, a->filename);

    if ((fd = open(a->filename, O_RDONLY)) == -1) {
        perror("Errore open");
        return NULL;
    }
    if (fstat(fd, &sb) == -1) {
        perror("Errore fstat");
        close(fd);
        return NULL;
    }

    // MAPPA IL FILE (Richiesta fondamentale dell'esame)
    map = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        perror("Errore mmap");
        close(fd);
        return NULL;
    }
    close(fd); // Il fd non serve più dopo mmap

    int num_vectors = sb.st_size / M;

    for (int i = 0; i < num_vectors; i++) {
        Record r;
        // Copio i dati dalla memoria mappata al record locale
        memcpy(r.dati, map + (i * M), M);
        r.file_id = a->id;
        r.vector_idx = i + 1;

        // Inserisco nel buffer (gestione concorrenza nascosta dentro buffer_in)
        buffer_in(a->buf, r);
        
        // printf("[READER-%d] Inserito vettore %d\n", a->id, i+1); // Decommenta per debug
    }

    munmap(map, sb.st_size);
    printf("[READER-%d] Terminato.\n", a->id);
    return NULL;
}

// ===================== VERIFIER (Consumatore) =====================
void *verifier_thread(void *arg) {
    Shared_buffer *buf = (Shared_buffer *)arg;
    int id = (int)(long)pthread_self(); // ID fittizio per log
    Record r;

    // Il ciclo while usa il valore di ritorno booleano di buffer_out
    // Appena buffer_out ritorna false (buffer chiuso e vuoto), il thread esce.
    while (buffer_out(buf, &r)) {
        if (check_equisum(r.dati)) {
            // Se equisomma, stampo (output richiesto)
            // Nota: per perfezione andrebbe un mutex sulla printf, ma qui semplifichiamo
            printf("[VERIFIER] Trovato equisomma! File %d, Vettore %d\n", r.file_id, r.vector_idx);
        }
    }

    printf("[VERIFIER] Terminato.\n");
    return NULL;
}

// ===================== MAIN =====================
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    int num_readers = argc - 1;
    Shared_buffer buffer;
    
    // 1. Inizializzo il monitor
    buffer_init(&buffer);

    pthread_t readers[num_readers];
    pthread_t verifiers[NUM_VERIFIERS];
    ReaderArgs r_args[num_readers];

    // 2. Avvio i Verificatori
    // Nota: passiamo solo il puntatore al buffer, molto pulito.
    for (int i = 0; i < NUM_VERIFIERS; i++) {
        pthread_create(&verifiers[i], NULL, verifier_thread, &buffer);
    }

    // 3. Avvio i Lettori
    for (int i = 0; i < num_readers; i++) {
        r_args[i].buf = &buffer;
        r_args[i].filename = argv[i + 1];
        r_args[i].id = i + 1;
        pthread_create(&readers[i], NULL, reader_thread, &r_args[i]);
    }

    // 4. Attesa Lettori
    // Il main aspetta che TUTTI i lettori finiscano.
    for (int i = 0; i < num_readers; i++) {
        pthread_join(readers[i], NULL);
    }
    printf("[MAIN] Tutti i lettori hanno finito.\n");

    // 5. Chiusura Buffer
    // Ora che non c'è più nessuno che scrive, dichiaro chiuso il buffer.
    // Questo farà ritornare 'false' a buffer_out nei verificatori appena svuotano la coda.
    buffer_close(&buffer);

    // 6. Attesa Verificatori
    for (int i = 0; i < NUM_VERIFIERS; i++) {
        pthread_join(verifiers[i], NULL);
    }
    printf("[MAIN] Tutti i verificatori hanno finito. Esco.\n");

    buffer_destroy(&buffer);
    return 0;
}