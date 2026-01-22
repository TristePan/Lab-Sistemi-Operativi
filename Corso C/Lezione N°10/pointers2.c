#include <stdio.h>

int main(void) {
    /*
    int x = 5;
    int *y = &x;
    int **z = &y; // un puntatore a un puntatore ad un intero

    printf("x is stored in %p and y is stored in %p\n", y, z);
    printf("%d ,%d, %d\n", (int)sizeof(x), (int)sizeof(y), (int)sizeof(z));
    */

    char mystr[] = "Hello, World!";
    char *p = mystr; // puntatore al primo carattere della stringa
    
    /*
    for(int i = 0; i < 13; i++) {
        printf("%c", *(p + i));
    }
    */
    
    printf("\n");
    
    printf("At the beginnig p is: %p\n", p);
    while (*p != 0) {
        putchar(*p);
        p++;
    }
    printf("\n");
    printf("At the end p is: %p\n", p);

    // printf("%c\n", p); // *p è equivalente a p[0], quindi stampa il primo carattere della stringa
    // printf("At %p I can see: %s\n", mystr, mystr);
    return 0;
}