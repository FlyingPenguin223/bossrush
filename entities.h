#pragma once

#include "entity.h"

void player_update(Entity* this);
void grapple_update(Entity* this);
void bullet_update(Entity* this);
void turret_update(Entity* this);

void (*obj_updates[4]) (Entity*) = {
	player_update,
	grapple_update,
	bullet_update,
	turret_update,
};
