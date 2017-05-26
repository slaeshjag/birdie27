#include <string.h>
#include <stdlib.h>
#include "network/protocol.h"
#include "main.h"
#include "blocklogic.h"
#include "block.h"

static int _mark_and_copy(uint8_t *data_in, uint8_t *data_out, int w, int h, int x, int y, uint8_t from, uint8_t to, int *center_of_gravity_x, int *center_of_gravity_y, int *mass_total) {
	int mass;
	if(x < 0 || x >= w || y < 0 || y >= h)
		return -1;
	
	if(data_in[y*w + x] == 0 || data_in[y*w + x] == to)
		return -1;
	
	mass = block_property[data_in[y*w + x]].mass;
	
	data_out[y*w + x] = data_in[y*w + x];
	data_in[y*w + x] = to;
	
	_mark_and_copy(data_in, data_out, w, h, x + 1, y, from, to, center_of_gravity_x, center_of_gravity_y, mass_total);
	_mark_and_copy(data_in, data_out, w, h, x - 1, y, from, to, center_of_gravity_x, center_of_gravity_y, mass_total);
	_mark_and_copy(data_in, data_out, w, h, x, y + 1, from, to, center_of_gravity_x, center_of_gravity_y, mass_total);
	_mark_and_copy(data_in, data_out, w, h, x, y - 1, from, to, center_of_gravity_x, center_of_gravity_y, mass_total);
	
	*mass_total += mass;
	*center_of_gravity_x += x*mass;
	*center_of_gravity_y += y*mass;
	
	return 0;
}

int block_place(int x, int y, int team, uint8_t block) {
	Packet pack;
	
	pack.type = PACKET_TYPE_BLOCK_PLACE;
	pack.size = sizeof(PacketBlockPlace);
	pack.block_place.x = x;
	pack.block_place.y = y;
	pack.block_place.team = team;
	pack.block_place.block = block;
	
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
	if (blocklogic_area_clear(area, x, y, block_w, block_h)) {
		*pos_x = x, *pos_y = y + 1;
		return 1;
	}
high:
	if (y - block_h < 0)
		return 0;
	if (y >= BLOCKLOGIC_AREA_HEIGHT)
		return 0;
	*pos_x = x, *pos_y = y;
	return 1;
}

#define BLOCK(x, y) (s->block[area].block[(y)*BLOCKLOGIC_AREA_WIDTH + (x)])

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

static int _separate_towers(int area, uint8_t * separated_field) {
	int fields = 0;
	int x, y;
	
	uint8_t *tempfield = malloc(BLOCKLOGIC_AREA_WIDTH*BLOCKLOGIC_AREA_HEIGHT);
	memcpy(tempfield, s->block[area].block, BLOCKLOGIC_AREA_HEIGHT*BLOCKLOGIC_AREA_WIDTH);
	
	y = BLOCKLOGIC_AREA_HEIGHT - 1;
	for(x = 0; x < BLOCKLOGIC_AREA_WIDTH; x++) {
		uint8_t block = BLOCK(x, y);
		if(block != 0 && block != 0xFF) {
			int center_of_gravity_x = 0, center_of_gravity_y = 0;
			int mass_total = 0;
			
			_mark_and_copy(tempfield, separated_field + (fields * BLOCKLOGIC_AREA_WIDTH*BLOCKLOGIC_AREA_HEIGHT), BLOCKLOGIC_AREA_WIDTH, BLOCKLOGIC_AREA_HEIGHT,
				x, y, block, 0xFF, &center_of_gravity_x, &center_of_gravity_y, &mass_total);
			
			if(mass_total) {
				center_of_gravity_x /= mass_total;
				center_of_gravity_y /= mass_total;
				
				printf("center of gravity: (%i %i)\n", center_of_gravity_x, center_of_gravity_y);
				s->center_of_gravity[fields].x = center_of_gravity_x;
				s->center_of_gravity[fields].y = center_of_gravity_y;
			}
			
			fields++;
		}
	}
	
	free(tempfield);
	
	return fields;
}

void blocklogic_separate_all_towers() {
	memset(s->tower, 0, 16*BLOCKLOGIC_AREA_HEIGHT*BLOCKLOGIC_AREA_WIDTH);
	
	s->towers[0] = _separate_towers(0, s->tower[0]);
	s->towers[1] = _separate_towers(1, s->tower[8]);
}

void spawn_projectile(int x, int y, double direction, int team) {
	
}

void trigger_weapons(DARNIT_TILEMAP *map, int team) {
	int x, y;
	int count, compare;
	
	for(y = 0; y < map->h; y++) {
		for(x = 0; x < map->w; x++) {
			if(map->data[y * map->w + x] & 0x80000000UL) {
				count = ((map->data[y * map->w + x]) >> 23) & 0xFF;
				compare = ((map->data[y * map->w + x]) >> (23 - 8)) & 0xFF;
				
				if(!((d_time_get() & 0xFF) ==  count)) {
					spawn_projectile(x, y, 0, team);
				}
				
				if(count == compare) {
					map->data[y * map->w + x] &= ~(0xFF << 23);
				}
					
			}
		}
	}
}


void blocklogic_copy_for_render() {
	int i;

	for (i = 0; i < 16; i++) {
		tm_renderhack_context_copy(s->tmrender[i], s->tower[i], 0, i>=8?1:0);
	}
}
