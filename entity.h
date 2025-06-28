#ifndef ENTITY_H
#define ENTITY_H

#include <raylib.h>

typedef struct Entity {
    int type;
    Vector2 pos;
    Vector2 spd;
    float rotation;
    Rectangle hitbox;
    void* data;
    void (*update)(struct Entity* this);
} Entity;

typedef struct Entity_array {
    int length;
    int capacity;
    Entity** array;
} Entity_array;

Entity_array* init_entity_array();

Entity* init_entity(Entity_array* array, int type, float x, float y, float rotation);

void kill_entity_id(Entity_array* array, int id);
void kill_entity(Entity_array* array, Entity* thing);

Entity* get_entity(Entity_array* array, int id);
Entity* get_entity_by_type(Entity_array* array, int type);

void free_entity_array(Entity_array* array);

int is_entity_valid(Entity_array* array, Entity* thing);

#endif
