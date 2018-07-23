#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *key;
	int value;
} KeyValue;

KeyValue *new_kv(char *key, int value) {
	KeyValue *kv = (KeyValue *)malloc(sizeof(KeyValue));
	if (kv == NULL) return NULL;
	kv->value = value;
	kv->key = (char *)malloc(sizeof(char) * strlen(key));
	strcpy(kv->key, key);
	return kv;
}

void debug_kv(KeyValue *kv) {
	printf("%s: %d\n", kv->key, kv->value);
}

typedef struct {
	size_t length;
	size_t buf;
	KeyValue *data;
} Vector;

Vector *init_vector() {
	Vector *vec = (Vector *)malloc(sizeof(Vector));
	if (vec == NULL) return NULL;
	vec->length = 0;
	vec->buf = 1;
	vec->data = (KeyValue *)malloc(sizeof(KeyValue) * vec->buf);
	return vec;
}

size_t vec_size(Vector *vec) {
	return vec->length;
}

void vec_push(Vector *vec, KeyValue *item) {
	if (vec->length == vec->buf) {
		vec->buf *= 2;
		vec->data = (KeyValue *)realloc(vec->data, sizeof(KeyValue) * vec->buf);
	}
	vec->data[vec->length] = *item;
	vec->length += 1;
}

KeyValue *vec_get(Vector *vec, int index) {
	if (vec->length < index) {
		return NULL;
	}
	return &vec->data[index];
}

KeyValue *find_by_key(Vector *vec, char *key) {
	int i = 0;
	KeyValue *kv;
	while(i < vec_size(vec)) {
		kv = vec_get(vec, i);
		if (strcmp(kv->key, key) == 0) return kv;
		i += 1;
	}
	return NULL;
}

int main() {
	Vector *vec = init_vector();
	int i;
	vec_push(vec, new_kv("hoge", 0));
	vec_push(vec, new_kv("fuga", 1));
	vec_push(vec, new_kv("bar", 2));
	vec_push(vec, new_kv("bar", 2));
	vec_push(vec, new_kv("bar", 2));
	vec_push(vec, new_kv("baz", 15));
	debug_kv(vec_get(vec, 0));
	debug_kv(vec_get(vec, 1));
	debug_kv(vec_get(vec, 2));
	debug_kv(vec_get(vec, 4));
	printf("vec buffer: %zu\n", vec->buf); // 8
	debug_kv(find_by_key(vec, "baz")); // baz 15
	return 0;
}
