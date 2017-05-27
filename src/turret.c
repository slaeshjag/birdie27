#include <string.h>
#include "main.h"
#include "turret.h"
#include "block.h"
#include "bullet.h"


void turret_place(uint8_t type, uint8_t x, uint8_t y, uint8_t area) {
	int i;
	
	i = s->turret.turrets++;
	s->turret.turret = realloc(s->turret.turret, sizeof(*s->turret.turret) * s->turret.turrets);
	s->turret.turret[i].x = x;
	s->turret.turret[i].y = y;
	s->turret.turret[i].area = area;
	s->turret.turret[i].type = type;
	s->turret.turret[i].place_tick = (type == BLOCK_TYPE_CANNON) ? s->turret.cannon_counter : s->turret.turret_counter;
	return;
}


void turret_kill(uint8_t x, uint8_t y, uint8_t area) {
	int i;

	for (i = 0; i < s->turret.turrets; i++) {
		if (s->turret.turret[i].x == x && s->turret.turret[i].y == y && s->turret.turret[i].area == area) {
			memmove(&s->turret.turret[i], &s->turret.turret[i+1], sizeof(s->turret.turret[i]) * s->turret.turrets - i - 1);
			s->turret.turrets--;
			return;
		}
	}
}


void turret_loop() {
	int i;

	s->turret.turret_counter++;
	s->turret.turret_counter &= 0x7F;
	if (!(s->turret.turret_counter & 0x3F))
		s->turret.cannon_counter++;
	if (s->turret.cannon_counter == 30)
		s->turret.cannon_counter = 0;
	
	for (i = 0; i < s->turret.turrets; i++) {
		if (s->turret.turret[i].type == BLOCK_TYPE_TURRET) {
			if (s->turret.turret[i].place_tick != s->turret.turret_counter)
				continue;
			bullet_shoot(s->turret.turret[i].x + 1 + s->turret.turret[i].area * 18, s->turret.turret[i].y + 1, s->turret.turret[i].area?2:0);
		} else {
			if (s->turret.turret[i].place_tick != s->turret.cannon_counter || s->turret.turret_counter != 0)
				continue;
			bullet_shoot(s->turret.turret[i].x + 1 + s->turret.turret[i].area * 18, s->turret.turret[i].y + 1, s->turret.turret[i].area?3:1);
		}
	}
}
