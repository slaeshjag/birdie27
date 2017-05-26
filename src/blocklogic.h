#ifndef BLOCKLOGIC_H_
#define	BLOCKLOGIC_H_

#include <stdint.h>

#define	BLOCKLOGIC_AREA_WIDTH	16
#define	BLOCKLOGIC_AREA_HEIGHT	21

struct BlockLogicBlock {
	uint8_t			block[BLOCKLOGIC_AREA_WIDTH*BLOCKLOGIC_AREA_HEIGHT];
};


struct BlockLogic {
	struct BlockLogicBlock	*block;
	int			blocks;
};

void blocklogic_separate_all_towers();


#endif
