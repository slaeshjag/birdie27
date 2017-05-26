#ifndef __COMMON_PROTOCOL_H__
#define __COMMON_PROTOCOL_H__

#include <stdint.h>
#include "../main.h"
#include "../ingame.h"


typedef enum PacketType PacketType;
enum PacketType {
	PACKET_TYPE_LOBBY,
	PACKET_TYPE_JOIN,
	PACKET_TYPE_TEAM,
	PACKET_TYPE_START,
	PACKET_TYPE_MOVE_OBJECT,
	PACKET_TYPE_SOUND,
	PACKET_TYPE_KEYPRESS,
	PACKET_TYPE_BLOCK_PLACE,
	PACKET_TYPE_BLOOD,
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
	int team;
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

	InGameKeyStateEntry keypress, keyrelease;
};

typedef struct PacketBlood PacketBlood;
struct PacketBlood {
	uint16_t type;
	uint16_t size;
	
	uint32_t player;
	uint32_t x;
	uint32_t y;
};


typedef struct PacketSound PacketSound;
struct PacketSound {
	uint16_t type;
	uint16_t size;

	uint8_t sound;
};


typedef struct PacketBlockPlace PacketBlockPlace;
struct PacketBlockPlace {
	uint16_t type;
	uint16_t size;
	
	uint8_t team;
	uint8_t	x;
	uint8_t	y;
	uint8_t block;
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
		uint8_t raw[1024];
	};
	
	PacketLobby lobby;
	PacketJoin join;
	PacketStart start;
	PacketSound sound;
	PacketKeypress keypress;
	PacketExit exit;
	PacketBlockPlace block_place;
	PacketBlood blood;
};

int protocol_send_packet(int sock, Packet *pack);
int protocol_recv_packet(int sock, Packet *pack);

#endif
