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

void draw_texture_rect(Texture2D texture, Rectangle src, Rectangle dst, float rotation) {
	Rectangle dest = {
		.x = (dst.x - camera.x) * camera.zoom,
		.y = (dst.y - camera.y) * camera.zoom,
		.width = (dst.width) * camera.zoom,
		.height = (dst.height) * camera.zoom, // 8 is tile width
	};

	float angle = rotation;
	float hyp = sqrtf(camera.zoom * camera.zoom + camera.zoom * camera.zoom) / -2;
	float hyp_w = hyp * (src.width / 8); // test, kinda broken
	float hyp_h = hyp * (src.height / 8);

	Vector2 offset = {(+sinf(angle + (M_PI / 4)) - sinf(M_PI / 4)) * hyp_w, (cosf(angle + (M_PI / 4)) - cosf(M_PI / 4)) * hyp_h};
	DrawTexturePro(texture, src, dest, offset, angle * (180 / M_PI), WHITE);
}
