#include <stdio.h>
#include <stdlib.h>

int main() {
    char a[5] = {'c','i','a','o',0};
    int i = 0;
    printf("-Stringa stampata direttamente con %%s: %s\n", a); // Con %s stampiamo tutta la stringa // %%s per mostrare a video %s 
    // Se volessimo però stampare tutta l'array di caratteri attraverso %c che permette di stampare il singolo carattere...
    printf("-Stringa stampata carattare per carattere con %%c: ");
    while(a[i] != 0) {
        printf("%c", a[i]);
        i++;
    }
    printf("\n");

    // Inizializzazione di un array di char da tastiera
    char b[] = {};
    printf("-Stringa prima inizializzata...\n");
    printf("    Inserisci i valori: \n");
    for(int j = 0; j < 4; j++) {
        printf("    -[%d]: ", j);
        scanf(" %c", &b[j]);
    }
    printf("-E poi, qui stampata direttamente con %%s: ");
    printf("%s\n", b); // Stampa tutta la stringa per intero con %s
    printf("-E qui stampato carattare per caratere con %%c: ");
    for(int z = 0; z < 4; z++) {
        printf("%c", b[z]);
    }
    printf("\n");
    // printf("Hello world!\n");
    // printf("%d", 65);
    return 0;
}