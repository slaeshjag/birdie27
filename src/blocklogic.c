#include "network/protocol.h"
#include "main.h"
#include "blocklogic.h"

int block_place(int x, int y, int team) {
	Packet pack;
	
	pack.type = PACKET_TYPE_BLOCK_PLACE;
	pack.size = sizeof(PacketBlockPlace);
	pack.block_place.x = x;
	pack.block_place.y = y;
	pack.block_place.team = team;
	
	return protocol_send_packet(server_sock, &pack);
}

int blocklogic_area_clear(int area, int x, int y, int w, int h) {
	int i, j;

	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++)
			if (s->block[area].block[i*BLOCKLOGIC_AREA_WIDTH+j])
				return 0;
	return 1;
}



int blocklogic_find_place_site(int area, int x, int y, int direction, int block_w, int block_h, int *pos_x, int *pos_y) {
	if (direction == 1) {
		x += 1;
	} else
		x -= block_w;
	if (x + block_w >= BLOCKLOGIC_AREA_WIDTH)
		return 0;
	if (x < 0)
		return 0;
	if (y - block_h + 1 < 0)
		return 0;
	if (y + 1 >= BLOCKLOGIC_AREA_HEIGHT)
		goto high;
	if (blocklogic_area_clear(area, x, y, block_w, block_h))
		*pos_x = x, *pos_y = y + 1;
high:
	if (y - block_h < 0)
		return 0;
	if (y >= BLOCKLOGIC_AREA_HEIGHT)
		return 0;
	*pos_x = x, *pos_y = y;
	return 1;
}


int blocklogic_find_place_splat(int area, int x, int y, int block_w, int block_h, int *pos_x, int *pos_y) {
	int i;

	if (x + block_w >= BLOCKLOGIC_AREA_WIDTH)
		return 0;
	if (x < 0)
		return 0;
	for (i = 0; i < 3; i++) {
		if (y - block_h - i < 0)
			return 0;
		if (y - 1 >= BLOCKLOGIC_AREA_HEIGHT)
			continue;
		if (!blocklogic_area_clear(area, x, y - i, block_w, block_h))
			continue;
		*pos_x = x;
		*pos_y = y;
		return 1;
	}

	return 0;
}
