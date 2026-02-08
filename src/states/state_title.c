#include <genesis.h>
#include "debug.h"
#include "fade.h"
#include "globals.h"
#include "states/states.h"
#include "title.h"
#include "level.h"

// KEINE static Variablen mehr hier oben!

static void enter() {
    /* Den gesamten Union-Speicher für diesen State nullen */
    memset(&state_ctx, 0, sizeof(state_ctx));

    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    state_ctx.title.show_text = TRUE;

    VDP_setTextPalette(PAL2);
    VDP_drawText("CELESTE DEMAKE", 9, 8);
    VDP_drawText("Darkjoy2k2  @ 2026", 7, 10);

    FADE_in(15, false);
}

static void update() {

    TitleStateData *state_data = &state_ctx.ingame;
    
    // Level-Anzeige (char-Buffer ist lokal auf dem Stack, das ist okay)
    char str[4];
    uintToStr(current_level_index, str, 1);
    VDP_drawText("LEVEL SELECT:", 8, 16);
    VDP_drawText(str, 22, 16);

    /* Blink-Logik */
    state_ctx.title.blink_timer++;
    if (state_ctx.title.blink_timer >= 30) {
        state_ctx.title.blink_timer = 0;
        state_data->show_text ^= 1;

        if (state_data->show_text) {
            VDP_drawText("PUSH (A) TO START GAME", 5, 20);
        } else {
            VDP_drawText("                      ", 5, 20);
        }
    }

    /* Joypad & Idle Logic */
    u16 current_joy_state = JOY_readJoypad(JOY_1);

    // Wenn irgendeine Taste gedrückt wird (current_joy_state != 0), Reset. Sonst +1.
    state_data->idle_timer = current_joy_state ? 0 : state_data->idle_timer + 1;

    // Wechsel zu Controls nach ca. 6 Sekunden (360 Frames bei 60Hz)
    if (state_ctx.title.idle_timer >= 360) {
        STATE_set(&State_Controls);
        return; 
    }

    // Start Game
    if (current_joy_state & BUTTON_A) {
        STATE_set(&State_InGame);
        return;
    }

    // Level Auswahl (Single Press Logik mit last_joy_state)
    if ((current_joy_state & BUTTON_UP) && !(state_data->last_joy_state & BUTTON_UP)) {
        if (current_level_index > 0) current_level_index--;
    }
    else if ((current_joy_state & BUTTON_DOWN) && !(state_data->last_joy_state & BUTTON_DOWN)) {
        if (current_level_index < 4) current_level_index++;
    }

    /* Zustands-Sicherung für den nächsten Frame */
    state_data->last_joy_state = current_joy_state;

    SPR_update();
    SYS_doVBlankProcess();
}

static void exit() {
    FADE_out(15, false);
    
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    SPR_reset();
    LIVES = MAX_LIVES; 
    // ind = TILE_USER_INDEX; // Falls du VRAM-Reset hier auch brauchst
}

const GameState State_Title = { enter, update, exit };