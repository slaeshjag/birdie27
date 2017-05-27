#include "movable.h"
#include "block.h"

Block block_property[BLOCK_TYPE_BLOCKS] = {
	[BLOCK_TYPE_BRICK] = {.mass = 300, .spawn_chance = 100, .tile = 21, COLLISION_ALL},
	[BLOCK_TYPE_BUS] = {.mass = 400, .spawn_chance = 100, .tile = 42, COLLISION_ALL},
	[BLOCK_TYPE_COW] = {.mass = 400, .spawn_chance = 100, .tile = 43, COLLISION_ALL},
	[BLOCK_TYPE_BANANAS] = {.mass = 100, .spawn_chance = 100, .tile = 48, COLLISION_ALL},
	[BLOCK_TYPE_CRATE] = {.mass = 100, .spawn_chance = 100, .tile = 47, COLLISION_ALL},
	
	[BLOCK_TYPE_LADDER] = {.mass = 100, .spawn_chance = 500, .tile = 22, COLLISION_TOP},
	[BLOCK_TYPE_BULLETPROOF] = {.mass = 500, .spawn_chance = 50, .tile = 23, COLLISION_ALL},
	[BLOCK_TYPE_DYNAMITE] = {.mass = 100, .spawn_chance = 50, .tile = 105, COLLISION_NONE},
	[BLOCK_TYPE_TURRET] = {.mass = 300, .spawn_chance = 30, .tile = 24, COLLISION_ALL},
	[BLOCK_TYPE_CANNON] = {.mass = 500, .spawn_chance = 20, .tile = 25, COLLISION_ALL},
};

int block_spawn() {
	int sum = 0;
	int i;
	int rnd;
	
	for(i = 0; i < BLOCK_TYPE_BLOCKS; i++) {
		sum += block_property[i].spawn_chance;
	}
	
	rnd = rand() % sum;
	
	for(i = 0; i < BLOCK_TYPE_BLOCKS; i++) {
		if(rnd < block_property[i].spawn_chance) {
			break;
		}
		rnd -= block_property[i].spawn_chance;
	}
	
	return i;
}
