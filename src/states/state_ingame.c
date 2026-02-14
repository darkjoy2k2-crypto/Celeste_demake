#include <genesis.h>
#include "debug.h"
#include "entities/handle_entities.h"
#include "entities/sprites.h"
#include "fade.h"
#include "globals.h"
#include "hud.h"
#include "background.h"
#include "states/states.h"
#include "levels.h"
#include "sprites.h"
#include "fonts.h"
#include "entities/update_camera.h"

/* --- Function Prototypes --- */
static void enter();
static void update();
static void exit();

/* --- State Definition --- */
const GameState State_InGame = { enter, update, exit };

/* --- Implementation --- */

static void enter() {
    VDP_loadFont(&TS_FONT_SOLID , DMA);
    init_entities(); 
    
    const LevelDefinition* lv = &levels[current_level_index];

    FADE_set_target(PAL0, pal_bg_city_2.data);
    FADE_set_target(PAL1, pal_layer_1.data);
    FADE_set_target(PAL2, pal_player_hud.data);
    FADE_set_target(PAL3, pal_entities.data);

    PAL_setColors(0, palette_black, 64, CPU);

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    hud_setup();
    init_camera();

    VDP_drawImageEx(BG_B, lv->background, TILE_ATTR_FULL(PAL0, false, false, false, ind), 0, 0, false, true);
    ind += lv->background->tileset->numTile;
    SYS_doVBlankProcess(); 

    state_ctx.ingame.active_areas = lv->areas;
    state_ctx.ingame.active_count = lv->area_count;
    load_areas(state_ctx.ingame.active_areas, state_ctx.ingame.active_count);

    VDP_loadTileSet(lv->tileset, ind, DMA);
    SYS_doVBlankProcess(); 

    state_ctx.ingame.current_map = MAP_create(lv->map_def, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));
    ind += lv->tileset->numTile;

    spawn_player(0);
    spawn_platforms(lv);
    spawn_pickups(lv);

    if (player_id != -1) {
        update_camera(entities[player_id], state_ctx.ingame.current_map, true);
    }

    SPR_update(); 
    SYS_doVBlankProcess();

    FADE_in(15, true); 
    
    JOY_init();
    debug_set_ram();
}

static void update() {
    /* 1. Update Entities (NPCs/Platforms first) */
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (entity_used[i] && i != player_id) {
            if (entities[i]->update) {
                entities[i]->update(entities[i]);
            }
        }
    }

    /* 2. Update Player */
    if (player_id != -1 && entity_used[player_id]) {
        entities[player_id]->update(entities[player_id]);
    }
    /* 3. Camera & Visuals */
    update_camera(entities[player_id], state_ctx.ingame.current_map, false);
    handle_all_animations();

    /* 4. Debug & UI */
    //debug_draw();

    /* 5. Engine Sync */
    SPR_update(); 
    SYS_doVBlankProcess();

}


static void exit() {
    FADE_out(15, false);      // Smoothly reveal the level
    clear_entities();
    hud_clear();
    /* 1. Cleanup Dynamic Memory */
    if (state_ctx.ingame.current_map) {
        MAP_release(state_ctx.ingame.current_map);
        state_ctx.ingame.current_map = NULL;
    }
}