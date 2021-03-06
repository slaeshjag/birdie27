#ifndef BLOCKLOGIC_H__
#define	BLOCKLOGIC_H__

#include <stdint.h>

#define	BLOCKLOGIC_AREA_WIDTH	16
#define	BLOCKLOGIC_AREA_HEIGHT	21

struct BlockLogicBlock {
	uint8_t			block[BLOCKLOGIC_AREA_WIDTH*BLOCKLOGIC_AREA_HEIGHT];
};


struct BlockLogic {
	struct BlockLogicBlock	*block;
	int			blocks;
	struct {
		int x;
		int y;
	} center_of_gravity;
};

void blocklogic_copy_for_render();
int blocklogic_find_place_site(int area, int x, int y, int direction, int block_w, int block_h, int *pos_x, int *pos_y);
int block_place(int x, int y, int team, uint8_t block);
void blocklogic_separate_all_towers();
int blocklogic_tower_size(int team);


#endif
