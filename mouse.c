#include <raylib.h>
#include "bossrush.h"
#include <math.h>

extern Cam camera;

Vector2 get_mouse_position() {
	Vector2 mouse_pos_raw = GetMousePosition();
	Vector2 mouse_pos = {mouse_pos_raw.x / camera.zoom + camera.x, mouse_pos_raw.y / camera.zoom + camera.y};
	
	return mouse_pos;
}

void draw_line(Vector2 start, Vector2 end, float thickness, Color color) {
	Vector2 real_start = (Vector2) {start.x * camera.zoom - camera.x * camera.zoom, start.y * camera.zoom - camera.y * camera.zoom};
	Vector2 real_end = (Vector2) {end.x * camera.zoom - camera.x * camera.zoom, end.y * camera.zoom - camera.y * camera.zoom};

	DrawLineEx(real_start, real_end, (thickness / 2) * camera.zoom, color);
}

void draw_texture_rect(Texture2D texture, Rectangle src, Vector2 pos, float rotation) {
	Rectangle dst = {
		.x = (pos.x - camera.x) * camera.zoom,
		.y = (pos.y - camera.y) * camera.zoom,
		.width = (src.width / 8) * camera.zoom,
		.height = (src.height / 8) * camera.zoom, // 8 is tile width
	};

	float angle = rotation;
	float hyp = sqrtf(camera.zoom * camera.zoom + camera.zoom * camera.zoom) / -2;
	float hyp_w = hyp * (src.width / 8);
	float hyp_h = hyp * (src.height / 8);

	Vector2 offset = {(+sinf(angle + (M_PI / 4)) - sinf(M_PI / 4)) * hyp_w, (cosf(angle + (M_PI / 4)) - cosf(M_PI / 4)) * hyp_h};
	DrawTexturePro(texture, src, dst, offset, angle * (180 / M_PI), WHITE);
}
