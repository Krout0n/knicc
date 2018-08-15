#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./knicc.h"

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

Map *init_map(void) {
	Map *map = malloc(sizeof(Map));
	map->vec = init_vector();
	return map;
}

void insert_map(Map *map, KeyValue *kv) {
	vec_push(map->vec, kv);
}

KeyValue *find_by_key(Map *map, char *key) {
	int i = 0;
	KeyValue *kv;
	Vector *vec = map->vec;
	while(i < vec_size(vec)) {
		kv = vec_get(vec, i);
		if (strcmp(kv->key, key) == 0) return kv;
		i += 1;
	}
	return NULL;
}
