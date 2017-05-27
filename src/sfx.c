#include "sfx.h"
#include "main.h"

void sfx_init() {
	s->sfx.sound[SFX_SPLATTER] = d_sound_streamed_load("res/sfx/splatter.ogg", DARNIT_SOUND_PRELOAD, 1);
	s->sfx.sound[SFX_CAR] = d_sound_streamed_load("res/sfx/car.ogg", DARNIT_SOUND_PRELOAD, 1);
	s->sfx.sound[SFX_COW] = d_sound_streamed_load("res/sfx/cow.ogg", DARNIT_SOUND_PRELOAD, 1);
	s->sfx.sound[SFX_EXPLOSION] = d_sound_streamed_load("res/sfx/explosion.ogg", DARNIT_SOUND_PRELOAD, 1);
}


void sfx_play(SfxSound sound) {
	d_sound_play(s->sfx.sound[sound], -1, 70, 70, 0);
	return;
}
