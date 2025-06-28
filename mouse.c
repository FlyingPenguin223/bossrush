#include <raylib.h>
#include "bossrush.h"

extern Cam camera;

Vector2 get_mouse_position() {
	Vector2 mouse_pos_raw = GetMousePosition();
	Vector2 mouse_pos = {mouse_pos_raw.x / camera.zoom + camera.x, mouse_pos_raw.y / camera.zoom + camera.y};
	
	return mouse_pos;
}

void draw_line(Vector2 start, Vector2 end, float thickness, Color color) {
	Vector2 real_start = (Vector2) {start.x * camera.zoom - camera.x * camera.zoom, start.y * camera.zoom - camera.y * camera.zoom};
	Vector2 real_end = (Vector2) {end.x * camera.zoom - camera.x * camera.zoom, end.y * camera.zoom - camera.y * camera.zoom};

	DrawLineEx(real_start, real_end, thickness * camera.zoom, color);
}
