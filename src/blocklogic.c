#include "network/protocol.h"
#include "main.h"
#include "blocklogic.h"

int block_place(int x, int y, int team) {
	Packet pack;
	
	pack.type = PACKET_TYPE_BLOCK_PLACE;
	pack.size = sizeof(PacketBlockPlace);
	pack.block_place.x = x;
	pack.block_place.y = y;
	pack.block_place.team = team;
	
	return protocol_send_packet(server_sock, &pack);
}



