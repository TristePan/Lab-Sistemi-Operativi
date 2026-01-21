#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h> // Non esiste su Windows ma solo su linux
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define M 12 // Definisce la dimensione dei vettore  
#define CAP_CODA 10 // Definisce la dimensione della coda intermedia (pdf esame)
#define THREAD_VERF 3 // Definisce i 3 thread verificatori

typedef struct {
    uint8_t v[M]; // il vettore con i 12 byte
    int is_end // Terminatore speciale: 0 se è normale, 1 per terminare
} record_t

