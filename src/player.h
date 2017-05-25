#ifndef _PLAYER_H__
#define	_PLAYER_H__

#include "movable.h"

struct PlayerState {
	int last_walk_direction;
	int pulling;
	MOVABLE_ENTRY *holding;
};


#endif
