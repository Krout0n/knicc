#include <stdio.h>
#include <stdlib.h>

#include "./knicc.h"

Vector *init_vector() {
	Vector *vec = (Vector *)malloc(sizeof(Vector));
	if (vec == NULL) return NULL;
	vec->length = 0;
	vec->buf = 1;
	vec->data = malloc(sizeof(void*) * vec->buf);
	return vec;
}

size_t vec_size(Vector *vec) {
	return vec->length;
}

void vec_push(Vector *vec, void *item) {
	if (vec->length == vec->buf) {
		vec->buf *= 2;
		vec->data = realloc(vec->data, sizeof(void*) * vec->buf);
	}
	vec->data[vec->length] = item;
	vec->length += 1;
}

void *vec_get(Vector *vec, int index) {
	if (vec->length < index) {
		return NULL;
	}
	return vec->data[index];
}
