#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 16 byte
struct line {
    char *s;
    struct line *next;
    };
    
int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Missing file name\n");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if(fp == NULL) {
        printf("File does not exist\n");
        return 1;
    }

    char buffer[2048];
    struct line *head = NULL;
    while(fgets(buffer, sizeof(buffer), fp) != NULL) {
        struct line *l = malloc(sizeof(struct line));
        size_t linelen = strlen(buffer);
        l -> s = malloc(linelen + 1);
        for(size_t j = 0; j <= linelen; j++) {
            l -> s[j] = buffer[j];
        }
        l -> next = head;
        head = l;
    }

    while(head != NULL) {
        printf("%s", head -> s);
        head = head -> next;
    }

    fclose(fp);
    return 0;
}