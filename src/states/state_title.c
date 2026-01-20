#include <genesis.h>
#include "debug.h"
#include "fade.h"
#include "globals.h"
#include "states/states.h"
#include "title.h"

static u16 blink_timer;
static bool show_text;

static void enter() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    blink_timer = 0;
    show_text = TRUE;

    VDP_setTextPalette(PAL2);
    VDP_drawText("CELESTE DEMAKE", 13, 8);
    VDP_drawText("Darkjoy2k2 @ 2026", 11, 10);

    FADE_in(15, false);

}

static void update() {
    blink_timer++;

    if (blink_timer >= 30) {
        blink_timer = 0;
        show_text = !show_text;

        if (show_text) {
            VDP_drawText("PUSH (A) TO START GAME", 9, 16);
        } else {
            VDP_drawText("                      ", 9, 16);
        }
    }

    u16 joy_state = JOY_readJoypad(JOY_1);
    if (joy_state & BUTTON_A) {
        STATE_set(&State_InGame);
    }

    SPR_update(); 
    SYS_doVBlankProcess();
}

static void exit() {
    FADE_out(15, false);
    
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    SPR_reset();
}

const GameState State_Title = { enter, update, exit };