#ifndef MAIN_H_
#define MAIN_H_

#include <stdbool.h>
#include <darnit/darnit.h>
#include "sfx.h"

#include "movable.h"
#include "player.h"
#include "bullet.h"
#include "blocklogic.h"
#include "turret.h"
#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600
#define PORT 1339

#define GAME_TITLE "Ikikaeru"

#define NAME_LEN_MAX 64
#define	PLAYER_CAP 32

#define PANE_R 0xCD
#define PANE_G 0xCD
#define PANE_B 0xCD


typedef enum GameState GameState;
enum GameState {
	GAME_STATE_MENU,
	GAME_STATE_SELECT_NAME,
	GAME_STATE_CHARACTERS,
	GAME_STATE_LOBBY,
	GAME_STATE_ENTER_IP,
	GAME_STATE_HOST,
	GAME_STATE_GAMEROOM,
	GAME_STATE_GAME,
	GAME_STATE_GAME_OVER,
	GAME_STATE_QUIT,
	GAME_STATES,
};

typedef struct Gfx Gfx;
struct Gfx {
	struct {
		DARNIT_FONT *small;
		DARNIT_FONT *large;
	} font;
};

extern Gfx gfx;
extern char player_name[];
extern int server_sock;
extern char *team_name[];


struct PlayerTimer {
	uint32_t		team1;
	uint32_t		team2;
	int			advantage;
};


struct GameStateStruct {
	int	player_id;
	bool is_host;
	MOVABLE			movable;
	DARNIT_MAP		*active_level;
	DARNIT_TILESHEET	*_7seg;
//	BULLET			bullet;
//	struct HealthbarStruct	healthbar;
	struct PlayerState	player[PLAYER_CAP];
	struct PlayerTimer	timer;
	struct BlockLogicBlock	block[2]; // One per team for now
	struct BulletList	bullet;
	struct Turret		turret;

	struct TMRenderHackContext *tmrender[16];
	
	DARNIT_PARTICLE *explosion[3];
	
	uint8_t tower[16][BLOCKLOGIC_AREA_HEIGHT*BLOCKLOGIC_AREA_WIDTH];
	int towers[2];
	struct {
		int x;
		int y;
	} center_of_gravity[16];
	
	SfxStruct sfx;
	struct {
				int x;
				int y;
				int follow;
	} camera;
};

extern struct GameStateStruct *s;

void game_state(GameState state);
void restart_to_menu(const char *name);
int join_game(unsigned long sip);

#endif
