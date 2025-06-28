#include <stdlib.h>

#include "entity.h"
#include "entities.h"

extern void player_update(Entity* this);

Entity_array* init_entity_array() {
    Entity_array* ret = malloc(sizeof(Entity_array));
    ret->length = 0;
    ret->capacity = 1;
    ret->array = malloc(ret->capacity * sizeof(Entity*));

    return ret;
}

Entity* init_entity(Entity_array* array, int type, float x, float y, float rotation) {
    Entity* new_entity = malloc(sizeof(Entity));

    new_entity->pos = (Vector2) {x, y};
    new_entity->spd = (Vector2) {0, 0};
    new_entity->rotation = rotation;
    new_entity->type = type;
    new_entity->hitbox = (Rectangle) {.x = 0, .y = 0, .width = 1, .height = 1};
    new_entity->data = NULL;
    new_entity->update = obj_updates[type];

    if (array->length >= array->capacity) {
        array->capacity *= 2;
        array->array = realloc(array->array, array->capacity * sizeof(Entity*));
    }

    array->array[array->length] = new_entity;

    array->length++;

    return new_entity;
}

void kill_entity_id(Entity_array* array, int id) {
    if (id < 0 || id >= array->length)
        return;

    if (array->array[id]->data != NULL)
        free(array->array[id]->data);
    free(array->array[id]);

    for (int i = id+1; i < array->length; i++) {
        array->array[i - 1] = array->array[i];
    }

    array->length--;
}

void kill_entity(Entity_array* array, Entity* thing) {
    for (int i = 0; i < array->length; i++) {
        if (thing == array->array[i]) {
            kill_entity_id(array, i);
            break;
        }
    }
}

Entity* get_entity(Entity_array* array, int id) {
    if (id < 0 || id >= array->length)
        return NULL;

    return array->array[id];
}

void free_entity_array(Entity_array* array) {
    for (int i = array->length-1; i >= 0; i--) {
        kill_entity_id(array, i);
    }
    free(array->array);
    free(array);
}

int is_entity_valid(Entity_array* array, Entity* thing) {
	for (int i = 0; i < array->length; i++) {
		if (thing == array->array[i])
			return 1;
	}
	return 0;
}
