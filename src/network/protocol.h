#ifndef __COMMON_PROTOCOL_H__
#define __COMMON_PROTOCOL_H__

#include <stdint.h>
#include "../main.h"
//#include "../ingame.h"


typedef enum PacketType PacketType;
enum PacketType {
	PACKET_TYPE_LOBBY,
	PACKET_TYPE_JOIN,
	PACKET_TYPE_START,
	PACKET_TYPE_MOVE_OBJECT,
	PACKET_TYPE_SOUND,
	PACKET_TYPE_KEYPRESS,
	PACKET_TYPE_EXIT,
};

typedef struct PacketLobby PacketLobby;
struct PacketLobby {
	uint16_t type;
	uint16_t size;
	
	char name[NAME_LEN_MAX];
};

typedef struct PacketJoin PacketJoin;
struct PacketJoin {
	uint16_t type;
	uint16_t size;
	
	uint32_t id;
	char name[NAME_LEN_MAX];
};

typedef struct PacketStart PacketStart;
struct PacketStart {
	uint16_t type;
	uint16_t size;
	
	uint32_t player_id;
};

typedef struct PacketKeypress PacketKeypress;
struct PacketKeypress {
	uint16_t type;
	uint16_t size;

	//struct InGameKeyStateEntry keypress, keyrelease;
};

typedef struct PacketParticleBurst PacketParticleBurst;
struct PacketParticleBurst {
	uint16_t type;
	uint16_t size;

	uint8_t point_size;
	uint8_t color_r;
	uint8_t color_g;
	uint8_t color_b;
	uint8_t color_a;
	uint8_t color_rt;
	uint8_t color_gt;
	uint8_t color_bt;
	uint8_t color_at;
	int16_t angle_min;
	int16_t angle_max;
	int16_t gravity_x;
	int16_t gravity_y;
	int16_t velocity_min;
	int16_t velocity_max;
	int16_t spawnrate;
	int16_t life;

	
};


typedef struct PacketSound PacketSound;
struct PacketSound {
	uint16_t type;
	uint16_t size;

	uint8_t sound;
};

typedef struct PacketExit PacketExit;
struct PacketExit {
	uint16_t type;
	uint16_t size;
	
	uint32_t player;
	char name[NAME_LEN_MAX];
};

typedef union Packet Packet;
union Packet {
	struct {
		uint16_t type;
		uint16_t size;
		uint8_t raw[512];
	};
	
	PacketLobby lobby;
	PacketJoin join;
	PacketStart start;
	PacketSound sound;
	PacketKeypress keypress;
	PacketExit exit;
};

int protocol_send_packet(int sock, Packet *pack);
int protocol_recv_packet(int sock, Packet *pack);

#endif
