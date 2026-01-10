#include <stdio.h>
#include <pthread.h>

int counter_globale = 0;
// Inizializzazione statica del Mutex [cite: 638]
pthread_mutex_t mio_mutex = PTHREAD_MUTEX_INITIALIZER;

void* incrementa(void* arg) {
    for(int i = 0; i < 100000; i++) {
        // Entrata in sezione critica [cite: 633]
        pthread_mutex_lock(&mio_mutex);
        
        counter_globale++; // Ora l'accesso è protetto!
        
        // Uscita dalla sezione critica [cite: 634]
        // pthread_mutex_unlock(&mio_mutex);
        pthread_mutex_trylock(&mio_mutex);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, incrementa, NULL);
    pthread_create(&t2, NULL, incrementa, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Valore finale: %d\n", counter_globale);
    return 0;
}