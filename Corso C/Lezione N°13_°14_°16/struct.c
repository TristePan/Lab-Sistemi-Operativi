#include <stdio.h>
#include <stdlib.h>

/*
int set_fraction_num(int *f, int num) {
    f[0] = num;
}

int set_fraction_den(int *f, int den) {
    f[1] = dem;
}

Uso le struct, molto più pulite e comode:
*/


/* Iternal layout:
 * 
 * +-+----+----+
 * |c|num |den |
 * +-+----+----+
 * 
*/ 
typedef struct {
    unsigned char color;
    int num;
    int den;
}fract;



/* Create a new fraction, setting num and den as the numerator
 * and denominator of the fraction.
 * The function return NULL on out of memory, otherwise the
 * faction object is returned */
fract *create_fraction(int num, int den) {
    fract *f = malloc(sizeof(*f));
    if(f == NULL) {
        return NULL; // malloc error checking.
    }
    f -> num = num;
    f -> den = den;  
    
    return f;
}
/* Simplify the provided fraction */
void simplify_fraction(fract *f) { 
    for(int d = 2; d <= f -> num && d <= f -> den; d++) {
        while(f -> num % d == 0 &&
           f -> den % d == 0) {
            f -> num /= d;
            f -> den /= d;
        }
    }
}

void print_fraction(fract *f) {
    printf("%d\n-\n%d\n", f -> num, f -> den);
}

int main(void) {

#if 0
    printf("%d\n\n", (int)sizeof(struct fract));

    struct fract a;
    // a.num = 1;
    // a.den = 2;
    struct fract *b = &a;
    /* Quando stiamo usando un puntatore ad una struct si usa l'operatore "->" */
    b -> num = 1;
    b -> den = 2;

    printf("%d\n-\n%d\n", a.num, a.den);
#endif

    /*
        int *f = malloc(sizeof(int) * 2);
        set_fraction(f, 1, 2);
    */
    fract *f1 = create_fraction(3, 4);
    fract *f2 = create_fraction(10, 20);
    
    printf("Fraction f1:\n");    
    print_fraction(f1);

    printf("\n");

    printf("Fraction f2:\n");
    printf("In this fraction there is a semplification of the num and den.\n\nBefore simplification:\n");
    print_fraction(f2);
    simplify_fraction(f2);
    printf("\nAfter simplification:\n");
    print_fraction(f2);
    
    return 0;
}