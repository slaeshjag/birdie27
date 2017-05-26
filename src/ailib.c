#include "ailib.h"
#include "movable.h"
#include "ingame.h"
#include "main.h"
#include "trigonometry.h"
//#include "soundeffects.h"
//#include "server/server.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>


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
			fprintf(stderr, "%i %i\n", player_id, ingame_keystate[0].left);
			if (self->flag) {
				if (player_id >= 0) {
					s->player[player_id].last_walk_direction = 0;
					s->player[player_id].holding = NULL;
					s->player[player_id].pulling = 0;
				}
				self->flag = 0;
				if (player_id < 0 || !s->player[player_id].active)
					self->hp = 0;
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
				
				if (!self->y_velocity)
					self->y_velocity = -600;
			}


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
