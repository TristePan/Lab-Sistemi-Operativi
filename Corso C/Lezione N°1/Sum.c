#include <stdio.h>

int sum(int a, int b) { // funzione che somma due numeri interi; a e b sono parametri formali
    int c; // variabile locale

    c = a + b;
    return c;
}

int main(void) {
    printf("Ehi questo e' il risultato: %d\n", sum(10, 20));
    return 0;
}