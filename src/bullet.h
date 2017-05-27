#ifndef BULLET_H_
#define	BULLET_H_

#include <darnit/darnit.h>
#include <stdint.h>

struct BulletEntry {
	int			x;
	int			y;
	int			type;
	int			id;
};	


struct BulletList {
	struct BulletEntry	*bullet;
	int			bullets;
	uint8_t			id;
	DARNIT_TILESHEET	*ts;
};


int bullet_add(int type, int id, int x, int y);
void bullet_update(int x, int y, int id);
int bullet_destroy(int id);
void bullet_render_loop();
int bullet_loop();
int bullet_shoot(int tx, int ty, int type);


#endif
