#include <genesis.h>
#include "debug.h"
#include "globals.h"
#include "states/states.h"
#include "title.h"

int i = 0;

int main() {
    SPR_init(); 
    VDP_setScreenWidth320();
    VDP_loadFont(&my_font_tiles , DMA);


    FADE_init();


    

    debug_init();
    SYS_showFrameLoad(true);


    STATE_set(&State_Title);

    while(1) {

        STATE_update();


    }

    return (0);
}