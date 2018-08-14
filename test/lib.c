#include <stdio.h>

void print_ok(void) {
    printf("OK\n");
}

void add_one(int i) {
    printf("%d\n", i+1);
}

void add(int x, int y) {
    printf("%d\n", x+y);
}

void print_all_args(int a, int b, int c, int d, int e, int f) {
    printf("%d%d%d%d%d%d\n", a,b,c,d,e,f);
}