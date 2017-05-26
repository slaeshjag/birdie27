#include "block.h"

Block block_property[] = {
	[BLOCK_TYPE_BRICK] = {.mass = 100, .spawn_chance = 100, .tile = 21},
	[BLOCK_TYPE_BUS] = {.mass = 200, .spawn_chance = 100, .tile = 42},
	[BLOCK_TYPE_COW] = {.mass = 300, .spawn_chance = 100, .tile = 42},
	[BLOCK_TYPE_BANANAS] = {.mass = 400, .spawn_chance = 100, .tile = 48},
	[BLOCK_TYPE_CRATE] = {.mass = 500, .spawn_chance = 100, .tile = 47},
};

