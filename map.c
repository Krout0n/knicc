#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./knicc.h"

KeyValue *new_kv(char *key, void *value) {
	KeyValue *kv = (KeyValue *)malloc(sizeof(KeyValue));
	if (kv == NULL) return NULL;
	kv->value = value;
	kv->key = (char *)malloc(sizeof(char) * strlen(key));
	strcpy(kv->key, key);
	return kv;
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

KeyValue *last_inserted(Map *map) {
	Vector *vec = map->vec;
	return (KeyValue *) vec_get(vec, vec_size(vec)-1);
}

void debug_map(Map *map) {
	printf("Map: {\n");
	for (int i = 0; i < map->vec->length; i++) {
		KeyValue *kv = ((KeyValue *)(vec_get(map->vec, i)));
		printf("  ");
		debug_var(kv->key, kv->value);
	}
	printf("}\n");
}