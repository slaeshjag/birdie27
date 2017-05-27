#ifndef BULLET_H_
#define	BULLET_H_

#include <darnit/darnit.h>

struct BulletEntry {
	int			x;
	int			y;
	int			type;
	int			id;
};	


struct BulletList {
	struct BulletEntry	*bullet;
	int			bullets;
};


int bullet_add(int type, int id, int x, int y);
int bullet_update(int x, int y, int id);
int bullet_destroy(int id);


#endif
