#include "entity.h"
#include <raylib.h>
#include <raymath.h>

#include "bossrush.h"

extern Cam camera;

void player_update(Entity* this) {
    static int init = 1;
    if (init) {
        init = 0;
        this->hitbox = (Rectangle) {0.125, 0.125, 0.75, 0.75};
    }

    this->pos = Vector2Add(this->pos, this->spd);
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
