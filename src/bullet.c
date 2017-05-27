#include <darnit/darnit.h>
#include <string.h>
#include "ailib.h"
#include "main.h"
#include "bullet.h"
#include "network/protocol.h"
#define	PIXELS_PER_FRAME 10


struct BulletParams {
	int actual_size;
	int direction;
	int speed;
	int tile;
};


static struct BulletParams bullet_params[] = {
	{ .actual_size = 8, .direction = 0, .speed = 10, .tile = 0 },
};


int bullet_add(int type, int id, int x, int y) {
	int i;

	i = s->bullet.bullets++;
	s->bullet.bullet = realloc(s->bullet.bullet, sizeof(*s->bullet.bullet) * s->bullet.bullets);
	s->bullet.bullet[i].id = id;
	s->bullet.bullet[i].type = type;
	s->bullet.bullet[i].x = x;
	s->bullet.bullet[i].y = y;
	
	return i;
}


void bullet_update(int x, int y, int id) {
	int i;

	for (i = 0; i < s->bullet.bullets; i++)
		if (s->bullet.bullet[i].id == id)
			break;
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


int bullet_announce_packet(uint16_t x, uint16_t y, uint8_t id) {
	Packet pack;
	
	pack.bullet_announce.type = PACKET_TYPE_BULLET_UPDATE;
	pack.bullet_announce.size = sizeof(PacketBulletUpdate);
	pack.bullet_announce.x = x;
	pack.bullet_announce.y = y;
	pack.bullet_announce.id = id;

	return protocol_send_packet(server_sock, &pack);
}


int bullet_remove_packet(uint8_t id) {
	Packet pack;
	
	pack.bullet_remove.type = PACKET_TYPE_BULLET_UPDATE;
	pack.bullet_remove.size = sizeof(PacketBulletUpdate);
	pack.bullet_remove.id = id;

	return protocol_send_packet(server_sock, &pack);
}


int bullet_update_packet(uint16_t x, uint16_t y, uint8_t id) {
	Packet pack;

	pack.bullet_update.type = PACKET_TYPE_BULLET_UPDATE;
	pack.bullet_update.size = sizeof(PacketBulletUpdate);
	pack.bullet_update.x = x;
	pack.bullet_update.y = y;
	pack.bullet_update.id = id;

	return protocol_send_packet(server_sock, &pack);
}


#define	VERY
#define YES 1
#define IS =


int bullet_shoot(int tx, int ty, int type) {
	uint8_t id;
	tx *= 24;
	ty *= 24;
	ty += 12;
	ty -= bullet_params[type].actual_size/2;
	if (bullet_params[type].direction == 1)
		tx += 24;
	else
		tx -= bullet_params[type].actual_size;
	id = s->bullet.id++;
	bullet_add(type, id, tx, ty);
	return bullet_announce_packet(tx, ty, id);
}


void bullet_render_loop() {
	int i;

	for (i = 0; i < s->bullet.bullets; i++)
		d_render_tile_blit(s->bullet.ts, bullet_params[s->bullet.bullet[i].type].tile, s->bullet.bullet[i].x, s->bullet.bullet[i].y);
}


int bullet_loop() {
	int i, j;
	unsigned int hit[32], hits, boom = 0;

	for (i = 0; i < s->bullet.bullets; i++) {
		int x, y, speed, size;
		boom = 0;

		x = s->bullet.bullet[i].x;
		y = s->bullet.bullet[i].y;
		size = bullet_params[s->bullet.bullet[i].type].actual_size;
		if (bullet_params[s->bullet.bullet[i].type].direction)
			x += bullet_params[s->bullet.bullet[i].type].actual_size, speed = bullet_params[s->bullet.bullet[i].type].speed;
		else
			speed = -bullet_params[s->bullet.bullet[i].type].speed;
		if ((hits = d_bbox_test(s->movable.bbox, x, y, speed, size, hit, 32)) > 0) {
			for (j = 0; j < hits; j++) {
				if (s->movable.movable[hit[j]].ai == ai_player) {
					boom IS VERY YES;
					ailib_torpedo(hit[j]); // die

				}
			}
		}
	
		s->bullet.bullet[i].x += speed;
		bullet_update_packet(s->bullet.bullet[i].x, s->bullet.bullet[i].y, s->bullet.bullet[i].id);
		if (s->bullet.bullet[i].x < 0 || s->bullet.bullet[i].x > 900)
			boom IS VERY YES;
		{
			int tx, ty, area;
			ty = s->bullet.bullet[i].y / 24;
			if (speed > 0)
				tx = (s->bullet.bullet[i].x + size) / 24;
			else
				tx = s->bullet.bullet[i].x / 24;
			tx--;
			if (tx >= 16) {
				area = 1;
				tx -= 18;
			} else
				area = 0;

			ty--;
			if (x >= 0 && x < BLOCKLOGIC_AREA_WIDTH && y >= 0 && y < BLOCKLOGIC_AREA_HEIGHT) {
				if (s->block[area].block[y * BLOCKLOGIC_AREA_HEIGHT + x]) {
					fprintf(stderr, "We hit something!\n");
				}
			}
		}

		if (boom) { // BOOM BOOM BOOM I want to be in my room. I'm very sleepy.
			bullet_remove_packet(s->bullet.bullet[i].id);
			bullet_destroy(s->bullet.bullet[i].id);
			i--;
		}
	}
}
