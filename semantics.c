#include <stdlib.h>
#include "knicc.h"

Var *new_var(TrueType type, int pos) {
    Var *v = malloc(sizeof(Var));
    v->type = type;
    v->position = pos;
    return v;
}