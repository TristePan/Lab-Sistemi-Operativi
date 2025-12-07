#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*
 * With malloc we can allocate memory, but we need to free it after use.
*/

/*  
 * Initialize a prefixed lenght string with
 * the specified string in 'init' of length 'len' 
 * 
 * The created strings have the following layout:
 * 
 * LLLL = 4 bytes. 32 bit. 
 * +----+----+------------///
 * |LLLL|CCCC|My strings here
 * +----+----+------------///
 * 
 * Where L is one unsigned byte statig(stato) the total length of the string.
 * thus(così) in the strings we can also use the zero bytes '\0' in the middle of strings.
 * So the string are "binary safe".
 * 
 * Where C is the reference counter. Every time we add a new pointer to the string, we increase the counter.
 * Instead when we free a pointer to the string, we decrease the counter. When the counter reaches zero, we free the memory with ps_free().
 *  
 * Warning: this function does not check for buffer overflows!
*/


char *ps_create(char *init, int len) {
    char *p = malloc(4 + len + 1); // alloco memoria per la stringa con prefisso di lunghezza
    uint32_t *lenptr = (uint32_t *)p; // This is the pointer to the length of the string 
    uint32_t *refcount = (uint32_t *)(p + 4); // This is the pointer to the reference counter of the string
    *lenptr = len; // salvo la lunghezza nella prima cella di memoria
    
    p += 8; // The string start after the length and the reference counter so after 8 bytes
    for(int j = 0; j < len; j++) {
        p[j] = init[j]; // We should use memcpy() here
    }
    p[len] = 0; // opzionale: aggiungo il terminatore di stringa per facilitare la stampa con %s
    return p;
}

/* Display the string s on the screen */
void ps_print(char *p) {
    uint32_t *lenptr = (uint32_t *)(p-4);
    for(int j = 0; j < *lenptr; j++) {
        putchar(p[j]);
    }
    printf("\n");
}

/* Return the pointer to the C_style string with null terminator
char *ps_get_c_s(char *p) {
    return p + 1; // ritorno il puntatore alla prima cella della stringa (dopo il prefisso di lunghezza)
}
*/

// Free previously created PS string
void ps_free(char *p) {
    free(p - 4); // libero la memoria allocata, spostandomi indietro di 4 byte per includere il prefisso di lunghezza
}

/* Return the length of the string 0(1) time.
 * It's much faster and efficient than strlen() which is O(n) time.
*/
uint32_t ps_len(char *p) {
    uint32_t *lenptr = (uint32_t *)(p-4);
    return *lenptr;
}

char *global_string;

int main(void) {
    char *string = ps_create("Hello WorldHello WorldHello World", 33);
    global_string = string;
    ps_print(string);
    ps_print(string);
    printf("%s %d\n", string, (int)ps_len(string)); // stampa la stringa con il prefisso di lunghezza (non corretto)
    // printf("%s\n", ps_get_c_s(string)); // stampa la stringa senza il prefisso di lunghezza
    ps_free(string);
    printf("%s\n", global_string); // comportamento indefinito: la memoria è stata liberata
    return 0;
}

/*
 * Utilizzando i puntatori posso creare degli oggetti 
 * come "char *string = ps_create("Hello WorldHelloWorldHello World", 33);"
 * che posso manipolare, farci cose, liberarli.
*/