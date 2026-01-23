#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../hexdump.h"

/*
typedef struct {
    int n;
    int d;
} fract;

typedef fract *fractptr;
*/

int main(void) {

    int fd = open("stdio2.c", O_RDONLY); // fd = ??
    printf("Error number is %d %d\n: ", errno, ENOENT);
    if(fd == -1 ) {
        perror("Error: Unable to open file");
        return 1;
    }
    
    char buf[32];
    ssize_t nread; // Con read(Chiamata di sistema) può restituire -1, quindi con segno
    int i = 0;
    while(1) {
        nread = read(fd, buf, sizeof(buf));
        if(nread == 0) break;
        // printf("[%d]: %zd\n", i,nread);
        // i++;
        hexdump(buf, nread);
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