#include "entity.h"
#include <raylib.h>
#include <raymath.h>

#include <stdio.h>

#include "bossrush.h"
#include "tiled2c.h"

int is_entity_touching_wall(Entity* thing);

extern Cam camera;

void player_update(Entity* this) {
    static int init = 1;
    if (init) {
        init = 0;
        this->hitbox = (Rectangle) {0.125, 0.125, 0.75, 0.75};
    }

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
        this->spd = Vector2Add(this->spd, Vector2Scale(mouse_delta_normalized, 0.01));

        DrawLineEx((Vector2) {(this->pos.x + 0.5) * camera.zoom, (this->pos.y + 0.5) * camera.zoom}, mouse_pos_raw, 10, P8_DARK_PURPLE);
        DrawCircleV(mouse_pos_raw, 10, P8_DARK_PURPLE);
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
