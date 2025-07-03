#ifndef ENTITIES_H
#define ENTITIES_H

#include "entity.h"
#include <stdio.h>

void player_update(Entity* this);
void grapple_update(Entity* this);
void bullet_update(Entity* this);
void turret_update(Entity* this);
void mage_update(Entity* this);
void explosion_update(Entity* this);

static void (*const obj_updates[])(Entity*) = {
	player_update,
	grapple_update,
	bullet_update,
	turret_update,
	mage_update,
	explosion_update,
};

void mage_draw(Entity *this);

static void (*const obj_draws[])(Entity*) = {
	NULL,
	NULL,
	NULL,
	NULL,
	mage_draw,
	NULL,
};

#define ENTITY_FLAG_SOLID               ( (short) (0b0000000000000001) )
#define ENTITY_FLAG_BULLET              ( (short) (0b0000000000000010) )
#define ENTITY_FLAG_BULLET_COLLIDABLE   ( (short) (0b0000000000000100) )

static const short entity_flags[] = {
	ENTITY_FLAG_BULLET_COLLIDABLE,
	0,
	ENTITY_FLAG_BULLET,
	ENTITY_FLAG_SOLID,
	0,
	0,
}; // from entities.h, can't include due to pragma once fail?

#endif
