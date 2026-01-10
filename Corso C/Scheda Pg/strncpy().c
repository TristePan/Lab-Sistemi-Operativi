#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char origine[] = "Ciao fra come va, tutto?";
    char destinazione[20];

    strcpy_s(destinazione, sizeof(origine), origine);
    printf("%s\n", destinazione);

    return 0;
}