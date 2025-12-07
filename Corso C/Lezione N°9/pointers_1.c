#include <stdio.h>

void incr(int *p) {
    *p = *p + 1;
}

int main() {
    int x = 5;
    int *y = NULL; // Un puntatore a intero. Indirizzo nullo. Per settare a 0 va messo NULL. 16 bit.09
    
    printf("The value of x is: %d\n", x);

    y = &x;     // Assegno a y l'indirizzo di memoria di x. L'operatore & restituisce l'indirizzo di memoria della variabile x.
    incr(y);    // Passo a incr y che punta all'indirizzo di x. 
                // La funzione incr incrementa il valore di x di 1.
    
    /*
    printf("The value of y is: %p\n", y);

    // Dove abita x in memoria? Con l'operatore & posso scoprirlo.
    printf("x is stored at the address: %p\n", p);

    //*p = 10;  // All'indirizzo di memoria che è contenuto in p, assegna il valore 10. *p = 10 == p[0] = 10
    p[1] = 10;  // All'indirizzo di memoria successivo a quello contenuto in p, assegna il valore 10.
                // In questo modo sto andando a scrivere in una zona di memoria non allocata per x.
                // Comportamento indefinito (undefined behavior).
    */
        
    printf("The value of x is now: %d\n", x);
    return 0;
}