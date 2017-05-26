#ifndef BLOCKLOGIC_H__
#define	BLOCKLOGIC_H__

#include <stdint.h>

struct BlockLogicBlock {
	uint8_t			*block;
	int			w;
	int			h;
};


struct BlockLogic {
	struct BlockLogicBlock	*block;
	int			blocks;
};



#endif
