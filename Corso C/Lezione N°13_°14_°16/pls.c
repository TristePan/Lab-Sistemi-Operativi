#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

struct pls {
    uint32_t len;
    uint32_t refcount;
    uint32_t magic;
    char str[]; // La grandezza verrà determinata con malloc poi;
};

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
    struct pls *p = malloc(sizeof(struct pls) + len + 1); // alloco memoria per la stringa
    p -> len = len;
    p -> refcount = 1;
    p -> magic = 0xDEADBEEF;    
    for(int j = 0; j < len; j++) {
        p -> str[j] = init[j]; // We should use memcpy() here
    }
    p -> str[len] = 0; // opzionale: aggiungo il terminatore di stringa per facilitare la stampa con %s
    return p -> str;
}

/* Display the string s on the screen */
void ps_print(char *p) {
    struct pls *s = (struct pls *)(p - sizeof(*s));
    for(int j = 0; j < s -> len; j++) {
        putchar(s -> str[j]);
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
    free(p - sizeof(struct pls)); // libero la memoria allocata, spostandomi indietro di 4 byte per includere il prefisso di lunghezza
}

// validate that a PS string look valid
void ps_validate(struct pls *p) {
    if(p -> magic != 0xDEADBEEF) {
        printf("INVALID STRING: aborting\n");
        exit(1);
    }
}

void ps_release(char *p) {
    struct pls *s = (struct pls *)(p - sizeof(*s));
    // printf("Current refcount is: %d\n", (int)s -> refcount);
    ps_validate(s);

    s -> refcount--;
    if(s -> refcount == 0) {
        s -> magic = 0;
        ps_free(p);
    }
}

// Incrementa il conto delle referenze alla stringa
void ps_retain(char *p) {
    struct pls *s = (struct pls *)(p - sizeof(*s));
    if(s -> refcount == 0) {
        printf("ABORT ON RETAIN OF ILLEGAL ERROR\n");
        exit(1);
    }
    s -> refcount++;
}

/* Return the length of the string 0(1) time.
 * It's much faster and efficient than strlen() which is O(n) time.
*/
uint32_t ps_len(char *p) {
    struct pls *s = (struct pls *)(p - sizeof(*s));
    return s -> len;
}

char *global_string;

int main(void) {
    char *string = ps_create("Hello WorldHello WorldHello World", 33);
    global_string = string;
    ps_retain(string);

    ps_print(string);
    ps_print(string);
    printf("%s %d\n", string, (int)ps_len(string)); // stampa la stringa con il prefisso di lunghezza (non corretto)
    ps_release(string);
    printf("%s\n", global_string); // comportamento indefinito: la memoria è stata liberata
    ps_release(string);
    // ps_release(string);
    return 0;
}

/*
 * Utilizzando i puntatori posso creare degli oggetti 
 * come "char *string = ps_create("Hello WorldHelloWorldHello World", 33);"
 * che posso manipolare, farci cose, liberarli.
*/