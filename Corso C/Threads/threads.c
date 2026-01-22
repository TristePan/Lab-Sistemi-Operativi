#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void *routine(void *arg) {
    char *msg = (char *)arg;
    printf("[THREAD] Inizio esecuzione con messaggio: %s\n", msg);
    sleep(5);
    printf("[THREAD] Esecuzione terminata.\n");

    return NULL;
}

int main() {
    pthread_t t; // identificatore del thread
    char *msg = "Benvenuto nel mondo del thread";

    printf("[MAIN] Creo il thread...\n");

    /* Creazione del thread:
        1. &t: puntatore all'identificatore;
        2. NULL: attributi predefiniti;
        3. &routine: la funzione che deve eseguire
        4. (void *)msg: l'argomento da passare alla funzione
    */
    
    if(pthread_create(&t, NULL, &routine, (void *)msg) != 0) {
        perror("Errore nella creazione del thread");
        return 1;
    }

    printf("[MAIN] Thread creato. Ora attendo la sua fine con pthread_join...\n");

    /*  pthread_join è bloccante: il main si ferma finchè t non finisce.
        È essenziale per evitare che il processo termini prima dei suoi thread. */

    if(pthread_join(t, NULL) != 0) {
        perror("Errore nel join del thread");
        return 2;
    }

    printf("[MAIN] Il thread ha finito. Esco dal programma");

    return 0;
}