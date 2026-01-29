#include <genesis.h>
#include "debug.h"
#include "fade.h"
#include "globals.h"
#include "states/states.h"
#include "title.h"
#include "level.h"

static u16 blink_timer;
static bool show_text;

u16 last_joy_state = 0; // Wichtig für die Einzeltasten-Abfrage

static void enter() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    blink_timer = 0;
    show_text = TRUE;

    VDP_setTextPalette(PAL2);
    VDP_drawText("CELESTE DEMAKE",9 , 8);
    VDP_drawText("Darkjoy2k2  @ 2026", 7, 10);

    FADE_in(15, false);

}

static void update() {

    char str[4];
    uintToStr(current_level_index, str, 1);
    VDP_drawText("LEVEL SELECT:", 8, 16);
    VDP_drawText(str, 22, 16);



    blink_timer++;

    if (blink_timer >= 30) {
        blink_timer = 0;
        show_text = !show_text;

        if (show_text) {
            VDP_drawText("PUSH (A) TO START GAME", 5, 20);
        } else {
            VDP_drawText("                      ", 5, 20);
        }
    }



    u16 current_joy_state = JOY_readJoypad(JOY_1);

    if (current_joy_state & BUTTON_A) {
        STATE_set(&State_InGame);
    }

    if ((current_joy_state & BUTTON_UP) && !(last_joy_state & BUTTON_UP)) {
        if (current_level_index > 0) {
            current_level_index--;

        }
    }
    else if ((current_joy_state & BUTTON_DOWN) && !(last_joy_state & BUTTON_DOWN)) {
        if (current_level_index < 3) {
            current_level_index++;
        }
    }    

    last_joy_state = current_joy_state;   

    SPR_update(); 
    SYS_doVBlankProcess();
}

static void exit() {
    FADE_out(15, false);
    
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    SPR_reset();
    LIVES = MAX_LIVES;
}

const GameState State_Title = { enter, update, exit };