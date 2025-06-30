#include "entity.h"
#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include <raymath.h>

#include <stdio.h>

#include "bossrush.h"
#include "tiled2c.h"
#include "mouse.h"
#include "entities.h"

int is_entity_touching_wall(Entity_array* objects, Entity* thing);
Entity* entity_colliding(Entity_array* objects, Entity* thing);
int entity_has_flag(Entity* thing, short flag);
Entity* entity_colliding_with_flag(Entity_array* objects, Entity* thing, short flag);

extern Cam camera;
extern Entity_array* objects;

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
	if (is_entity_touching_wall(objects, this)) {
		if (this->spd.x > 0) {
			this->pos.x = (int) (this->pos.x + this->hitbox.x + this->hitbox.width) - 1 + this->hitbox.x - epsilon;
		} else {
			this->pos.x = (int) (this->pos.x + this->hitbox.x) + 1 - this->hitbox.x + epsilon;
		}
		this->spd.x *= -0.8;
	}

	this->pos.y += this->spd.y;
	if (is_entity_touching_wall(objects, this)) {
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
		draw_line(Vector2AddValue(this->pos, 0.5), Vector2AddValue(data->grapple->pos, 0.5), 0.2, P8_DARK_PURPLE);
	} else {
		if (data->grapple != NULL) {
			kill_entity(objects, data->grapple);
			data->grapple = NULL;
		}
	}
}

struct bullet_data {
	int timer;
	int grappled;
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
			if (is_entity_touching_wall(objects, this) && fabsf(sub_spd.x) > 0) {
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
			if (is_entity_touching_wall(objects, this) && fabsf(sub_spd.y) > 0) {
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
				if (b_data)
					b_data->grappled = 1;
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
		this->hitbox = (Rectangle) {0.25, 0.25, 0.5, 0.5};
		this->data = malloc(sizeof(struct bullet_data));
		struct bullet_data* data = (struct bullet_data*) this->data;
		data->timer = 10;
		data->grappled = 0;
	}

	struct bullet_data* data = (struct bullet_data*) this->data;

	data->timer--;

	this->pos = Vector2Add(this->pos, this->spd);
	if ((is_entity_touching_wall(objects, this) || ( entity_colliding_with_flag(objects, this, ENTITY_FLAG_BULLET_COLLIDABLE) && !data->grappled)) && data->timer <= 0) {
		init_entity(objects, 5, this->pos.x, this->pos.y, 0);
		return kill_entity(objects, this); // don't run subsequent assignment
	}

	data->grappled = 0;
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
		bullet->spd = Vector2Scale((Vector2) {cos(this->rotation - M_PI / 2), sin(this->rotation - M_PI / 2)}, 0.1);
	}
}

void arrow_update(Entity* this) {

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

int is_entity_touching_wall(Entity_array* objects, Entity* thing) {
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
	return entity_flags[thing->type] & flag;
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
