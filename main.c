#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

struct player_data {
	Entity* grapple;
	int health;
	int invulnerable_frames;
};

int main() {
	srand(time(NULL));
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "boss rush!");

	SetTargetFPS(60);

	load_textures();

	objects = init_entity_array();

	const Tiled2cObjectLayer* objlayer = &map.layers[1].objectlayer;
	for (unsigned int i = 0; i < objlayer->numobjects; i++) {
		const Tiled2cObject* obj = &objlayer->objects[i];
		init_entity(objects, obj->tile - 1, obj->x / 8, (obj->y - 8) / 8, obj->rotation * M_PI / 180);
	}

	camera.x = 0;
	camera.y = 0;
	camera.zoom = 32;

	while (!WindowShouldClose()) {

		Entity* player = get_entity_by_type(objects, 0);
		struct player_data* p_data = (struct player_data*) player->data;

		if (player != NULL) {

			float num_tiles_across = WINDOW_WIDTH / camera.zoom;
			float num_tiles_down = WINDOW_HEIGHT / camera.zoom;

			float cam_ideal_x = player->pos.x - num_tiles_across / 2;
			float cam_ideal_y = player->pos.y - num_tiles_down / 2;

			float smoothness = 10;

			camera.x = ((camera.x * smoothness) + cam_ideal_x) / (smoothness + 1);
			camera.y = ((camera.y * smoothness) + cam_ideal_y) / (smoothness + 1);
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (unsigned int y = 0; y < map.height; y++) {
			for (unsigned int x = 0; x < map.width; x++) {
				draw_tile(camera, tile_at(x, y), x, y);
			}
		}

		for (int i = 0; i < objects->length; i++) {
			Entity* thing = get_entity(objects, i);
			thing->update(thing);
			draw_entity(camera, thing);
		}

		if (p_data) {
			char hud_string[11];
			snprintf(hud_string, 11, "health: %d", p_data->health);
			DrawText(hud_string, 0, 0, 20, WHITE);
		}

		// draw_hitboxes(camera, objects);

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
	int tile_size = 8;
	int spritesheet_image_size = 256; // 256x256 at the moment
	int tiles_per_row = spritesheet_image_size / tile_size;

	id--;

	if (id < 0)
		return;

	Rectangle src = {
		.x = (id % tiles_per_row) * tile_size,
		.y = (int) (id / tiles_per_row) * tile_size,
		.width = tile_size,
		.height = tile_size,
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
	if (thing->draw) {
		thing->draw(thing);
	} else {
		int tile_size = 8;
		int spritesheet_image_size = 256; // 256x256 at the moment
		int tiles_per_row = spritesheet_image_size / tile_size;

		Rectangle src = {
			.x = (thing->type % tiles_per_row) * tile_size,
			.y = (int) (thing->type / tiles_per_row) * tile_size,
			.width = tile_size,
			.height = tile_size,
		};

		Rectangle dst = {
			.x = (thing->pos.x - cam.x) * cam.zoom,
			.y = (thing->pos.y - cam.y) * cam.zoom,
			.width = cam.zoom,
			.height = cam.zoom,
		};

		float angle = thing->rotation;
		float hyp = sqrtf(cam.zoom * cam.zoom + cam.zoom * cam.zoom) / -2;

		Vector2 offset = {(+sinf(angle + (M_PI / 4)) - sinf(M_PI / 4)) * hyp, (cosf(angle + (M_PI / 4)) - cosf(M_PI / 4)) * hyp};
		DrawTexturePro(tileset, src, dst, offset, angle * (180 / M_PI), WHITE);
		// DrawTexturePro(tileset, src, dst, (Vector2) {0, 0}, angle * (180 / M_PI), WHITE);
	}
}

void draw_hitboxes(Cam cam, Entity_array* objects) {
	for (int i = 0; i < objects->length; i++) {
		Entity* obj = get_entity(objects, i);

		Rectangle hitbox_drawn = {
			(obj->pos.x + obj->hitbox.x - camera.x) * cam.zoom, 
			(obj->pos.y + obj->hitbox.y - camera.y) * cam.zoom, 
			obj->hitbox.width * cam.zoom, 
			obj->hitbox.height * cam.zoom
		};

		DrawRectangleLinesEx(hitbox_drawn, 5, P8_RED);
	}
}
