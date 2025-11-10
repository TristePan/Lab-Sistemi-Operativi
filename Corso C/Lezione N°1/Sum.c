#include <stdio.h>

int x = 0; // variabile globale



int sum(int a, int b) { // funzione che somma due numeri interi; a e b sono parametri formali
    int c; // variabile locale

    c = a + b;
    return c;
}

int incr(int x) {
    x = x + 1;
    return x;
}


int main(void) {
    int a = 10;
    a = incr(a);
    a = incr(a);
    a = incr(a);
    printf("a = %d\n", a);
    return 0;
}