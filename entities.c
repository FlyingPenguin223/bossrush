#include "entity.h"
#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include <raymath.h>

#include <stdio.h>

#include "bossrush.h"
#include "tiled2c.h"

int is_entity_touching_wall(Entity* thing);

extern Cam camera;
extern Entity_array* objects;

struct player_data {
    Entity* grapple;
};

void player_update(Entity* this) {
    if (this->data == NULL) {
        this->data = malloc(sizeof(struct player_data));
        this->hitbox = (Rectangle) {0.125, 0.125, 0.75, 0.75};
    }

    struct player_data* data = (struct player_data*) (this->data);

    float epsilon = 0.01;

    this->pos.x += this->spd.x;
    if (is_entity_touching_wall(this)) {
        if (this->spd.x > 0) {
            this->pos.x = (int) (this->pos.x + this->hitbox.x + this->hitbox.width) - 1 + this->hitbox.x - epsilon;
        } else {
            this->pos.x = (int) (this->pos.x + this->hitbox.x) + 1 - this->hitbox.x + epsilon;
        }
        this->spd.x *= -0.8;
    }

    this->pos.y += this->spd.y;
    if (is_entity_touching_wall(this)) {
        if (this->spd.y > 0) {
            this->pos.y = (int) (this->pos.y + this->hitbox.y + this->hitbox.height) - 1 + this->hitbox.y - epsilon;
        } else {
            this->pos.y = (int) (this->pos.y + this->hitbox.y) + 1 - this->hitbox.y + epsilon;
        }
        this->spd.y *= -0.8;
    }

    Vector2 mouse_pos_raw = GetMousePosition();

    Vector2 mouse_pos = {mouse_pos_raw.x / camera.zoom, mouse_pos_raw.y / camera.zoom};

    Vector2 mouse_delta = Vector2Subtract(mouse_pos, (Vector2) {this->pos.x + 0.5, this->pos.y + 0.5});
    Vector2 mouse_delta_normalized = Vector2Normalize(mouse_delta);

    float mouse_angle = atan2(mouse_delta.y, mouse_delta.x);

    this->rotation = mouse_angle + M_PI / 2;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (data->grapple == NULL) {
            data->grapple = init_entity(objects, 1, this->pos.x, this->pos.y, this->rotation);
            data->grapple->spd = Vector2Scale(mouse_delta_normalized, 1.5);
        }

        Vector2 grapple_delta = Vector2Subtract(data->grapple->pos, this->pos);
        Vector2 grapple_delta_normalized = Vector2Normalize(grapple_delta);

        if (Vector2Length(data->grapple->spd) == 0)
            this->spd = Vector2Add(this->spd, Vector2Scale(grapple_delta_normalized, 0.01)); // only if grapple has stopped

        DrawLineEx((Vector2) {(this->pos.x + 0.5) * camera.zoom, (this->pos.y + 0.5) * camera.zoom}, (Vector2) {(data->grapple->pos.x + 0.5) * camera.zoom, (data->grapple->pos.y + 0.5) * camera.zoom}, 0.2 * camera.zoom, P8_DARK_PURPLE);
    } else {
        if (data->grapple != NULL) {
            kill_entity(objects, data->grapple);
            data->grapple = NULL;
        }
    }
}

void grapple_update(Entity* this) {
    this->hitbox = (Rectangle) {0.375, 0.375, 0.25, 0.25};

    Vector2 sub_spd = Vector2Scale(this->spd, 0.1);

    float epsilon = 0;

    for (int i = 0; i < 10; i++) {
        this->pos.x += sub_spd.x;
        if (is_entity_touching_wall(this) && fabsf(sub_spd.x) > 0) {
            if (sub_spd.x > 0) {
                this->pos.x = (int) (this->pos.x + this->hitbox.x + this->hitbox.width) - 1 + this->hitbox.x - epsilon;
                this->rotation = M_PI / 2;
            } else {
                this->pos.x = (int) (this->pos.x + this->hitbox.x) + 1 - this->hitbox.x + epsilon;
                this->rotation = -M_PI / 2;
            }
            this->spd = (Vector2) {0, 0};
            break;
        }

        this->pos.y += sub_spd.y;
        if (is_entity_touching_wall(this) && fabsf(sub_spd.y) > 0) {
            if (sub_spd.y > 0) {
                this->pos.y = (int) (this->pos.y + this->hitbox.y + this->hitbox.height) - 1 + this->hitbox.y - epsilon;
                this->rotation = M_PI;
            } else {
                this->pos.y = (int) (this->pos.y + this->hitbox.y) + 1 - this->hitbox.y + epsilon;
                this->rotation = 0;
            }
            this->spd = (Vector2) {0, 0};
            break;
        }
    }
}

void bullet_update(Entity* this) {
    this->hitbox = (Rectangle) {0.25, 0.25, 0.5, 0.5};
    Vector2 spd = (Vector2) {cos(this->rotation - M_PI / 2), sin(this->rotation - M_PI / 2)};
    this->spd = Vector2Scale(spd, 0.1);
    this->pos = Vector2Add(this->pos, this->spd);
    if (is_entity_touching_wall(this))
        kill_entity(objects, this);
}

struct turret_data {
    int timer;
};

void turret_update(Entity* this) {
    if (this->data == NULL) {
        this->data = malloc(sizeof(struct turret_data));
        struct turret_data* data = (struct turret_data*) this->data;
        data->timer = 0;
    }

    struct turret_data* data = (struct turret_data*) this->data;

    data->timer++;
    if (data->timer >= 30) {
        data->timer = 0;
        init_entity(objects, 2, this->pos.x, this->pos.y, this->rotation);
    }
}

int is_tile_solid(int tile) {
    return tile > 0; // tmp, use fget system
}

extern Tiled2cMap map;

int is_entity_touching_wall(Entity* thing) {
    int left_x = (int) (thing->pos.x + thing->hitbox.x);
    int top_y = (int) (thing->pos.y + thing->hitbox.y);

    int right_x = (int) (thing->pos.x + thing->hitbox.x + thing->hitbox.width) + 1; // add one since for is exclusive
    int bottom_y = (int) (thing->pos.y + thing->hitbox.y + thing->hitbox.height) + 1;

    for (int y = top_y; y < bottom_y; y++) {
        for (int x = left_x; x < right_x; x++) {
            int index = y * map.width + x;
            int tile = map.layers[0].tilelayer.data[index] - 1; // add tile_at function
            
            if (is_tile_solid(tile)) {
                return 1;
            }
        }
    }
    return 0;
}
