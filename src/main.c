#include <genesis.h>
#include "debug.h"
#include "globals.h"
#include "states/states.h"
#include "background.h"
#include "fonts.h"
#include "fade.h"
#include "level.h"

int i = 0;

int main() {
    SPR_init(); 


    FADE_init();
    LEVELS_init();

    

    debug_init();
    SYS_showFrameLoad(true);
    VDP_setScreenWidth256();

    STATE_set(&State_Title);

    while(1) {

        STATE_update();


    }

    return (0);
}