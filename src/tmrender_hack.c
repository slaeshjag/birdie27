#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include <darnit/darnit.h>
#include "tmrender_hack.h"
#include "blocklogic.h"
#include "block.h"

#define	TILESHEET void
#define	TILE_CACHE void
#define	SPRITE_ENTRY void

typedef struct {
	int				map_w;
	int				map_h;
	int				w;
	int				h;
	TILESHEET			*ts;
	TILE_CACHE			*cache;
	int				cache_used;
	unsigned int			inv_div;
	unsigned int			*map;
	float				cam_x;
	float				cam_y;
	int				cam_xi;
	int				cam_yi;
	int				cam_xi_c;
	int				cam_yi_c;
	int				cam_xp;
	int				cam_yp;
	unsigned int			mask;
	unsigned int			isometric;
	unsigned int			r_h;

	SPRITE_ENTRY			**sprite;
	unsigned int			sprites;
	unsigned int			sprites_used;
	unsigned int			*sprite_row;
	unsigned int			*tile_row;
} RENDER_TILEMAP;


void renderCache(TILE_CACHE *cache, TILESHEET *ts, int tiles);

void renderTilemapRenderHack(RENDER_TILEMAP *tm, float rotation, int offset_x) {
	glPushMatrix();
	glLoadIdentity();
	glRotatef(0., 0., 1., rotation);
	glTranslatef(tm->cam_x + (2./800*offset_x), tm->cam_y, 0);
	renderCache(tm->cache, tm->ts, tm->cache_used);
	
	glLoadIdentity();
	glPopMatrix();
}


struct TMRenderHackContext *tm_renderhack_context_new(DARNIT_TILEMAP *copy) {
	struct TMRenderHackContext *tmrhc = malloc(sizeof(*tmrhc));

	tmrhc->tm = d_tilemap_new(0xFFF, ((RENDER_TILEMAP *) copy->render)->ts, 0xFFF, BLOCKLOGIC_AREA_WIDTH*2, BLOCKLOGIC_AREA_HEIGHT);
	d_tilemap_camera_move(tmrhc->tm, 0, 0);
	d_tilemap_recalc(tmrhc->tm);

	return tmrhc;
}


void tm_renderhack_context_copy(struct TMRenderHackContext *tmrhc, uint8_t *tiles, int corner, int area) {
	memset(tmrhc->tm->data, 0, tmrhc->tm->w * tmrhc->tm->h * 4);
	int i, j;

	for (j = 0; j < BLOCKLOGIC_AREA_HEIGHT; j++) {
		for (i = 0; i < BLOCKLOGIC_AREA_WIDTH; i++) {
			tmrhc->tm->data[j*BLOCKLOGIC_AREA_WIDTH*2 + (BLOCKLOGIC_AREA_WIDTH - corner) + i] = block_property[tiles[BLOCKLOGIC_AREA_WIDTH*j+i]].tile;
		}
	}

	d_tilemap_recalc(tmrhc->tm);
	tmrhc->offset = corner*24-BLOCKLOGIC_AREA_WIDTH*24+(area*24*BLOCKLOGIC_AREA_WIDTH);
}


void tm_renderhack_context_render(struct TMRenderHackContext *tmrhc, float angle) {
	renderTilemapRenderHack(tmrhc->tm->render, angle, tmrhc->offset);
}
