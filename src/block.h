#ifndef BLOCK_H_
#define BLOCK_H_

typedef enum BlockType BlockType;
enum BlockType {
	BLOCK_TYPE_NONE,
	BLOCK_TYPE_BRICK,
	BLOCK_TYPE_BUS,
	BLOCK_TYPE_COW,
	BLOCK_TYPE_BANANAS,
	BLOCK_TYPE_CRATE,
	
	BLOCK_TYPE_LADDER,
	BLOCK_TYPE_BULLETPROOF,
	BLOCK_TYPE_TURRET,
	BLOCK_TYPE_CANNON,
	
	BLOCK_TYPE_BLOCKS,
};

typedef struct Block Block;
struct Block {
	int mass;
	int spawn_chance;
	int tile;
};

extern Block block_property[];

int block_spawn();

#endif
