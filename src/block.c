#include "block.h"

Block block_property[] = {
	[BLOCK_TYPE_BRICK] = {.mass = 100, .spawn_chance = 100, .tile = 21},
	[BLOCK_TYPE_BUS] = {.mass = 100, .spawn_chance = 100, .tile = 42},
	[BLOCK_TYPE_COW] = {.mass = 100, .spawn_chance = 100, .tile = 42},
	[BLOCK_TYPE_BANANAS] = {.mass = 100, .spawn_chance = 100, .tile = 45},
	[BLOCK_TYPE_CRATE] = {.mass = 100, .spawn_chance = 100, .tile = 46},
};

