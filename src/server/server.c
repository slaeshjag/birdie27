#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <darnit/darnit.h>
#include "../block.h"
#include "../main.h"
#include "../network/network.h"
#include "../network/protocol.h"

#define HANDLE_KEY(A) do { \
		if(pack.keypress.keypress.A ) \
			ingame_keystate[cli->id].A = 1; \
		if(pack.keypress.keyrelease.A ) \
			ingame_keystate[cli->id].A = 0; \
	} while(0)

int usleep(useconds_t usec);

typedef enum ServerState ServerState;
enum ServerState {
	SERVER_STATE_LOBBY,
	SERVER_STATE_STARTING,
	SERVER_STATE_GAME,
};

typedef struct ClientList ClientList;
struct ClientList {
	int id;
	int sock;
	char name[NAME_LEN_MAX];
	int team;
	
	ClientList *next;
};

static int listen_sock;
static ClientList *client = NULL;
static int clients = 0;
static volatile ServerState server_state;

DARNIT_SEMAPHORE *sem;

void server_handle_client(ClientList *cli) {
	Packet pack;
	Packet response;
	ClientList *tmp;
	
	while(network_poll_tcp(cli->sock)) {
		protocol_recv_packet(cli->sock, &pack);
		
		switch(pack.type) {
			case PACKET_TYPE_JOIN:
				strcpy(cli->name, pack.join.name);
				cli->team = pack.join.team;
				
				response.type = PACKET_TYPE_JOIN;
				response.size = sizeof(PacketJoin);
				
				for(tmp = client; tmp; tmp = tmp->next) {
					memset(response.join.name, 0, NAME_LEN_MAX);
					strcpy(response.join.name, tmp->name);
					response.join.team = tmp->team;
					response.join.id = tmp->id;
					protocol_send_packet(cli->sock, &response);
					
					if(tmp->sock != cli->sock) {
						response.join.id = cli->id;
						strcpy(response.join.name, cli->name);
						response.join.team = cli->team;
						protocol_send_packet(tmp->sock, &response);
					}
				}
				
				break;
			
			case PACKET_TYPE_KEYPRESS:
				HANDLE_KEY(left);
				HANDLE_KEY(right);
				HANDLE_KEY(jump);
				HANDLE_KEY(action);
				HANDLE_KEY(suicide);
				break;
			
			case PACKET_TYPE_BLOCK_PLACE:
				response.type = PACKET_TYPE_BLOCK_PLACE;
				response.size = sizeof(PacketBlockPlace);
				response.block_place.x = pack.block_place.x;
				response.block_place.y = pack.block_place.y;
				response.block_place.team = pack.block_place.team;
				response.block_place.block = pack.block_place.block;
				
				for(tmp = client; tmp; tmp = tmp->next) {
					protocol_send_packet(tmp->sock, &response);
				}
				
				if(pack.block_place.block == BLOCK_TYPE_DYNAMITE) {
					response.type = PACKET_TYPE_EXPLOSION;
					response.size = sizeof(PacketExplosion);
					response.explosion.x = pack.block_place.x;
					response.explosion.y = pack.block_place.y;
					response.explosion.team = pack.block_place.team;
					
					for(tmp = client; tmp; tmp = tmp->next) {
						protocol_send_packet(tmp->sock, &response);
					}
				}
				break;
			
			case PACKET_TYPE_BLOOD:
				response.type = PACKET_TYPE_BLOOD;
				response.size = sizeof(PacketBlood);
				response.blood.x = pack.blood.x;
				response.blood.y = pack.blood.y;
				response.blood.player = pack.blood.player;
				
				for(tmp = client; tmp; tmp = tmp->next) {
					protocol_send_packet(tmp->sock, &response);
				}
				break;
			case PACKET_TYPE_TIMER:
				response.type = PACKET_TYPE_TIMER;
				response.size = sizeof(PacketTimer);
				response.timer.advantage = pack.timer.advantage;;
				response.timer.team1 = pack.timer.team1;
				response.timer.team2 = pack.timer.team2;
				
				for(tmp = client; tmp; tmp = tmp->next) {
					protocol_send_packet(tmp->sock, &response);
				}
				break;
			case PACKET_TYPE_BULLET_ANNOUNCE:
				response.type = PACKET_TYPE_BULLET_ANNOUNCE;
				response.size = sizeof(PacketBulletAnnounce);
				response.bullet_announce.x = pack.bullet_announce.x;
				response.bullet_announce.y = pack.bullet_announce.y;
				response.bullet_announce.id = pack.bullet_announce.id;
				response.bullet_announce.bullet_type = pack.bullet_announce.bullet_type;
				
				for(tmp = client; tmp; tmp = tmp->next) {
					protocol_send_packet(tmp->sock, &response);
				}
				break;
			case PACKET_TYPE_BULLET_UPDATE:
				response.type = PACKET_TYPE_BULLET_UPDATE;
				response.size = sizeof(PacketBulletUpdate);
				response.bullet_update.x = pack.bullet_update.x;
				response.bullet_update.y = pack.bullet_update.y;
				response.bullet_update.id = pack.bullet_update.id;
				
				for(tmp = client; tmp; tmp = tmp->next) {
					protocol_send_packet(tmp->sock, &response);
				}
				break;
			case PACKET_TYPE_EXIT:
				response.type = PACKET_TYPE_EXIT;
				response.size = sizeof(PacketExit);
				response.exit.team = pack.exit.team;
				
				for(tmp = client; tmp; tmp = tmp->next) {
					protocol_send_packet(tmp->sock, &response);
				}
				break;
			case PACKET_TYPE_BULLET_REMOVE:
				response.type = PACKET_TYPE_BULLET_REMOVE;
				response.size = sizeof(PacketBulletRemove);
				response.bullet_remove.id = pack.bullet_remove.id;
				
				for(tmp = client; tmp; tmp = tmp->next) {
					protocol_send_packet(tmp->sock, &response);
				}
				break;
			default:
				fprintf(stderr, "wat %i\n", pack.type);
				break;
		}
	}
}

int server_thread(void *arg) {
	PacketLobby pack;
	Packet move;
	ClientList *tmp;
	void *p;
	int i;
	
	for(;;) {
		switch(server_state) {
			case SERVER_STATE_LOBBY:
				if(network_poll_tcp(listen_sock)) {
					int sock;
					
					sock = network_accept_tcp(listen_sock);
					
					tmp = malloc(sizeof(ClientList));
					tmp->sock = sock;
					tmp->id = clients++;
					tmp->next = client;
					client = tmp;
				}
				
				for(tmp = client; tmp; tmp = tmp->next)
					server_handle_client(tmp);
				
				
				pack.type = PACKET_TYPE_LOBBY;
				pack.size = sizeof(PacketLobby);
				memset(pack.name, 0, NAME_LEN_MAX);
				strcpy(pack.name, player_name);
				
				network_broadcast_udp(&pack, pack.size);
				usleep(100000);
				break;
				
			case SERVER_STATE_STARTING:
				for(tmp = client; tmp; tmp = tmp->next) {
					PacketStart start;
					
					start.type = PACKET_TYPE_START;
					start.size = sizeof(PacketStart);
					
					start.player_id = tmp->id;
					
					protocol_send_packet(tmp->sock, (void *) &start);
				}
				
				server_state = SERVER_STATE_GAME;
				break;
				
			case SERVER_STATE_GAME:
				d_util_semaphore_wait(sem);
				
				move.type = PACKET_TYPE_MOVE_OBJECT;
				move.size = 4 + s->movable.movables*10;
				p = move.raw;
				
				for(i = 0; i < s->movable.movables; i++) {
					*((uint16_t *) p) = (s->movable.movable[i].x)/1000;
					p+= 2;
					*((uint16_t *) p) = (s->movable.movable[i].y)/1000;
					p+= 2;
					*((uint8_t *) p) = s->movable.movable[i].direction;
					p+= 1;
					*((uint8_t *) p) = (s->movable.movable[i].angle / 10 / 2);
					p += 1;
					*((uint16_t *) p) = (s->movable.movable[i].hp);
					p += 2;
					*((uint16_t *) p) = (s->movable.movable[i].hp_max);
					p += 2;
				}
				
				for(tmp = client; tmp; tmp = tmp->next) {
					if(tmp->id != s->player_id)
						protocol_send_packet(tmp->sock, &move);
				}
				
				for(tmp = client; tmp; tmp = tmp->next)
					server_handle_client(tmp);
				break;
		}
	}
	
	return 0;
}

void server_start() {
	sem = d_util_semaphore_new(0);
	server_state = SERVER_STATE_LOBBY;
	if((listen_sock = network_listen_tcp(PORT + 1)) < 0) {
		fprintf(stderr, "Server failed to open listening socket\n");
		exit(1);
	}
	d_util_thread_new(server_thread, NULL);
}

/*void server_sound(enum SoundeffectSound sound) {
	PacketSound pack;
	ClientList *tmp;

	pack.type = PACKET_TYPE_SOUND;
	pack.size = sizeof(pack);
	pack.sound = sound;
	for (tmp = client; tmp; tmp = tmp->next)
		protocol_send_packet(tmp->sock, &pack);
	return;
}*/


void server_start_game() {
	server_state = SERVER_STATE_STARTING;
}

void server_kick() {
	d_util_semaphore_add(sem, 1);
}

bool server_player_is_present(int id) {
	struct ClientList *next;

	for (next = client; next; next = next->next)
		if (next->id == id)
			return true;
	return false;
}

void server_shutdown() {
	ClientList *tmp;
	
	network_close_udp();
	
	for(tmp = client; tmp; tmp = tmp->next)
		network_close_tcp(tmp->sock);
	
	network_close_tcp(listen_sock);
	
}

