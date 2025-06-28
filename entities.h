#ifndef ENTITIES_H
#define ENTITIES_H

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
/*
#define ENTITY_FLAG_SOLID  ( (short) (0b0000000000000001) )
#define ENTITY_FLAG_BULLET ( (short) (0b0000000000000010) )

short entity_flags[] = {
	0,
	0,
	ENTITY_FLAG_BULLET,
	ENTITY_FLAG_SOLID,
};
*/
#endif
