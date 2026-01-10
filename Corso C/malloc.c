#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr;
    
    // Alloca spazio per un singolo intero
    ptr = (int *)malloc(sizeof(int));
    
    // ptr = NULL;
    if (ptr == NULL) {
        printf("Errore: memoria insufficiente!\n");
        return 1;
    }
    
    *ptr = 42;
    printf("Valore: %d\n", *ptr);
    
    free(ptr);  // Libera la memoria
    
    return 0;
}