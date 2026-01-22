#include <stdio.h>

/*  
    Initialize a prefixed lenght string with 
    the specified string in 'init' of length 'len' 
*/
void ps_init(char *p, char *init, size_t len) {

    unsigned char *lenptr = (unsigned char *)p; // il pointer è sempre l'indirizzo di memoria di p, ma con un tipo diverso
    *lenptr = len; // salvo la lunghezza nella prima cella di memoria
    for(int j = 0; j < len; j++) {
        p[j + 1] = init[j]; // copio i caratteri a partire dalla cella successiva
        
    }
}

/* Display the string s on the screen */
void ps_print(char *p) {
    unsigned char *lenptr = (unsigned char *)p;
    for(int j = 0; j < *lenptr; j++) {
        putchar(p[j + 1]);
    }
    printf("\n");
}


int main(void) {
    char buf[256];
    ps_init(buf, "Hello World!", 11);
    ps_print(buf);
    ps_print(buf);
    return 0;
}