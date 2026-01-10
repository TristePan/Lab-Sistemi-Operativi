#include <stdio.h>
#include <limits.h>
#include <stdint.h>


int main() {
    int x = 21; // 4 bytes -> 32 bit
    char c = 1; // 1 byte -> 8 bit(intorno)
    short s = 2000; // 2 byte -> 16 bit
    long l = 10;
    long long ll = 10; // 8 bytes -> 64 bit
    int32_t t32 = 10; // 4 bytes -> 32 bit
    int64_t t64 = 10; // 8 bytes -> 64 bit


    printf("%d\n", x);
    printf("Variable 'l' is %lu bytes\n", sizeof(l));
    printf("long long is %lu bytes\n", sizeof(long long));
    printf("int32_t is %lu bytes\n", sizeof(int32_t));
    printf("int64_t is %lu bytes\n", sizeof(int64_t));
    printf("int is %lu bytes\n", sizeof(int));
    printf("Variable x is %lu bytes\n", sizeof(x));
    printf("Hello World: int min is %d bytes and int max %d\n", INT_MAX, INT_MIN);
    return 0;
}  