#include "entity.h"
#include <stdint.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include <raymath.h>

#include <stdio.h>

#include "bossrush.h"
#include "tiled2c.h"
#include "mouse.h"
#include "entities.h"

int is_entity_touching_wall(Entity* thing);
int is_entity_touching_solid(Entity_array* objects, Entity* thing);
Entity* entity_colliding(Entity_array* objects, Entity* thing);
int entity_has_flag(Entity* thing, short flag);
Entity* entity_colliding_with_flag(Entity_array* objects, Entity* thing, short flag);

extern Cam camera;
extern Entity_array* objects;

extern Texture2D tileset;

struct player_data {
	Entity* grapple;
};

struct grapple_data {
	Entity* player;
	int attached;
	Entity* attached_to;
};

const float grapple_accel = 0.01;

void player_update(Entity* this) {
	if (this->data == NULL) {
		this->data = malloc(sizeof(struct player_data));
		this->hitbox = (Rectangle) {0.125, 0.125, 0.75, 0.75};
	}

	struct player_data* data = (struct player_data*) (this->data);

	float epsilon = 0.01;

	this->pos.x += this->spd.x;
	if (is_entity_touching_solid(objects, this)) {
		if (this->spd.x > 0) {
			this->pos.x = (int) (this->pos.x + this->hitbox.x + this->hitbox.width) - 1 + this->hitbox.x - epsilon;
		} else {
			this->pos.x = (int) (this->pos.x + this->hitbox.x) + 1 - this->hitbox.x + epsilon;
		}
		this->spd.x *= -0.8;
	}

	this->pos.y += this->spd.y;
	if (is_entity_touching_solid(objects, this)) {
		if (this->spd.y > 0) {
			this->pos.y = (int) (this->pos.y + this->hitbox.y + this->hitbox.height) - 1 + this->hitbox.y - epsilon;
		} else {
			this->pos.y = (int) (this->pos.y + this->hitbox.y) + 1 - this->hitbox.y + epsilon;
		}
		this->spd.y *= -0.8;
	}

	Vector2 mouse_pos = get_mouse_position();

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

		struct grapple_data* grapple_data = (struct grapple_data*) data->grapple->data;
		if (grapple_data != NULL) {
			if (grapple_data->attached) {
				grapple_data->player = this;
				this->spd = Vector2Add(this->spd, Vector2Scale(grapple_delta_normalized, grapple_accel)); // only if grapple has stopped
			}
		}

		// DrawLineEx((Vector2) {(this->pos.x + 0.5) * camera.zoom, (this->pos.y + 0.5) * camera.zoom}, (Vector2) {(data->grapple->pos.x + 0.5) * camera.zoom, (data->grapple->pos.y + 0.5) * camera.zoom}, 0.2 * camera.zoom, P8_DARK_PURPLE);
		draw_line(Vector2AddValue(this->pos, 0.5), Vector2AddValue(data->grapple->pos, 0.5), 0.5, P8_BLUE);
	} else {
		if (data->grapple != NULL) {
			kill_entity(objects, data->grapple);
			data->grapple = NULL;
		}
	}
}

struct bullet_data {
	int timer;
	bool initial_intangibility;
	int grappled;
	int bounce;
	bool has_been_grappled;
};

void grapple_update(Entity* this) {
	if (this->data == NULL) {
		this->data = malloc(sizeof(struct grapple_data));
		this->hitbox = (Rectangle) {0.375, 0.375, 0.25, 0.25};
		struct grapple_data* data = (struct grapple_data*) this->data;
		data->attached = 0;
		data->attached_to = NULL;
	}

	struct grapple_data* data = (struct grapple_data*) this->data;

	Vector2 sub_spd = Vector2Scale(this->spd, 0.1);

	float epsilon = 0;

	if (!data->attached) {
		for (int i = 0; i < 10; i++) {
			this->pos.x += sub_spd.x;
			if (is_entity_touching_solid(objects, this) && fabsf(sub_spd.x) > 0) {
				if (sub_spd.x > 0) {
					this->pos.x = (int) (this->pos.x + this->hitbox.x + this->hitbox.width) - 1 + this->hitbox.x - epsilon;
					this->rotation = M_PI / 2;
				} else {
					this->pos.x = (int) (this->pos.x + this->hitbox.x) + 1 - this->hitbox.x + epsilon;
					this->rotation = -M_PI / 2;
				}
				this->spd = (Vector2) {0, 0};
				data->attached = 1;
				break;
			}

			this->pos.y += sub_spd.y;
			if (is_entity_touching_solid(objects, this) && fabsf(sub_spd.y) > 0) {
				if (sub_spd.y > 0) {
					this->pos.y = (int) (this->pos.y + this->hitbox.y + this->hitbox.height) - 1 + this->hitbox.y - epsilon;
					this->rotation = M_PI;
				} else {
					this->pos.y = (int) (this->pos.y + this->hitbox.y) + 1 - this->hitbox.y + epsilon;
					this->rotation = 0;
				}
				this->spd = (Vector2) {0, 0};
				data->attached = 1;
				break;
			}

			Entity* thing = entity_colliding(objects, this);
			if (thing) {
				if (entity_has_flag(thing, ENTITY_FLAG_SOLID | ENTITY_FLAG_BULLET)) {
					data->attached = 1;
					data->attached_to = thing;
					break;
				}
			}
		}
	} else if (data->attached_to) {
		if (is_entity_valid(objects, data->attached_to)) {
			if (entity_has_flag(data->attached_to, ENTITY_FLAG_BULLET)) {
				struct bullet_data* b_data = (struct bullet_data*) data->attached_to->data;
				if (b_data) {
					b_data->grappled = 1;
					b_data->timer = 10;
					b_data->has_been_grappled = true;
				}
				Vector2 player_delta = Vector2Subtract(data->player->pos, this->pos);
				Vector2 player_delta_normalized = Vector2Normalize(player_delta);

				data->attached_to->spd = Vector2Add(data->attached_to->spd, Vector2Scale(player_delta_normalized, grapple_accel));
				this->pos = data->attached_to->pos;
			}
		} else { // bullet destroyed
			data->attached = 0;
			data->attached_to = NULL;
		}
	}
}

void bullet_update(Entity* this) {
	if (this->data == NULL) {
		this->data = malloc(sizeof(struct bullet_data));
		struct bullet_data* data = (struct bullet_data*) this->data;
		data->initial_intangibility = true;
		data->timer = 0;
		data->grappled = 0;
		data->has_been_grappled = false;
		return;
	}

	struct bullet_data* data = this->data;

	this->hitbox = data->bounce > 0 ? (Rectangle) {0, 0, 1, 1} : (Rectangle) {0.25, 0.25, 0.5, 0.5};

	if (data->initial_intangibility && !is_entity_touching_solid(objects, this) && !entity_colliding_with_flag(objects, this, ENTITY_FLAG_BULLET_COLLIDABLE))
		data->initial_intangibility = false;

	data->timer--;

	bool kms = false;
	this->pos.x += this->spd.x;
	if (!data->initial_intangibility && is_entity_touching_solid(objects, this)) {
		if (data->bounce > 0) {
			data->bounce -= 1;
			this->pos.x -= this->spd.x;
			this->spd.x *= -1;
		} else
			kms = true;
	}
	this->pos.y += this->spd.y;
	if (!data->initial_intangibility && is_entity_touching_solid(objects, this)) {
		if (data->bounce > 0) {
			data->bounce -= 1;
			this->pos.y -= this->spd.y;
			this->spd.y *= -1;
		} else
			kms = true;
	}
	if (!data->initial_intangibility && ((entity_colliding_with_flag(objects, this, ENTITY_FLAG_BULLET_COLLIDABLE | ENTITY_FLAG_SOLID) && !data->grappled && data->timer <= 0)))
		kms = true;

	if (!data->initial_intangibility && data->has_been_grappled && entity_colliding_with_flag(objects, this, ENTITY_FLAG_ENEMY))
		kms = true;

	if (kms) {
			init_entity(objects, 5, this->pos.x, this->pos.y, 0);
			return kill_entity(objects, this);
	}

	data->grappled = 0;
}

void bullet_draw(Entity *this) {
	struct bullet_data* data = this->data;
	int tile_size = 8;
	int spritesheet_image_size = 256; // 256x256 at the moment
	int tiles_per_row = spritesheet_image_size / tile_size;

	Rectangle src = {
		.x = (this->type % tiles_per_row) * tile_size,
		.y = (int) (this->type / tiles_per_row) * tile_size,
		.width = tile_size,
		.height = tile_size,
	};

	Rectangle dst = {
		.x = (this->pos.x - camera.x - (data->bounce ? 0.5 : 0)) * camera.zoom,
		.y = (this->pos.y - camera.y - (data->bounce ? 0.5 : 0)) * camera.zoom,
		.width = (data->bounce ? 2 : 1) * camera.zoom,
		.height = (data->bounce ? 2 : 1) * camera.zoom,
	};

	float angle = this->rotation;
	float hyp = sqrtf(camera.zoom * camera.zoom + camera.zoom * camera.zoom) / -2;

	Vector2 offset = {(+sinf(angle + (M_PI / 4)) - sinf(M_PI / 4)) * hyp, (cosf(angle + (M_PI / 4)) - cosf(M_PI / 4)) * hyp};
	DrawTexturePro(tileset, src, dst, offset, angle * (180 / M_PI), WHITE);
	// DrawTexturePro(tileset, src, dst, (Vector2) {0, 0}, angle * (180 / M_PI), WHITE);
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
		Entity* bullet = init_entity(objects, 2, this->pos.x, this->pos.y, this->rotation);
		bullet_update(bullet);
		bullet->spd = Vector2Scale((Vector2) {cos(this->rotation - M_PI / 2), sin(this->rotation - M_PI / 2)}, 0.1);
	}
}

enum mage_states: uint8_t {
	SHOOTING,
	WAITING,
	LIMBO,
};

// 60f limbo -> 120f waiting -> 20f shooting -> loop

struct mage_data {
	Rectangle valid_locations;
	enum mage_states state;
	int timer;
};

void mage_update(Entity* this) {
	if (this->data == NULL) {
		this->data = malloc(sizeof(struct mage_data));
		this->hitbox = (Rectangle) {0, 0, 3, 3};

		struct mage_data* data = (struct mage_data*) this->data;
		data->state = LIMBO;
		data->timer = 60;

		data->valid_locations = (Rectangle) {this->pos.x - 12, this->pos.y - 9, 24, 16};
	}
	struct mage_data* data = (struct mage_data*) this->data;

	Vector2 mage_pos_gun = {
		this->pos.x + (17.0 / 8.0),
		this->pos.y + (7.0 / 8.0)
	};

	switch (data->state) {
		case LIMBO:
			data->timer--;
			if (data->timer <= 0) {
				this->pos.x = (rand() % (int)data->valid_locations.width) + data->valid_locations.x;
				this->pos.y = (rand() % (int)data->valid_locations.height) + data->valid_locations.y;
				data->timer = 120;
				data->state = WAITING;
			}
			break;
		case WAITING:
			data->timer--;
			if (data->timer <= 0) {
				data->timer = 30;
				data->state = SHOOTING;
			}
			break;
			
		case SHOOTING:
			data->timer--;
			if (data->timer % 5 == 0) {
				Entity* bullet = init_entity(objects, 2, mage_pos_gun.x, mage_pos_gun.y, 0);
				bullet_update(bullet);
				auto bdata = (struct bullet_data*)bullet->data;
				bdata->bounce = 1;
				Entity* player = get_entity_by_type(objects, 0);
				if (player) {
					float angle_to_player = atan2((player->pos.y + 0.5) - mage_pos_gun.y, (player->pos.x + 0.5) - mage_pos_gun.x);
					bullet->spd = Vector2Scale((Vector2) {cos(angle_to_player), sin(angle_to_player)}, 0.25);
				}
			}
			if (data->timer <= 0) {
				data->timer = 60;
				data->state = LIMBO;
			}
			break;
		default:
			printf("what???\n");
			break;
	}

	// this->rotation += 0.1;
}

void mage_draw(Entity* this) {
	Rectangle sprite_source = {
		0, 232, 24, 24
	};

	struct mage_data* data = (struct mage_data*) this->data;
	if (data->state != LIMBO)
		draw_texture_rect(tileset, sprite_source, this->pos, this->rotation);
}

struct explosion_data {
	int timer;
};

void explosion_update(Entity* this) {
	if (this->data == NULL) {
		this->data = malloc(sizeof(struct explosion_data));
		struct explosion_data* data = (struct explosion_data*) this->data;
		data->timer = 30;
	}

	struct explosion_data* data = (struct explosion_data*) this->data;
	data->timer--;

	if (data->timer <= 0) {
		kill_entity(objects, this);
	}
}

int is_tile_solid(int tile) {
	return tile > 4; // tmp, use fget system
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

int is_entity_touching_solid(Entity_array* objects, Entity* thing) {
	if (is_entity_touching_wall(thing))
		return 1;

	for (int i = 0; i < objects->length; i++) {
		Entity* obj = objects->array[i];
		if (thing != obj) {
			Rectangle h1 = thing->hitbox;
			Rectangle h2 = obj->hitbox;
			h1.x += thing->pos.x;
			h1.y += thing->pos.y;

			h2.x += obj->pos.x;
			h2.y += obj->pos.y;

			if (h1.x < h2.x + h2.width && h1.x + h1.width > h2.x && h1.y < h2.y + h2.height && h1.y + h1.height > h2.y) {
				if (entity_has_flag(obj, ENTITY_FLAG_SOLID))
					return 1;
			}
		}
	}
	return 0;
}

Entity* entity_colliding(Entity_array* objects, Entity* thing) {
	for (int i = 0; i < objects->length; i++) {
		Entity* obj = objects->array[i];
		if (thing != obj) {
			Rectangle h1 = thing->hitbox;
			Rectangle h2 = obj->hitbox;
			h1.x += thing->pos.x;
			h1.y += thing->pos.y;

			h2.x += obj->pos.x;
			h2.y += obj->pos.y;

			if (h1.x < h2.x + h2.width && h1.x + h1.width > h2.x && h1.y < h2.y + h2.height && h1.y + h1.height > h2.y) {
				return obj;
			}
		}
	}
	return NULL;
}

int entity_has_flag(Entity* thing, short flag) {
	if (thing)
		return entity_flags[thing->type] & flag;
	return 0;
}

Entity* entity_colliding_with_flag(Entity_array* objects, Entity* thing, short flag) {
	for (int i = 0; i < objects->length; i++) {
		Entity* obj = objects->array[i];
		if (thing != obj) {
			Rectangle h1 = thing->hitbox;
			Rectangle h2 = obj->hitbox;
			h1.x += thing->pos.x;
			h1.y += thing->pos.y;

			h2.x += obj->pos.x;
			h2.y += obj->pos.y;

			if (h1.x < h2.x + h2.width && h1.x + h1.width > h2.x && h1.y < h2.y + h2.height && h1.y + h1.height > h2.y) {
				if (entity_has_flag(obj, flag))
					return obj;
			}
		}
	}
	return NULL;
}
