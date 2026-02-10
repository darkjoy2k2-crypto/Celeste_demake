#include <genesis.h>
#include "fade.h"
#include "globals.h"
#include "states/states.h"
#include "states/states.h"

static u16 tick = 0;

static void enter() {

    VDP_setTextPalette(PAL2);

    VDP_drawText("GAME OVER", 10, 13);
    
    FADE_in(30, false); 
    tick = 0;
}

static void update() {

    u16 joy_state = JOY_readJoypad(JOY_1);

    /* Nach 3 Sekunden (180 Frames) ODER Tastendruck A */
    if (tick > 180 || (joy_state & BUTTON_A)) {
        STATE_set(&State_Title);
    }

    SYS_doVBlankProcess();
    tick++;
}

static void exit() {

    FADE_out(30, false);

    tick = 0;
}

const GameState State_GameOver = { enter, update, exit };