#include <stdio.h>

int main(void) {
    int exit_code;
    scanf("%d", &exit_code);
    puts("_main:\n");
    printf("  movl %d, %%eax\n", exit_code);
    puts("  ret\n");
    return exit_code;
}
