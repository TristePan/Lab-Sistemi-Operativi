#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>

#define MAX_M 16
#define CAPIENZA_CODA 5

/* Struttura base per una matrice */
typedef struct {
    unsigned char dati[MAX_M][MAX_M];
    int id_lettore;   /* Utile per l'output: [READER-ID] */
    int n_sequenza;   /* Utile per l'output: quadrato candidato n.X */
    int size_m;       /* Dimensione effettiva M */
} Matrice;

/* Monitor per la Coda Intermedia (Reader -> Verifier) */
typedef struct {
    Matrice buffer[CAPIENZA_CODA];
    int testa;
    int coda;
    int count;              /* Numero di elementi presenti */
    int lettori_attivi;     /* Contatore per la terminazione */
    pthread_mutex_t mutex;
    pthread_cond_t non_pieno;
    pthread_cond_t non_vuoto;
} CodaIntermedia;

/* Struttura per il passaggio al Main (Verifier -> Main) */
typedef struct {
    Matrice matrice;
    int piena;              /* 0 = vuota, 1 = contiene dato */
    int finito;             /* Flag di terminazione dal Verifier */
    pthread_mutex_t mutex;
    pthread_cond_t main_ready; /* Main aspetta dato */
    pthread_cond_t verif_done; /* Verifier aspetta che Main legga */
} CodaFinale;

/* Struttura per passare gli argomenti ai thread lettori */
typedef struct {
    int id;                 /* ID del lettore (es. 1, 2...) */
    char *nome_file;        /* Il nome del file da aprire */
    CodaIntermedia *coda;   /* Puntatore alla coda condivisa dove scrivere */
} ReaderArgs;

void *reader_thread(void *arg) {
    /* Setup argomenti */

    /* 1. CASTING: Recuperiamo la struttura dagli argomenti generici */
    ReaderArgs *args = (ReaderArgs*)arg;

    int fd = open(args, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    struct stat sb;
    if(fstat(fd, &sb) == -1) {
        perror(fstat);
        exit(1);
    }

}