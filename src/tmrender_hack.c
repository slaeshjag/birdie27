#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include <darnit/darnit.h>
#include "tmrender_hack.h"
#include "darnit_tile_format.h"
#include "darnit_tilesheet.h"
#include "blocklogic.h"

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
int renderTilemapRecalcHack(TILE_CACHE *cache, TILESHEET *ts, int x, int y, int w, int h, int map_w, int map_h, unsigned int *tilemap, int inv_div, unsigned int mask);

void renderTilemapRenderHack(RENDER_TILEMAP *tm, float rotation, int offset_x) {
	glPushMatrix();
	glLoadIdentity();
	//fprintf(stderr, "offset=%i\n", offset_x);
	glTranslatef(tm->cam_x + (2./800*offset_x), tm->cam_y + 1.0f - 2.0/600*24, 0);
	glRotatef(rotation, 0., 0., 1.);
	renderCache(tm->cache, tm->ts, tm->cache_used);
	
	glLoadIdentity();
	glPopMatrix();
}


struct TMRenderHackContext *tm_renderhack_context_new(DARNIT_TILEMAP *copy) {
	struct TMRenderHackContext *tmrhc = malloc(sizeof(*tmrhc));

	tmrhc->tm = d_tilemap_new(0xFFF, ((RENDER_TILEMAP *) copy->render)->ts, 0xFFF, BLOCKLOGIC_AREA_WIDTH*2, BLOCKLOGIC_AREA_HEIGHT);
	d_tilemap_camera_move(tmrhc->tm, 0, 0);
	RENDER_TILEMAP *tm = tmrhc->tm->render;
	tm->cache_used = renderTilemapRecalcHack(tm->cache, tm->ts, tm->cam_xi, tm->cam_yi, tm->w, tm->h, tm->map_w, tm->map_h, tm->map, tm->inv_div, tm->mask);
	//d_tilemap_recalc(tmrhc->tm);

	return tmrhc;
}


void tm_renderhack_context_copy(struct TMRenderHackContext *tmrhc, uint8_t *tiles, int corner, int area) {
	RENDER_TILEMAP *tm = tmrhc->tm->render;
	memset(tmrhc->tm->data, 0, tmrhc->tm->w * tmrhc->tm->h * 4);
	int i, j;

	for (j = 0; j < BLOCKLOGIC_AREA_HEIGHT; j++) {
		for (i = 0; i < BLOCKLOGIC_AREA_WIDTH; i++) {
			tmrhc->tm->data[j*BLOCKLOGIC_AREA_WIDTH*2 + (BLOCKLOGIC_AREA_WIDTH - corner) + i] = tiles[BLOCKLOGIC_AREA_WIDTH*j+i];
		}
	}

	tm->cache_used = renderTilemapRecalcHack(tm->cache, tm->ts, BLOCKLOGIC_AREA_WIDTH - corner, tm->cam_yi, tm->w, tm->h, tm->map_w, tm->map_h, tm->map, tm->inv_div, tm->mask);
	tmrhc->offset = corner*24-BLOCKLOGIC_AREA_WIDTH*24-24+(area*24*BLOCKLOGIC_AREA_WIDTH);
}


void tm_renderhack_context_render(struct TMRenderHackContext *tmrhc, float angle) {
	renderTilemapRenderHack(tmrhc->tm->render, angle, tmrhc->offset);
}

int renderTilemapRecalcHack(TILE_CACHE *cache, TILESHEET *ts, int x, int y, int w, int h, int map_w, int map_h, unsigned int *tilemap, int inv_div, unsigned int mask) {
	float x_start, y_start;
	float *x_adv_buf, *y_adv_buf;
	int i, j, k, t, x_cur, y_cur;

	x_adv_buf = malloc(sizeof(float) * (w + 1));
	y_adv_buf = malloc(sizeof(float) * (h + 1));
	x_start = 0.0f;
	y_start = 0.0f;

	for (i = 0; i < (w + 1); i++)
		x_adv_buf[i] = x_start + ts->sw * (float) i;
	for (i = 0; i < (h + 1); i++)
		y_adv_buf[i] = y_start - ts->sh * (float) i;
	
	for (i = k = 0, x_cur = x; i < w; i++, x_cur++) {
		if (x_cur < 0 || x_cur >= map_w)
			continue;
		for (j = 0, y_cur = y; j < h; j++, y_cur++) {
			if (y_cur < 0 || y_cur >= map_h)
				continue;
			t = tilemap[x_cur + y_cur * map_w] & mask;
			if (t >= ts->tiles)
				continue;

			if (inv_div != 0)
				if (t % inv_div == 0)
					continue;
			/* ugh.. */
			cache[k].vertex[0].coord.x = x_adv_buf[i];
			cache[k].vertex[0].coord.y = y_adv_buf[j];
			cache[k].vertex[1].coord.x = x_adv_buf[i+1];
			cache[k].vertex[1].coord.y = y_adv_buf[j];
			cache[k].vertex[2].coord.x = x_adv_buf[i+1];
			cache[k].vertex[2].coord.y = y_adv_buf[j+1];
			cache[k].vertex[3].coord.x = x_adv_buf[i+1];
			cache[k].vertex[3].coord.y = y_adv_buf[j+1];
			cache[k].vertex[4].coord.x = x_adv_buf[i];
			cache[k].vertex[4].coord.y = y_adv_buf[j+1];
			cache[k].vertex[5].coord.x = x_adv_buf[i];
			cache[k].vertex[5].coord.y = y_adv_buf[j];
			cache[k].vertex[0].tex.u = ts->tile[t].r;
			cache[k].vertex[0].tex.v = ts->tile[t].s;
			cache[k].vertex[1].tex.u = ts->tile[t].u;
			cache[k].vertex[1].tex.v = ts->tile[t].s;
			cache[k].vertex[2].tex.u = ts->tile[t].u;
			cache[k].vertex[2].tex.v = ts->tile[t].v;
			cache[k].vertex[3].tex.u = ts->tile[t].u;
			cache[k].vertex[3].tex.v = ts->tile[t].v;
			cache[k].vertex[4].tex.u = ts->tile[t].r;
			cache[k].vertex[4].tex.v = ts->tile[t].v;
			cache[k].vertex[5].tex.u = ts->tile[t].r;
			cache[k].vertex[5].tex.v = ts->tile[t].s;
			k++;
		}
	}

	free(x_adv_buf);
	free(y_adv_buf);
	
	return k;
}
