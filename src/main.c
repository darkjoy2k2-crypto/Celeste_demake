#include <genesis.h>
#include "debug.h"
#include "globals.h"
#include "states.h"


int i = 0;

int main() {
    SPR_init(); 
    VDP_setScreenWidth320();



    FADE_init();


    

    debug_init();
    


    STATE_set(&State_InGame);

    while(1) {

        STATE_update();


    }

    return (0);
}