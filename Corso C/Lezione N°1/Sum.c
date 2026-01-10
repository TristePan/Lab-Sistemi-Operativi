#include <stdio.h>

int x = 0; // variabile globale

int sum(int a, int b) { // funzione che somma due numeri interi; a e b sono parametri formali
    int c; // variabile locale

    c = a + b;
    return c;
}

int incr(int x) {
    for(int i = 0; i < 3; i++) {
        x++;
    }
    return x;
}


int main(void) {
    int a = 10;
    // incr(a) // il valore di a passa per valore, quindi la funzione riceverà una copia del dato 'a' e non il dato originale
    a = incr(a); // In questo caso invece, andremo sempre a chiamare la funzione 'incr' con la copia di 'a' ma il valore finale verrà assegnato ad 'a'
    printf("a = %d\n", incr(a));
    printf("a = %d\n", a);
    
    printf("Somma: %d\n", sum(30, 15));

    char str[] = "ciao";
    for(int i = 0; i < 1; i++) {
        printf("%s",str);
    }
    return 0;
}   