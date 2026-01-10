#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define FINAL_DIMENSION 3
#define INTERMEDIATE_CAP 10
#define NUM_CELLS (DIMENSION * DIMENSION)

struct square {
    int matrix[3][3];
    int seqnum;
};

struct intermediate_queue {
    struct square buffer[INTERMEDIATE_CAP];
    int head, tail, count;
    pthread_mutex_t mtx;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
};



