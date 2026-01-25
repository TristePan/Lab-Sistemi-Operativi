#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#define MAX_SIZE 5
#define MAX_ROUND 10

/* ===================== RECORD ===================== */
typedef struct {
    int *vector;
    int round;
} Record;

/* ===================== BUFFER ===================== */
typedef struct {
    Record buffer[MAX_SIZE];
    int in, out, count;
    bool close;

    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} Shared_buffer;

void buffer_init(Shared_buffer *b) {
    b->in = b->out = b->count = 0;
    b->close = false;
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->not_empty, NULL);
    pthread_cond_init(&b->not_full, NULL);
}

void buffer_close(Shared_buffer *b) {
    pthread_mutex_lock(&b->mutex);
    b->close = true;
    pthread_cond_broadcast(&b->not_empty);
    pthread_cond_broadcast(&b->not_full);
    pthread_mutex_unlock(&b->mutex);
}

void buffer_in(Shared_buffer *b, Record r) {
    pthread_mutex_lock(&b->mutex);
    while (b->count == MAX_SIZE && !b->close)
        pthread_cond_wait(&b->not_full, &b->mutex);

    if (b->close) {
        pthread_mutex_unlock(&b->mutex);
        return;
    }

    b->buffer[b->in] = r;
    b->in = (b->in + 1) % MAX_SIZE;
    b->count++;

    pthread_cond_signal(&b->not_empty);
    pthread_mutex_unlock(&b->mutex);
}

bool buffer_out(Shared_buffer *b, Record *r) {
    pthread_mutex_lock(&b->mutex);
    while (b->count == 0 && !b->close)
        pthread_cond_wait(&b->not_empty, &b->mutex);

    if (b->count == 0 && b->close) {
        pthread_mutex_unlock(&b->mutex);
        return false;
    }

    *r = b->buffer[b->out];
    b->out = (b->out + 1) % MAX_SIZE;
    b->count--;

    pthread_cond_signal(&b->not_full);
    pthread_mutex_unlock(&b->mutex);
    return true;
}

/* ===================== UTILS ===================== */
bool equisum(int *vector, int N)
{
    int even = 0;
    int odd = 0;

    for(int i = 0; i < N; ++i)
    {
        if(i % 2 == 0)
        {
            even += vector[i];
        }
        else
        {
            odd += vector[i];
        }
    }

    return even == odd;
}

void random_vector(int *v, int N) {
    for (int i = 0; i < N; ++i)
        v[i] = rand() % 100;
}

void regenerate(int *v, int N) {
    v[rand() % N] = rand() % 100;
}

/* ===================== CONTEXT ===================== */
typedef struct {
    pthread_mutex_t print_mutex;
} Shared_context;

void atomic_print(const char *tag, int *vector, int N,
                  Shared_context *ctx, Record *r)
{
    pthread_mutex_lock(&ctx->print_mutex);

    if (r)
        printf("%s | round=%d\n", tag, r->round);
    else
        printf("%s\n", tag);

    if (vector)
        for (int i = 0; i < N; ++i)
            printf("  elemento[%d] = %d\n", i, vector[i]);

    printf("\n");
    fflush(stdout);
    pthread_mutex_unlock(&ctx->print_mutex);
}

/* ===================== PRODUCER ===================== */
typedef struct {
    Shared_buffer *proposte;
    Shared_context *ctx;
    int num_prop;
    int N;
} Prod_args;

void *producer(void *arg) {
    Prod_args *a = arg;
    for (int i = 0; i < a->num_prop; ++i) {
        Record r;
        r.round = 0;
        r.vector = malloc(sizeof(int) * a->N);
        random_vector(r.vector, a->N);

        atomic_print("[PRODUCER]", r.vector, a->N, a->ctx, &r);
        buffer_in(a->proposte, r);
    }

    atomic_print("[PRODUCER] terminato", NULL, 0, a->ctx, NULL);
    return NULL;
}

/* ===================== VERIFIER ===================== */
typedef struct {
    Shared_buffer *proposte;
    Shared_buffer *scarti;
    Shared_context *ctx;
    pthread_mutex_t *count_mutex;
    int *equisum;
    int N;
} Ver_args;

void *verifier(void *arg) {
    Ver_args *a = arg;
    Record r;

    while (buffer_out(a->proposte, &r)) {
        if (equisum(r.vector, a->N)) {
            pthread_mutex_lock(a->count_mutex);
            (*a->equisum)++;
            pthread_mutex_unlock(a->count_mutex);

            atomic_print("[VERIFIER] accettato", r.vector, a->N, a->ctx, &r);
            free(r.vector);
        } else {
            atomic_print("[VERIFIER] scartato", r.vector, a->N, a->ctx, &r);
            buffer_in(a->scarti, r);
        }
    }

    atomic_print("[VERIFIER] terminato", NULL, 0, a->ctx, NULL);
    return NULL;
}

/* ===================== REPAIRER ===================== */
typedef struct {
    Shared_buffer *proposte;
    Shared_buffer *scarti;
    Shared_context *ctx;
    int N;
} Rep_args;

void *repairer(void *arg) {
    Rep_args *a = arg;
    Record r;

    while (buffer_out(a->scarti, &r)) {
        if (r.round >= MAX_ROUND) {
            atomic_print("[REPAIRER] scarto definitivo", r.vector, a->N, a->ctx, &r);
            free(r.vector);
            continue;
        }

        atomic_print("[REPAIRER] riparo", r.vector, a->N, a->ctx, &r);
        r.round++;
        regenerate(r.vector, a->N);
        atomic_print("[REPAIRER] riparato", r.vector, a->N, a->ctx, &r);

        buffer_in(a->proposte, r);
    }

    atomic_print("[REPAIRER] terminato", NULL, 0, a->ctx, NULL);
    return NULL;
}

/* ===================== MAIN ===================== */
int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc != 4) {
        printf("Uso: %s <N> <T> <R>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]); // dimensione vettore
    int T = atoi(argv[2]); // numero vettori da produrre
    int R = atoi(argv[3]); // numero repairer

    Shared_buffer proposte, scarti;
    buffer_init(&proposte);
    buffer_init(&scarti);

    Shared_context ctx;
    pthread_mutex_init(&ctx.print_mutex, NULL);

    int equisum_count = 0;
    pthread_mutex_t count_mutex;
    pthread_mutex_init(&count_mutex, NULL);

    pthread_t prod, ver, rep[R];

    Prod_args pa = { &proposte, &ctx, T, N };
    Ver_args va = { &proposte, &scarti, &ctx, &count_mutex, &equisum_count, N };
    Rep_args ra = { &proposte, &scarti, &ctx, N };

    pthread_create(&prod, NULL, producer, &pa);
    pthread_create(&ver, NULL, verifier, &va);
    for (int i = 0; i < R; ++i)
        pthread_create(&rep[i], NULL, repairer, &ra);

    pthread_join(prod, NULL);
    buffer_close(&proposte);

    pthread_join(ver, NULL);
    buffer_close(&scarti);

    for (int i = 0; i < R; ++i)
        pthread_join(rep[i], NULL);

    printf("\nVettori equisum trovati: %d\n", equisum_count);
    return 0;
}
