#include <stdio.h>
#include <stdlib.h>
/*
 * With malloc we can allocate memory, but we need to free it after use.
*/

/*  
 * Initialize a prefixed lenght string with
 * the specified string in 'init' of length 'len' 
 * 
 * The created strings have the following layout:
 * 
 * +-+------------///
 * |L|My strings here
 * +-+------------///
 * 
 * Where L is one unsigned byte statig(stato) the total length of the string.
 * thus(così) in the strings we can also use the zero bytes '\0' in the middle of strings.
 * So the string are "binary safe".
 * 
 * Warning: this function does not check for buffer overflows!
*/
char *ps_create(char *init, int len) {
    char *p = malloc(1 + len + 1); // alloco memoria per la stringa con prefisso di lunghezza
    unsigned char *lenptr = (unsigned char *)p; // il pointer è sempre l'indirizzo di memoria di p, ma con un tipo diverso
    *lenptr = len; // salvo la lunghezza nella prima cella di memoria
    for(int j = 0; j < len; j++) {
        p[j + 1] = init[j]; // copio i caratteri a partire dalla cella successiva. We should use memcpy() here
    }
    p[len + 1] = 0; // opzionale: aggiungo il terminatore di stringa per facilitare la stampa con %s
    return p;
}

/* Display the string s on the screen */
void ps_print(char *p) {
    unsigned char *lenptr = (unsigned char *)p;
    for(int j = 0; j < *lenptr; j++) {
        putchar(p[j + 1]);
    }
    printf("\n");
}

/* Return the pointer to the C_style string with null terminator */
char *ps_get_c_s(char *p) {
    return p + 1; // ritorno il puntatore alla prima cella della stringa (dopo il prefisso di lunghezza)
}


int main(void) {
    char *string = ps_create("Hello WorldHelloWorldHello World", 33);
    // ps_print(string);
    ps_print(string);
    ps_print(string);
    printf("%s\n", string); // stampa la stringa con il prefisso di lunghezza (non corretto)
    printf("%s\n", ps_get_c_s(string)); // stampa la stringa senza il prefisso di lunghezza
    free(string); // libero la memoria allocata
    return 0;
}

/*
 * Utilizzando i puntatori posso creare degli oggetti 
 * come "char *string = ps_create("Hello WorldHelloWorldHello World", 33);"
 * che posso manipolare, farci cose, liberarli.
*/