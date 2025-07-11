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

#define ENTITY_PLAYER 0
#define ENTITY_GRAPPLE 1
#define ENTITY_BULLET 2
#define ENTITY_TURRET 3
#define ENTITY_MAGE 4
#define ENTITY_EXPLOSION 5

static void (*const obj_updates[])(Entity*) = {
	player_update,
	grapple_update,
	bullet_update,
	turret_update,
	mage_update,
	explosion_update,
};

void player_draw(Entity* this);
void bullet_draw(Entity* this);
void mage_draw(Entity *this);

static void (*const obj_draws[])(Entity*) = {
	player_draw,
	NULL,
	bullet_draw,
	NULL,
	mage_draw,
	NULL,
};

#define ENTITY_FLAG_NONE                0
#define ENTITY_FLAG_SOLID               ( (short) (0b0000000000000001) )
#define ENTITY_FLAG_BULLET              ( (short) (0b0000000000000010) )
#define ENTITY_FLAG_BULLET_COLLIDABLE   ( (short) (0b0000000000000100) )
#define ENTITY_FLAG_ENEMY               ( (short) (0b0000000000001000) )
#define ENTITY_FLAG_HURTS               ( (short) (0b0000000000010000) )

static const short entity_flags[] = {
	ENTITY_FLAG_BULLET_COLLIDABLE,
	ENTITY_FLAG_NONE,
	ENTITY_FLAG_BULLET,
	ENTITY_FLAG_SOLID,
	ENTITY_FLAG_ENEMY,
	ENTITY_FLAG_HURTS,
};

#endif
