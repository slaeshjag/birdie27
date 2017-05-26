#include "block.h"

Block block_property[BLOCK_TYPE_BLOCKS] = {
	[BLOCK_TYPE_BRICK] = {.mass = 100, .spawn_chance = 100, .tile = 21},
	[BLOCK_TYPE_BUS] = {.mass = 200, .spawn_chance = 100, .tile = 42},
	[BLOCK_TYPE_COW] = {.mass = 300, .spawn_chance = 100, .tile = 42},
	[BLOCK_TYPE_BANANAS] = {.mass = 400, .spawn_chance = 100, .tile = 48},
	[BLOCK_TYPE_CRATE] = {.mass = 500, .spawn_chance = 100, .tile = 47},
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
			printf("you get block %i\n", i);
			break;
		}
		rnd -= block_property[i].spawn_chance;
	}
	
	return i;
}
