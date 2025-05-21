#include "tiled2c.h"
#include <raylib.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

extern const Tiled2cMap map;

Texture2D tileset;
void load_textures();
int tile_at(int x, int y);
void draw_tile(int id, int x, int y, float zoom);

int main() {
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "boss rush!");

	SetTargetFPS(60);

	load_textures();

	int zoom = 1;

	while (!WindowShouldClose()) {
		zoom++;

		BeginDrawing();

		for (int y = 0; y < map.height; y++) {
			for (int x = 0; x < map.width; x++) {
				draw_tile(tile_at(x, y), x, y, zoom);
			}
		}

		EndDrawing();
	}

	UnloadTexture(tileset);
	CloseWindow();
}

void load_textures() {
	tileset = LoadTexture("./maps/tileset.png");
}

int tile_at(int x, int y) {
	return map.layers[0].tilelayer.data[y * map.height + x];
}

void draw_tile(int id, int x, int y, float zoom) {
	int tileset_w = 8;
	int tileset_h = 8;

	id--;

	if (id < 0)
		return;

	Rectangle src = {
		.x = (id % 8) * 8,
		.y = (id / 8) * 8,
		.width = 8,
		.height = 8,
	};

	Rectangle dst = {
		.x = x * zoom,
		.y = y * zoom,
		.width = zoom,
		.height = zoom,
	};
	DrawTexturePro(tileset, src, dst, (Vector2) {0, 0}, 0, WHITE);
}
