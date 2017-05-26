#include "ailib.h"
#include "movable.h"
#include "ingame.h"
#include "main.h"
#include "block.h"
#include "trigonometry.h"
#include "blocklogic.h"
//#include "soundeffects.h"
//#include "server/server.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>



int _get_block_from_entry(MOVABLE_ENTRY *self) {
	const char *playerid_str = d_map_prop(s->active_level->object[self->id].ref, "player_id");
	int i;

	for (i = 0; i < s->active_level->objects; i++)
		if (!strcmp(playerid_str, d_map_prop(s->active_level->object[i].ref, "block_id")))
			return i;
	fprintf(stderr, "bad block\n");
	return -1;
}


int _get_player_id(MOVABLE_ENTRY *self) {
	/* On a scale of 1 to italy, how inefficient is this? */
	const char *playerid_str;
	int movable_id, i;
	if (!(playerid_str = d_map_prop(s->active_level->object[self->id].ref, "player_id"))) {
		return -1;
	}

	//fprintf(stderr, "%s, %i\n", playerid_str, self->id);
	movable_id = atoi(playerid_str) - 1;
	//return movable_id;
	for (i = 0; i < PLAYER_CAP; i++) {
		if (!s->player[i].active)
			continue;
		//fprintf(stderr, "active %i, %i, %i\n", i, s->player[i].movable, movable_id);
		if (s->player[i].movable == movable_id)
			return i;
	}
	return -1;
}


static int _player_direction(MOVABLE_ENTRY *self) {
	int player_id;

	return 0;
	player_id = _get_player_id(self);
	if (player_id < 0 || player_id >= PLAYER_CAP)
		return 0;
	if (s->player[player_id].pulling > d_time_get())
		return (!s->player[player_id].last_walk_direction) + 4;
	
	return ((!s->player[player_id].last_walk_direction) | ((self->x_velocity != 0) << 1)) + (s->player[player_id].holding ? 6 : 0);
}


static int _player_fix_hitbox(MOVABLE_ENTRY *self) {
	int dir, box1_x, box2_x, box1_y, box2_y, box1_w, box2_w, box1_h, box2_h, diff;
	if ((movableTileCollision(self, 0, 0) & COLLISION_KILL) || (movableTileCollision(self, 0, -2) & COLLISION_KILL) || (movableTileCollision(self, -2, 0) & COLLISION_KILL) || (movableTileCollision(self, -2, -2) & COLLISION_KILL));
	// TODO: Do something with this //
	
	dir = _player_direction(self);
	if (dir == self->direction)
		return 0;
	d_sprite_hitbox(self->sprite, &box1_x, &box1_y, &box1_w, &box1_h);
	d_sprite_direction_set(self->sprite, dir);
	d_sprite_hitbox(self->sprite, &box2_x, &box2_y, &box2_w, &box2_h);
	diff = (((box2_y + box2_h) - (box1_y + box1_h)));
	self->y -= (diff * 1000);

	diff = (box2_x - box1_x);
	if (movableTileCollision(self, -1, -1) & COLLISION_RIGHT)
		self->x += abs(diff * 1000);
	else {
		if (movableTileCollision(self, 1, 1) & COLLISION_LEFT)
			self->x -= abs(diff * 2000);
	}

	return 1;
}


static void _reset_block(MOVABLE_ENTRY *me) {
	
}


void ai_block(void *dummy, void *entry, MOVABLE_MSG msg) {
	MOVABLE_ENTRY *self = entry;
	
	if (!s->is_host) {
		return;
	}

	switch (msg) {
		case MOVABLE_MSG_INIT:
			self->hp = self->hp_max = 12;
			self->x = 99999999; // Nobody should see us here... ///
			self->y = 99999999;
			self->gravity_effect = 0;
			self->direction = BLOCK_TYPE_CRATE;
			break;
		default:
			break;
	}
}


void ai_player(void *dummy, void *entry, MOVABLE_MSG msg) {
	MOVABLE_ENTRY *self = entry;
	int player_id;

	if (!s->is_host) {
		fprintf(stderr, "Not host\n");
		return;
	}
	switch (msg) {
		case MOVABLE_MSG_INIT:
			self->flag = 1;
			self->hp = self->hp_max = 400;
			self->gravity_effect = 1;
			//if (player_id >= PLAYER_CAP)	// TODO: replace PLAYER_CAP with actual number of connected players //
			//	self->hp = 0;
			//if (!server_player_is_present(player_id))
			//	self->hp = 0;
			break;
		case MOVABLE_MSG_LOOP:
			player_id = _get_player_id(self);
			//if (_player_fix_hitbox(self))
			//	break;
			if (self->flag) {
				if (player_id >= 0) {
					s->player[player_id].last_walk_direction = 0;
					s->player[player_id].holding = &s->movable.movable[_get_block_from_entry(self)];
					s->player[player_id].pulling = 0;
				}
				self->flag = 0;
				if (player_id < 0 || !s->player[player_id].active)
					self->hp = 0;
				self->direction = 0;
				return;
			}
			
			if (player_id < 0)
				return;
			if (ingame_keystate[player_id].left) {
				self->x_velocity = -400;
				s->player[player_id].last_walk_direction = 0;
			} else if (ingame_keystate[player_id].right) {
				self->x_velocity = 400;
				s->player[player_id].last_walk_direction = 1;
			} else {
				self->x_velocity = 0;
			}
			if (ingame_keystate[player_id].jump) {
				DARNIT_KEYS keys;
				
				ingame_keystate[player_id].jump = 0;
				
				if (!self->y_velocity) {
					if(!s->player[player_id].holding->direction) {
						self->y_velocity = -400;
					} else {
						self->y_velocity = -400 + block_property[s->player[player_id].holding->direction].mass/4;
					}
				}
			}

			if (ingame_keystate[player_id].action/* && !self->y_velocity*/) {
				int x, y, area;
				area = self->x < 432000?0:1;
				if (!s->player[player_id].holding->direction) {
					fprintf(stderr, "Nothing to park here\n");
				} else if (blocklogic_find_place_site(area, ((self->x - 24000)%(384000))/1000/24, (self->y - 24000)/1000/24, s->player[player_id].last_walk_direction, 1, 1, &x, &y)) {
					s->block[area].block[x + BLOCKLOGIC_AREA_WIDTH * y] = s->player[player_id].holding->direction;
					block_place(x, y, area, s->player[player_id].holding->direction);

					//s->player[player_id].holding->direction = 0;
				} else {
					fprintf(stderr, "can't park here\n");
				}
			}

			s->player[player_id].holding->x = self->x;
			s->player[player_id].holding->y = self->y - 24000;

			noinput:

			self->direction = _player_direction(self);
			break;
		case MOVABLE_MSG_DESTROY:
			/* TODO: Handle destroy, announce the dead. Etc. */
			break;
		default:
			break;

	}
}


static struct AILibEntry ailib[] = {
	{ "player", ai_player },
	{ "block", ai_block },
	{ NULL, NULL }
};

void *ailib_get(const char *str) {
	int i;

	for (i = 0; ailib[i].symbol; i++) {
		if (!strcmp(ailib[i].symbol, str))
			return (void *) ailib[i].func;
	}

	return NULL;
}
