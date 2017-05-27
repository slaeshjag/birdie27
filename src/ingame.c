#include "main.h"
#include "ingame.h"
#include "blocklogic.h"
#include "network/protocol.h"
#include "tmrender_hack.h"
#include <stdbool.h>
#include <string.h>
#include "network/network.h"
#include "network/protocol.h"
#include "server/server.h"
#include "main.h"
#include "block.h"
#include "gameover.h"
#include "sfx.h"
#include "util.h"
#include "bullet.h"
#include "turret.h"

InGameKeyStateEntry ingame_keystate[PLAYER_CAP];
int ingame_timer_package_send(uint8_t advantage, uint32_t team1, uint32_t team2);
void ingame_timer_blit(int time_left, int mode, int pos);

void ingame_init() {
	int i;
	const char *playerid_str;
	/* Leak *all* the memory */
	s->active_level = d_map_load(util_binrel_path("res/goesta.ldmz"));
	s->camera.follow = -1;
	s->camera.x = s->camera.y = 0;
	for (i = 0; i < 16; i++)
		s->tmrender[i] = tm_renderhack_context_new(s->active_level->layer[0].tilemap);

	movableInit();
//	bulletInit();
	movableLoad();
	s->_7seg = d_render_tilesheet_load("res/7seg.png", 24, 32, DARNIT_PFORMAT_RGB5A1);
	s->bullet.ts = d_render_tilesheet_load("res/bullets.png", 24, 24, DARNIT_PFORMAT_RGB5A1);
//	healthbar_init();
//	soundeffects_init();

/*	for (i = 0; i < s->movable.movables; i++) {
		if (!(playerid_str = d_map_prop(s->active_level->object[i].ref, "player_id")))
			continue;
		if (atoi(playerid_str) == s->player_id + 1) {
			s->camera.follow = i;
			break;
		}
	}
*/
}

void _win(int team) {
	Packet pack;
	
	pack.type = PACKET_TYPE_EXIT;
	pack.size = sizeof(PacketExit);
	
	pack.exit.team = team;
	
	protocol_send_packet(server_sock, &pack);
}


void ingame_loop() {
	int i, team1t, team2t;
	
	d_render_clearcolor_set(0x88, 0xf2, 0xff);
	
	d_render_tint(255, 255, 255, 255);
	
	movableLoop();
	
	if(s->is_host) {
		server_kick();
		team1t = blocklogic_tower_size(0);
		team2t = blocklogic_tower_size(1);
		s->timer.advantage = 0;
		if (team1t >= 4 || team2t >= 4) {
			if (team1t > team2t)
				s->timer.advantage = 1;
			else if (team2t > team1t)
				s->timer.advantage = 2;
		}
		if (s->timer.advantage == 1)
			s->timer.team1 += d_last_frame_time();
		else if (s->timer.advantage == 2)
			s->timer.team2 += d_last_frame_time();
		ingame_timer_package_send(s->timer.advantage, s->timer.team1, s->timer.team2);
		bullet_loop();
		turret_loop();
		
		if(s->timer.team1/1000 == TIMER_COUNTDOWN_WIN) {
			_win(0);
		} else if(s->timer.team2/1000 == TIMER_COUNTDOWN_WIN) {
			_win(1);
		}
	}
	
//	camera_work();
	d_map_camera_move(s->active_level, 30, 0);
	
	for (i = 0; i < s->active_level->layers; i++) {
		d_render_offset(0, 0);
		d_render_tint(255, 255, 255, 255);
//		d_render_tile_blit(s->active_level->layer[i].ts, 0, 0, 1);
		d_tilemap_draw(s->active_level->layer[i].tilemap);
		d_render_offset(30, 0);
		
	}
	for (i = 0; i < 16; i++)
		tm_renderhack_context_render(s->tmrender[i], 0.f);
	
	bullet_render_loop();
	
	for (i = 0; i < s->active_level->layers; i++) {
		movableLoopRender(i);
	}
	
	for(i = 0; i < PLAYER_CAP; i++) {
		if(s->player[i].active) {
			d_particle_draw(s->player[i].blood);
		}
	}
	
	for(i = 0; i < 3; i++)
		d_particle_draw(s->explosion[i]);
	
	d_render_offset(0, 0);
	ingame_timer_blit(TIMER_COUNTDOWN_WIN - s->timer.team1/1000, s->timer.advantage == 1 ? 1 : 0, 0);
	ingame_timer_blit(TIMER_COUNTDOWN_WIN - s->timer.team2/1000, s->timer.advantage == 2 ? 3 : 2, 704);
//	healthbar_draw();
	ingame_client_keyboard();
}


void ingame_client_keyboard() {
	static struct InGameKeyStateEntry oldstate = {};
	struct InGameKeyStateEntry newstate, pressevent, releaseevent;

	memset(&pressevent, 0, sizeof(pressevent));
	memset(&releaseevent, 0, sizeof(pressevent));
	
	newstate.left = d_keys_get().left;
	newstate.right = d_keys_get().right;
	newstate.jump = d_keys_get().up;
	newstate.action = d_keys_get().a;
	newstate.suicide = d_keys_get().x;
	
	if(d_keys_get().select)
		restart_to_menu(player_name);
	
	
	if (newstate.left ^ oldstate.left) {
		if (newstate.left)
			pressevent.left = true, releaseevent.left = false;
		else
			releaseevent.left = true, pressevent.left = false;
	}

	if (newstate.right ^ oldstate.right) {
		if (newstate.right)
			pressevent.right = true, releaseevent.right = false;
		else
			releaseevent.right = true, pressevent.right = false;
	}
	
	if (newstate.jump ^ oldstate.jump) {
		if (newstate.jump)
			pressevent.jump = true, releaseevent.jump = false;
		else
			releaseevent.jump = true, pressevent.jump = false;
	}

	if (newstate.action ^ oldstate.action) {
		if (newstate.action)
			pressevent.action = true, releaseevent.action = false;
		else
			releaseevent.action = true, pressevent.action = false;
	}
	
	if (newstate.suicide ^ oldstate.suicide) {
		if (newstate.suicide)
			pressevent.suicide = true, releaseevent.suicide = false;
		else
			releaseevent.suicide = true, pressevent.suicide = false;
	}
	
	PacketKeypress kp;

	kp.size = sizeof(kp);
	kp.type = PACKET_TYPE_KEYPRESS;
	kp.keypress = pressevent;
	kp.keyrelease = releaseevent;

	protocol_send_packet(server_sock, (void *) &kp);

	oldstate = newstate;
}

void ingame_network_handler() {
	Packet pack;
	void *p;
	int i;
	
	while(network_poll_tcp(server_sock)) {
		
		protocol_recv_packet(server_sock, &pack);
		
		switch(pack.type) {
			case PACKET_TYPE_MOVE_OBJECT:
				p = pack.raw;
				
				for(i = 0; i < s->movable.movables; i++) {
					s->movable.movable[i].x = ((int) (*((uint16_t *) p))) * 1000;
					p+= 2;
					s->movable.movable[i].y = ((int) (*((uint16_t *) p))) * 1000;
					p+= 2;
					s->movable.movable[i].direction = *((uint8_t *) p);
					p+= 1;
					s->movable.movable[i].angle = *((uint8_t *) p);
					s->movable.movable[i].angle *= (2 * 10);
					p += 1;
					s->movable.movable[i].hp = *((uint16_t *) p);
					p += 2;
					s->movable.movable[i].hp_max = *((uint16_t *) p);
					p += 2;
				}
				break;
			
			case PACKET_TYPE_BLOCK_PLACE:
				switch(pack.block_place.block) {
					case BLOCK_TYPE_COW:
						sfx_play(SFX_COW);
						break;
					case BLOCK_TYPE_BUS:
						sfx_play(SFX_CAR);
						break;
					case BLOCK_TYPE_TURRET:
					case BLOCK_TYPE_CANNON:
						if (s->is_host) {
							turret_place(pack.block_place.block, pack.block_place.x, pack.block_place.y, pack.block_place.team);
						}
				}
				
				s->block[pack.block_place.team].block[pack.block_place.y*BLOCKLOGIC_AREA_WIDTH + pack.block_place.x] = pack.block_place.block;
				blocklogic_separate_all_towers();
				blocklogic_copy_for_render();
				break;
			
			case PACKET_TYPE_EXPLOSION:
				s->block[pack.explosion.team].block[pack.explosion.y*BLOCKLOGIC_AREA_WIDTH + pack.explosion.x] = 0;
				if(pack.explosion.x < BLOCKLOGIC_AREA_WIDTH - 1)
					s->block[pack.explosion.team].block[pack.explosion.y*BLOCKLOGIC_AREA_WIDTH + pack.explosion.x + 1] = 0;
				if(pack.explosion.x > 0)
					s->block[pack.explosion.team].block[pack.explosion.y*BLOCKLOGIC_AREA_WIDTH + pack.explosion.x - 1] = 0;
				if(pack.explosion.y < BLOCKLOGIC_AREA_HEIGHT - 1)
					s->block[pack.explosion.team].block[(pack.explosion.y + 1)*BLOCKLOGIC_AREA_WIDTH + pack.explosion.x] = 0;
				if(pack.explosion.y > 0)
					s->block[pack.explosion.team].block[(pack.explosion.y - 1)*BLOCKLOGIC_AREA_WIDTH + pack.explosion.x] = 0;
				
				blocklogic_separate_all_towers();
				blocklogic_copy_for_render();
				
				for(i = 0; i < 3; i++) {
					d_particle_emitter_move(s->explosion[i], pack.explosion.x*24, pack.explosion.y*24);
					d_particle_pulse(s->explosion[i]);
				}
				sfx_play(SFX_EXPLOSION);
				
				break;
			
			case PACKET_TYPE_BLOOD:
				d_particle_emitter_move(s->player[pack.blood.player].blood, pack.blood.x, pack.blood.y);
				d_particle_pulse(s->player[pack.blood.player].blood);
				sfx_play(SFX_SPLATTER);
				break;

			case PACKET_TYPE_TIMER:
				s->timer.advantage = pack.timer.advantage;
				s->timer.team1 = pack.timer.team1;
				s->timer.team2 = pack.timer.team2;
				break;
			
			case PACKET_TYPE_BULLET_ANNOUNCE:
				if (s->is_host)
					bullet_add(pack.bullet_announce.bullet_type, pack.bullet_announce.id, pack.bullet_announce.x, pack.bullet_announce.y);
				break;
			case PACKET_TYPE_BULLET_UPDATE:

				if (s->is_host)
					bullet_update(pack.bullet_update.x, pack.bullet_update.y, pack.bullet_update.id);
				break;
			case PACKET_TYPE_BULLET_REMOVE:
				if (s->is_host)
					bullet_destroy(pack.bullet_remove.id);
				break;
			case PACKET_TYPE_SOUND:
	//			soundeffects_play(pack.sound.sound);
				break;
			case PACKET_TYPE_EXIT:
				game_over_set_team(pack.exit.team);
				game_state(GAME_STATE_GAME_OVER);
				break;
		}
	}
}


int ingame_timer_package_send(uint8_t advantage, uint32_t team1, uint32_t team2) {
	Packet pack;
	
	pack.type = PACKET_TYPE_TIMER;
	pack.size = sizeof(PacketTimer);
	pack.timer.advantage = advantage;
	pack.timer.team1 = team1;
	pack.timer.team2 = team2;

	return protocol_send_packet(server_sock, &pack);
}


void ingame_timer_blit(int time_left, int mode, int pos) {
	int minute, deka, second;

	minute = time_left / 60;
	deka = (time_left % 60) / 10;
	second = time_left % 10;
	d_render_tile_blit(s->_7seg, minute + 11*mode, pos, 568);
	d_render_tile_blit(s->_7seg, 10 + 11*mode, pos+24, 568);
	d_render_tile_blit(s->_7seg, deka + 11*mode, pos+48, 568);
	d_render_tile_blit(s->_7seg, second + 11*mode, pos+72, 568);
}
