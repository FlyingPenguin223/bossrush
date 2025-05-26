#include "tiled2c.h"
#include <stdio.h>
#include <raylib.h>

#include "entity.h"

#define WINDOW_WIDTH 1536
#define WINDOW_HEIGHT 1024

extern const Tiled2cMap map;

typedef struct {
	float x;
	float y;
	float zoom;
} Cam;

Texture2D tileset;
void load_textures();
int tile_at(int x, int y);
void draw_tile(Cam cam, int id, int x, int y);
void draw_entity(Cam cam, Entity* thing);

int main() {
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "boss rush!");

	SetTargetFPS(60);

	load_textures();

	Entity_array* objects = init_entity_array();

	const Tiled2cObjectLayer* objlayer = &map.layers[1].objectlayer;
	for (int i = 0; i < objlayer->numobjects; i++) {
		const Tiled2cObject* obj = &objlayer->objects[i];
		init_entity(objects, obj->tile - 1, obj->x / 8, (obj->y - 8) / 8);
	}

	Cam camera = {
		.x = 0,
		.y = 0,
		.zoom = 64,
	};

	while (!WindowShouldClose()) {

		BeginDrawing();

		ClearBackground(BLACK);

		for (int y = 0; y < map.height; y++) {
			for (int x = 0; x < map.width; x++) {
				draw_tile(camera, tile_at(x, y), x, y);
			}
		}

		for (int i = 0; i < objects->length; i++) {
			Entity* thing = get_entity(objects, i);
			thing->update(thing);
			draw_entity(camera, thing);
		}

		EndDrawing();
	}

	free_entity_array(objects);
	UnloadTexture(tileset);
	CloseWindow();
}

void load_textures() {
	tileset = LoadTexture("./maps/tileset.png");
}

int tile_at(int x, int y) {
	return map.layers[0].tilelayer.data[y * map.height + x];
}

void draw_tile(Cam cam, int id, int x, int y) {
	int tileset_w = 8;
	int tileset_h = 8;

	id--;

	if (id < 0)
		return;

	Rectangle src = {
		.x = (id % 8) * 8,
		.y = (int) (id / 8) * 8,
		.width = 8,
		.height = 8,
	};

	Rectangle dst = {
		.x = (x - cam.x) * cam.zoom,
		.y = (y - cam.y) * cam.zoom,
		.width = cam.zoom,
		.height = cam.zoom,
	};
	DrawTexturePro(tileset, src, dst, (Vector2) {0, 0}, 0, WHITE);
}

void draw_entity(Cam cam, Entity* thing) {
	Rectangle src = {
		.x = (thing->type % 8) * 8,
		.y = (int) (thing->type / 8) * 8,
		.width = 8,
		.height = 8,
	};

	Rectangle dst = {
		.x = (thing->pos.x - cam.x) * cam.zoom,
		.y = (thing->pos.y - cam.y) * cam.zoom,
		.width = cam.zoom,
		.height = cam.zoom,
	};
	DrawTexturePro(tileset, src, dst, (Vector2) {0, 0}, thing->rotation, WHITE);
}
