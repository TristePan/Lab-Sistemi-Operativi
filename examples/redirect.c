/* Effettua la manipolazione dei canali standard output ed error */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define LOGNAME "file.log"

int main() {
    int fd;

    printf("messaggio n.1 sullo standard output\n");
    dprintf(STDERR, "errore n.1 simulato\n");

    if ((fd = open(LOGNAME, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1) {
        perror(LOGNAME);
        exit(1);
    }

    close(STDOUT);
    dup(fd);
    printf("messaggio n.2 sullo standard output\n");
    dprintf(STDERR, "errore n.2 simulato\n");

    close(STDERR);
    dup(fd);
    printf("messaggio n.3 sullo standard output\n");
    dprintf(STDERR, "errore n.3 simulato\n");
}
