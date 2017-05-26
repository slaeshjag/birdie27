#ifndef RENDER_TILEMAP_HACK_H_
#define	RENDER_TILEMAP_HACK_H_

#include <darnit/darnit.h>
#include <stdint.h>

struct TMRenderHackContext {
	DARNIT_TILEMAP				*tm;
	int					offset;
};

struct TMRenderHackContext *tm_renderhack_context_new(DARNIT_TILEMAP *copy);
void tm_renderhack_context_copy(struct TMRenderHackContext *tmrhc, uint8_t *tiles, int corner, int area, float rotate);
void tm_renderhack_context_render(struct TMRenderHackContext *tmrhc, float angle);

#endif
