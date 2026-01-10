#include <stdio.h>

int main(void) {
    char str[] = "\017 Hello00\0000000123";
    char *p = str;
    printf("My string len is %d\n", p[0]);
    int len = *p++;
    for(int j = 0; j < len; j++) {
        putchar(p[j]);
    }
    printf("\n");
    
    return 0;

}