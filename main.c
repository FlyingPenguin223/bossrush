#include <stdio.h>
#include <raylib.h>
#include <math.h>

#include "bossrush.h"

#include "tiled2c.h"
#include "entity.h"

#define WINDOW_WIDTH 1536
#define WINDOW_HEIGHT 1024

extern const Tiled2cMap map;

Texture2D tileset;
void load_textures();
int tile_at(int x, int y);
void draw_tile(Cam cam, int id, int x, int y);
void draw_entity(Cam cam, Entity* thing);

void draw_hitboxes(Cam cam, Entity_array* objects);

Vector2 get_mouse_pos_scaled();

Cam camera;
Entity_array* objects;

int main() {
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "boss rush!");

	SetTargetFPS(60);

	load_textures();

	objects = init_entity_array();

	const Tiled2cObjectLayer* objlayer = &map.layers[1].objectlayer;
	for (int i = 0; i < objlayer->numobjects; i++) {
		const Tiled2cObject* obj = &objlayer->objects[i];
		init_entity(objects, obj->tile - 1, obj->x / 8, (obj->y - 8) / 8);
	}

	camera.x = 0;
	camera.y = 0;
	camera.zoom = 64;

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

		draw_hitboxes(camera, objects);

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

	float angle = thing->rotation;
	float hyp = sqrtf(cam.zoom * cam.zoom + cam.zoom * cam.zoom) / -2;

	Vector2 offset = {(+sin(thing->rotation + (M_PI / 4)) - sin(M_PI / 4)) * hyp, (cos(thing->rotation + (M_PI / 4)) - cos(M_PI / 4)) * hyp};
	DrawTexturePro(tileset, src, dst, offset, thing->rotation * (180 / M_PI), WHITE);
}

void draw_hitboxes(Cam cam, Entity_array* objects) {
	for (int i = 0; i < objects->length; i++) {
		Entity* obj = get_entity(objects, i);

		Rectangle hitbox_drawn = {
			(obj->pos.x + obj->hitbox.x) * cam.zoom, 
			(obj->pos.y + obj->hitbox.y) * cam.zoom, 
			obj->hitbox.width * cam.zoom, 
			obj->hitbox.height * cam.zoom
		};

		DrawRectangleLinesEx(hitbox_drawn, 5, P8_RED);
	}
}
