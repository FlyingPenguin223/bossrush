#ifndef ENTITIES_H
#define ENTITIES_H

#include "entity.h"

void player_update(Entity* this);
void grapple_update(Entity* this);
void bullet_update(Entity* this);
void turret_update(Entity* this);
void arrow_update(Entity* this);
void explosion_update(Entity* this);

void (*obj_updates[6]) (Entity*) = {
	player_update,
	grapple_update,
	bullet_update,
	turret_update,
	arrow_update,
	explosion_update,
};

#endif
