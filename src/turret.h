#ifndef TURRET_H_
#define	TURRET_H_

#include <stdint.h>


struct TurretEntry {
	uint8_t			type;
	uint8_t			x;
	uint8_t			y;
	uint8_t			area;
	uint8_t			place_tick;
};


struct Turret {
	struct TurretEntry	*turret;
	int			turrets;

	uint8_t			turret_counter;
	uint8_t			cannon_counter;
};

void turret_place(uint8_t type, uint8_t x, uint8_t y, uint8_t area);
void turret_kill(uint8_t x, uint8_t y, uint8_t area);
void turret_loop();


#endif
