#include <stdio.h>
#include <stdlib.h>

void allocate4(int **p, int a, int b, int c, int d) {
    *p = malloc(sizeof(int) * 4);
    (*p)[0] = a;
    (*p)[1] = b;
    (*p)[2] = c;
    (*p)[3] = d;
}

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

void print(char *c) {
    printf("%s\n", c);
}