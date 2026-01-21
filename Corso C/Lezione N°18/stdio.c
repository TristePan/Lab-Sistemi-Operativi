#include <stdio.h>
#include <ctype.h>
#include "../hexdump.h"

/*
typedef struct {
    int n;
    int d;
} fract;

typedef fract *fractptr;
*/

int main(void) {

    /*
    fract f;
    fractptr fp = &f;
    f.n = 10;
    f.d = 20;
    printf("%d/%d my fraction is stored at %p\n", f.n, f.d, fp);
    */

    FILE *fp = fopen("stdio.c", "r");
    if(fp == NULL) {
        printf("Unable to open the file...\n");
        return 1;
    }
    char buf[32];
    size_t nread;
    int i = 0;
    while(1) {
        nread = fread(buf, 1, sizeof(buf),fp);
        if(nread == 0) break;
        // printf("[%d]: %zd\n", i,nread);
        // i++;
        hexdump(buf, nread);
    }
    int filesize = ftell(fp);
    printf("The real size of the file is: %d", filesize);
    fclose(fp);

    return 0;
}