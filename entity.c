#include <stdlib.h>

#include "entity.h"

extern void player_update(Entity* this);

Entity_array* init_entity_array() {
    Entity_array* ret = malloc(sizeof(Entity_array));
    ret->length = 0;
    ret->capacity = 1;
    ret->array = malloc(ret->capacity * sizeof(Entity*));

    return ret;
}

Entity* init_entity(Entity_array* array, int type, float x, float y) {
    Entity* new_entity = malloc(sizeof(Entity));

    new_entity->pos = (Vector2) {x, y};
    new_entity->spd = (Vector2) {0, 0};
    new_entity->rotation = 0;
    new_entity->type = type;
    new_entity->update = player_update;


    if (array->length >= array->capacity) {
        array->capacity *= 2;
        array->array = realloc(array->array, array->capacity * sizeof(Entity*));
    }

    array->array[array->length] = new_entity;

    array->length++;

    return new_entity;
}

void kill_entity(Entity_array* array, int id) {
    if (id < 0 || id >= array->length)
        return;

    free(array->array[id]);

    for (int i = id+1; i < array->length; i++) {
        array->array[i - 1] = array->array[i];
    }

    array->length--;
}

Entity* get_entity(Entity_array* array, int id) {
    if (id < 0 || id >= array->length)
        return NULL;

    return array->array[id];
}

void free_entity_array(Entity_array* array) {
    for (int i = array->length-1; i >= 0; i--) {
        kill_entity(array, i);
    }
    free(array->array);
    free(array);
}
