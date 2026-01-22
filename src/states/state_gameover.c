#include <genesis.h>
#include "fade.h"
#include "globals.h"
#include "states/states.h"
#include "states/states.h"

static u16 tick = 0;

static void enter() {
    /* RAM sauber machen */
    
    /* VRAM leeren */
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearPlane(WINDOW, TRUE);

    /* Palette für Namco-Font auf PAL0 setzen (für einfachen Text) */
    VDP_setTextPalette(PAL2);


    /* Text zentrieren */
    VDP_drawText("GAME OVER", 15, 13);
    
    /* Synchroner Fade In */
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
    /* Synchroner Fade Out */
    FADE_out(30, false);

    /* Alles sauber hinterlassen */
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    tick = 0;
}

const GameState State_GameOver = { enter, update, exit };