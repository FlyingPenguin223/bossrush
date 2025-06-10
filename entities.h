#pragma once

#include "entity.h"

void player_update(Entity* this);
void grapple_update(Entity* this);

void (*obj_updates[2]) (Entity*) = {
	player_update,
	grapple_update,
};
