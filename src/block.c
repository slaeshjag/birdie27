#include "block.h"

Block block_property[] = {
	[BLOCK_TYPE_BRICK] = {.weight = 100, .spawn_chance = 100, .tile = 21},
	[BLOCK_TYPE_BUS] = {.weight = 100, .spawn_chance = 100, .tile = 42},
	[BLOCK_TYPE_COW] = {.weight = 100, .spawn_chance = 100, .tile = 42},
	[BLOCK_TYPE_BANANAS] = {.weight = 100, .spawn_chance = 100, .tile = 45},
	[BLOCK_TYPE_CRATE] = {.weight = 100, .spawn_chance = 100, .tile = 46},
};

