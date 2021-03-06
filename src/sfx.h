#ifndef SFX_H_
#define	SFX_H_


#include <darnit/darnit.h>

typedef enum SfxSound SfxSound;
enum SfxSound {
	SFX_SPLATTER,
	SFX_CAR,
	SFX_COW,
	SFX_EXPLOSION,
	
	SFX_SOUNDS,
};

typedef struct SfxStruct SfxStruct;
struct SfxStruct {
	DARNIT_SOUND		*sound[SFX_SOUNDS];
};

void sfx_init();
void sfx_play(SfxSound sound);


#endif
