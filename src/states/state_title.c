#include <genesis.h>
#include "debug.h"
#include "fade.h"
#include "globals.h"
#include "states/states.h"
#include "background.h"
#include "level.h"
#include "sprites.h"
#include "fonts.h"

// Fangnetz für den H-Int Crash
static void hint_dummy() { }

static void applyTitleHScroll(TitleStateData *state_data) {
    u16 t = state_data->scroll_x_dark;
    s16 offset_top = F32_toRoundedInt(getSinusValueF32(t, 2, 60));
    for (int i = 22; i <= 47; i++) global_hscroll[i] = offset_top;
    for (int i = 50; i <= 93; i++) {
        global_hscroll[i] = F32_toRoundedInt(getSinusValueF32(t + (i * 2), 10, 2));
    }
    s16 offset_bottom = F32_toRoundedInt(getSinusValueF32(t, 2, 60));
    for (int i = 94; i <= 112; i++) global_hscroll[i] = -offset_bottom;

    VDP_setHorizontalScrollLine(BG_A, 0, global_hscroll, 224, DMA);
}

static void enter() {
    TitleStateData *state_data = &state_ctx.title;
    
    VDP_loadFont(&TS_FONT_CLEAR , DMA);
    state_data->level_select = false;
    current_level_index = 0;

    VDP_loadTileSet(&TS_MOUNTAIN, ind, DMA);
    state_data->current_map = MAP_create(&MAP_MOUNTAION, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));
    ind += TS_MOUNTAIN.numTile;
    
    state_data->scroll_x_demake = 0; 
    MAP_scrollTo(state_data->current_map, 0, 0);
    SYS_doVBlankProcess(); 

    // 3. LOGO (BG_A)
    VDP_drawImageEx(BG_A, &bg_title, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bg_title.tileset->numTile;
    SYS_doVBlankProcess();

    // 4. TEXT & UI SETUP
    VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);
    VDP_setTextPalette(PAL2); 
    VDP_setTextPriority(1); 

    FADE_set_target(PAL0, pal_bg_mountain.data);
    FADE_set_target(PAL1, pal_bg_title.data);
    FADE_set_target(PAL2, pal_player_hud.data);

    state_data->show_text = TRUE;
    state_data->idle_timer = 0;
    state_data->scroll_x_dark = 0;
    state_data->blink_timer = 0;

    VDP_drawText("Celeste Demake/Dark Mode for", 2, 22);
    VDP_drawText("Sega Genesis by Darkjoy2k2", 3, 23);
    VDP_drawText("Original by Extremely OK Games", 1, 24);
    VDP_drawText("Maddy Thorson, Noel Berry,", 3, 25);
    VDP_drawText("Lena Raine. Get it on Steam!", 2, 26);

    applyTitleHScroll(state_data);
    FADE_in(30, true);
}

static void update() {
    TitleStateData *state_data = &state_ctx.title;

    state_data->scroll_x_dark++; 
    applyTitleHScroll(state_data);

    // Berg-Scrolling
    if (state_data->scroll_x_demake < 544) {
        state_data->scroll_x_demake++;
        if (state_data->current_map) {
            MAP_scrollTo(state_data->current_map,0, state_data->scroll_x_demake);
        }
    }

    // Level-Anzeige (Aktualisierung alle 4 Frames reicht)
    if ((state_data->scroll_x_dark % 4) == 0 && state_data->level_select) {
        char str[4];
        uintToStr(current_level_index, str, 1);
        VDP_drawText("LEVEL SELECT:", 8, 18);
        VDP_drawText(str, 22, 18);
    }

    // Blinkender Start-Text
    state_data->blink_timer++;
    if (state_data->blink_timer >= 30) {
        state_data->blink_timer = 0;
        state_data->show_text ^= 1;
        VDP_drawText(state_data->show_text ? "PUSH (A) TO START GAME" : "                      ", 5, 16);
    }

    u16 joy = JOY_readJoypad(JOY_1);
    
    // Start Game
    if ((joy & BUTTON_A) && !(state_data->last_joy_state & BUTTON_A)) {
        LIVES = 0;
        HEARTS = 0;
        TIME = vtimer;
        STATE_set(&State_InGame);
        return;
    }

    // Level Auswahl
    if (state_data->level_select){
        if ((joy & BUTTON_UP) && !(state_data->last_joy_state & BUTTON_UP)) {
            if (current_level_index > 0) current_level_index--;
        }
        else if ((joy & BUTTON_DOWN) && !(state_data->last_joy_state & BUTTON_DOWN)) {
            if (current_level_index < MAX_LEVEL) current_level_index++;
        }
    }

    if (joy & BUTTON_B)
        if (joy & BUTTON_C)
            if (joy & BUTTON_START)
                state_data->level_select = true;

    state_data->last_joy_state = joy;
    
    // Idle-Timer für Controls-Screen
    state_data->idle_timer = joy ? 0 : state_data->idle_timer + 1;
    if (state_data->idle_timer >= 720) {
        STATE_set(&State_Controls);
        return; 
    }

    SPR_update();
    SYS_doVBlankProcess();
}

static void exit() {
    // 1. Interrupts stoppen, BEVOR das Fading fertig ist
    VDP_setHInterrupt(FALSE);
    SYS_setHIntCallback(&hint_dummy); 

    FADE_out(15, false);

    // 3. Map-Engine Cleanup
    if (state_ctx.title.current_map) {
        MAP_release(state_ctx.title.current_map);
        state_ctx.title.current_map = NULL;
    }

}
const GameState State_Title = { enter, update, exit };