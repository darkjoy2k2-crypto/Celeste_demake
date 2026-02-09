#include <genesis.h>
#include "debug.h"
#include "fade.h"
#include "globals.h"
#include "states/states.h"
#include "title.h"
#include "level.h"

// Der ausgelagerte Effekt
static void applyTitleHScroll(TitleStateData *state_data) {
    u16 t = state_data->scroll_x_dark;

    // 1. Der obere Text (Block-Bewegung)
    s16 offset_top = F32_toRoundedInt(getSinusValueF32(t, 2, 60));
    for (int i = 22; i <= 47; i++) {
        state_data->hscroll_vals[i] = offset_top;
    }

    // 2. Der Bereich DAZWISCHEN (Wobble-Effekt)
    for (int i = 50; i <= 93; i++) {
        state_data->hscroll_vals[i] = 
        F32_toRoundedInt(getSinusValueF32(t + (i * 2), 10, 2));
    }

    // 3. Der untere Text (Block-Bewegung, gegengleich)
    s16 offset_bottom = F32_toRoundedInt(getSinusValueF32(t, 2, 60));
    for (int i = 94; i <= 112; i++) {
        state_data->hscroll_vals[i] = -offset_bottom;
    }

    VDP_setHorizontalScrollLine(BG_B, 0, state_data->hscroll_vals, 224, DMA);
}

static void enter() {
    TitleStateData *state_data = &state_ctx.ingame;

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    VDP_setHorizontalScroll(BG_B, 0);
    VDP_setVerticalScroll(BG_B, 0);

    memset(state_data->hscroll_vals, 0, sizeof(state_data->hscroll_vals));

    VDP_setHorizontalScrollLine(BG_B, 0, state_data->hscroll_vals, 224, DMA);    
    
    FADE_set_target(PAL0, pal_title.data);
    PAL_setColors(0, palette_black, 64, CPU);

    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);
   
    VDP_drawImageEx(BG_B, &bg_title, TILE_ATTR_FULL(PAL0, false, false, false, ind), 0, 0, false, true);
    
    VDP_setTextPalette(PAL2); 
    VDP_setTextPriority(1); 

    state_data->show_text = TRUE;
    state_data->idle_timer = 0;
    state_data->scroll_x_dark = 0; // Initialisierung

    VDP_drawText("Celeste Demake/Dark Mode for", 2, 22);
    VDP_drawText("Sega Genesis by Darkjoy2k2", 3, 23);
    VDP_drawText("Original by Extremely OK Games", 1, 24);
    VDP_drawText("Maddy Thorson, Noel Berry,", 3, 25);
    VDP_drawText("Lena Raine. Get it on Steam!", 2, 26);

    // Initialen Frame berechnen, damit beim Fade-In alles an seinem Platz ist
    memset(state_data->hscroll_vals, 0, sizeof(state_data->hscroll_vals));
    applyTitleHScroll(state_data);

    FADE_in(30, true);
}

static void update() {
    TitleStateData *state_data = &state_ctx.ingame;

    // Timer erhöhen und Effekt anwenden
    state_data->scroll_x_dark++; 
    applyTitleHScroll(state_data);

    char str[4];
    uintToStr(current_level_index, str, 1);
    VDP_drawText("LEVEL SELECT:", 8, 18);
    VDP_drawText(str, 22, 18);

    state_data->blink_timer++;
    if (state_data->blink_timer >= 30) {
        state_data->blink_timer = 0;
        state_data->show_text ^= 1;
        VDP_drawText(state_data->show_text ? "PUSH (A) TO START GAME" : "                      ", 5, 16);
    }

    u16 current_joy_state = JOY_readJoypad(JOY_1);
    state_data->idle_timer = current_joy_state ? 0 : state_data->idle_timer + 1;

    if (state_data->idle_timer >= 720) {
        STATE_set(&State_Controls);
        return; 
    }

    if (current_joy_state & BUTTON_A) {
        STATE_set(&State_InGame);
        return;
    }

    if ((current_joy_state & BUTTON_UP) && !(state_data->last_joy_state & BUTTON_UP)) {
        if (current_level_index > 0) current_level_index--;
    }
    else if ((current_joy_state & BUTTON_DOWN) && !(state_data->last_joy_state & BUTTON_DOWN)) {
        if (current_level_index < 4) current_level_index++;
    }

    state_data->last_joy_state = current_joy_state;

    SPR_update();
    SYS_doVBlankProcess();
}

static void exit() {
    FADE_out(15, false);
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    SPR_reset();
    LIVES = MAX_LIVES; 
}

const GameState State_Title = { enter, update, exit };