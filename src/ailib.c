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


#define	ROOT_SPAWN_TIME ((rand() % 20) * 500 + 3000 + d_time_get())
#define	ROOT_PULL_TIME 1000
#define	ROOT_REGEN 40
#define	ROOT_FAVOURITE_DMG 100
#define	ROOT_GENERIC_DMG 70

enum RootType {
	ROOT_TYPE_CARROT,
	ROOT_TYPE_POTATO,
	ROOT_TYPE_ONION,
	ROOT_TYPE_RUTABAGA,
	ROOT_TYPE_TYPES
};

int _get_player_id(MOVABLE_ENTRY *self) {
	/* On a scale of 1 to italy, how inefficient is this? */
	const char *playerid_str;
	//if (!(playerid_str = d_map_prop(s->active_level->object[self->id].ref, "player_id")))
	//	return 0;
	return atoi(playerid_str) - 1;
}


static int _player_direction(MOVABLE_ENTRY *self) {
	int player_id;

	player_id = _get_player_id(self);
	//if (player_id < 0 || player_id >= PLAYER_CAP)
	//	return 0;
	//if (s->player[player_id].pulling > d_time_get())
	//	return (!s->player[player_id].last_walk_direction) + 4;
	
	//return ((!s->player[player_id].last_walk_direction) | ((self->x_velocity != 0) << 1)) + (s->player[player_id].holding ? 6 : 0);
	return 0;
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
	
	player_id = _get_player_id(self);

	#if 0
	switch (msg) {
		case MOVABLE_MSG_INIT:
			self->hp = self->hp_max = 400;
			self->gravity_effect = 1;
			s->player[player_id].last_walk_direction = 0;
			if (player_id >= PLAYER_CAP)	// TODO: replace PLAYER_CAP with actual number of connected players //
				self->hp = 0;
			if (!server_player_is_present(player_id))
				self->hp = 0;
			s->player[player_id].holding = NULL;
			s->player[player_id].pulling = 0;
			break;
		case MOVABLE_MSG_LOOP:
//			if (_player_fix_hitbox(self))
//				break;
			if (s->player[player_id].pulling > d_time_get())
				goto noinput;
			if (s->player[player_id].holding && (_root_type(s->player[player_id].holding) == player_id)) {
				_root_splat(s->player[player_id].holding);
				s->player[player_id].holding = NULL;

				server_sound(SOUND_CRUNCH);

				self->hp += ROOT_REGEN;
				if (self->hp > self->hp_max)
					self->hp = self->hp_max;
			}

			if (ingame_keystate[player_id].left) {
				self->x_velocity = -400;
				s->player[player_id].last_walk_direction = 0;
			} else if (ingame_keystate[player_id].right) {
				self->x_velocity = 400;
				s->player[player_id].last_walk_direction = 1;
			} else
				self->x_velocity = 0;
			if (ingame_keystate[player_id].jump) {
				DARNIT_KEYS keys;
				
				ingame_keystate[player_id].jump = 0;
				
				if (!self->y_velocity)
					self->y_velocity = -600;
			}

			if (ingame_keystate[player_id].grab && !s->player[player_id].holding) {
				int nearby[6], x, y, w, h, i, found;

				ingame_keystate[player_id].grab = 0;
				d_sprite_hitbox(self->sprite, &x, &y, &w, &h);
				x += self->x / 1000;
				y += self->y / 1000;
				found = d_bbox_test(s->movable.bbox, x, y, w, h, (unsigned int *) nearby, 6);
				for (i = 0; i < found; i++) {
					if (nearby[i] == self->id)
						continue;
					if (s->movable.movable[nearby[i]].ai == ai_root) {	// If it looks like a potato, and thinks like a potato, it probably is a potato
						if (_root_grab(&s->movable.movable[nearby[i]], self)) {
							s->player[player_id].pulling = d_time_get() + ROOT_PULL_TIME;
							server_sound(SOUND_UPROOT);
							s->player[player_id].holding = &s->movable.movable[nearby[i]];
						} else
							continue;
					}
						
				}
			} else if (ingame_keystate[player_id].grab) {
				_root_toss(s->player[player_id].holding, s->player[player_id].last_walk_direction);
				s->player[player_id].holding = NULL;
			}

			{
				int nearby[16], found, x, y, w, h, i;
				
				d_sprite_hitbox(self->sprite, &x, &y, &w, &h);
				x += self->x / 1000;
				y += self->y / 1000;
				found = d_bbox_test(s->movable.bbox, x, y, w, h, (unsigned int *) nearby, 16);
				for (i = 0; i < found; i++) {
					enum RootType type;
					if (s->movable.movable[nearby[i]].ai != ai_root)
						continue;	// Who cares about them pesky players? //
					if (!_root_is_hostile(&s->movable.movable[nearby[i]], self))
						continue;
					type = _root_type(&s->movable.movable[nearby[i]]);
					if (type == player_id) {
						self->hp -= ROOT_FAVOURITE_DMG;
						if (self->hp < 0)
							self->hp = 0;
						if (!self->hp) {
							server_sound(SOUND_HIT);
							server_sound(SOUND_HIT);
						} else {
							server_sound(SOUND_HIT);
						}
					} else {
						self->hp -= ROOT_GENERIC_DMG;
						if (self->hp < 0)
							self->hp = 0;
						if (!self->hp) {
							server_sound(SOUND_HIT);
							server_sound(SOUND_HIT);
						} else {
							server_sound(SOUND_HIT);
						}
					}

					/* Handle specific per-root effects here */
					if (type == ROOT_TYPE_POTATO) {

					} else if (type == ROOT_TYPE_CARROT) {
					
					} else if (type == ROOT_TYPE_RUTABAGA) {

					} else {

					}
					_root_splat(&s->movable.movable[nearby[i]]);
				}

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
	#endif
}


enum RootState {
	ROOT_STATE_INVISIBLE,
	ROOT_STATE_GROWING,
	ROOT_STATE_RIPE,
	ROOT_STATE_HELD,
	ROOT_STATE_THROWN
};



struct RootStateStruct {
	int home_x, home_y;
	enum RootState	state;
	int grow_progress;
	enum RootType type;
	MOVABLE_ENTRY *picker;
};


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
