#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include "../hexdump.h"

/*
typedef struct {
    int n;
    int d;
} fract;

typedef fract *fractptr;
*/

int main(void) {

    int fd = open("stdio3.c", O_RDONLY);
    if(fd == -1 ) {
        perror("Error: Unable to open file");
        return 1;
    }
    close(fd); // Chiamata di sistema per chiudere un file;

    return 0;
    /*
    fract f;
    fractptr fp = &f;
    f.n = 10;
    f.d = 20;
    printf("%d/%d my fraction is stored at %p\n", f.n, f.d, fp);
    */
}