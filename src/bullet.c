#include <darnit/darnit.h>
#include "ailib.h"
#include "main.h"
#include "bullet.h"
#define	PIXELS_PER_FRAME 10


struct BulletParams {
	int actual_size;
	int direction;
	int speed;
	int tile;
};


static struct BulletParams bullet_params[] = {
	{ .actual_size = 8, .direction = 1, .speed = 10, .tile = 0 },
};


int bullet_add(int type, int id, int x, int y) {
	int i;

	i = s->bullet.bullets++;
	s->bullet.bullet = realloc(sizeof(*s->bullet.bullet) * s->bullet.bullets);
	s->bullet.bullet[i].id = id;
	s->bullet.bullet[i].type = type;
	s->bullet.bullet[i].x = x;
	s->bullet.bullet[i].y = y;
	return i;
}


int bullet_update(int x, int y, int id) {
	int i;

	for (i = 0; i < s->bullet.bullets; i++)
		if (s->bullet.bullet[i].id == id)
			break;
	if (i == s->bullet.bullets)
		return fprintf(stderr, "shit's broken\n"), -1;
	s->bullet.bullet[i].x = x;
	s->bullet.bullet[i].y = y;
}


int bullet_destroy(int id) {
	int i;

	for (i = 0; i < s->bullet.bullets; i++)
		if (s->bullet.bullet[i].id == id)
			break;
	if (i == s->bullet.bullets)
		return fprintf(stderr, "why did you break everything?\n"), -1;
	memmove(&s->bullet.bullet[i], &s->bullet.bullet[i + 1], sizeof(*s->bullet.bullet) * (s->bullet.bullets - i - 1));
	s->bullet.bullets--;
	return 0;
}


#define	VERY
#define YES 1
#define IS =

int bullet_loop() {
	int i, j;
	unsigned int hit[32], hits, boom = 0;

	for (i = 0; i < s->bullet.bullets; i++) {
		int x, y, speed, size;

		x = s->bullet.bullet[i].x;
		size = bullet_params[s->bullet.bullet[i].type].actual_size;
		if (bullet_params[s->bullet.bullet[i].type].direction)
			x += bullet_params[s->bullet.bullet[i].type].actual_size, speed = bullet_params[s->bullet.bullet[i].type].speed;
		else
			speed = -bullet_params[s->bullet.bullet[i].type].speed;
		if ((hits = d_bbox_test(s->movable.bbox, x*1000, y*1000, speed*1000, size * 1000, hits)) > 1) {
			for (j = 0; j < hits; j++) {
				if (s->movable.movable[hit[j]].ai == ai_player) {
					boom IS VERY YES;
					ailib_torpedo(hit[j]); // die

				}
			}
		}

		if 
		
	}
}
