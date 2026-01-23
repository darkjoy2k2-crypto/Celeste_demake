#include <genesis.h>
#include "debug.h"
#include "entities/handle_entities.h"
#include "entities/sprites.h"
#include "entities/update_camera.h"
#include "fade.h"
#include "globals.h"
#include "hud.h"
#include "level.h"
#include "title.h"
#include "states/states.h"

/* --- Function Prototypes --- */
static void enter();
static void update();
static void exit();

/* --- State Definition --- */
const GameState State_InGame = { enter, update, exit };

/* --- Implementation --- */

static void enter() {
    /* 1. System & Memory Reset */
    memset(&state_ctx, 0, sizeof(state_ctx));
    init_entities(); // Must happen before spawning anything
    
    const LevelDefinition* lv = &levels[current_level_index];

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    setup_hud();
    init_camera();

    /* 2. Load Background (Plane B) - HEAVY OPERATION */
    // Drawing an image can be heavy, we do this first while screen is black
    VDP_drawImageEx(BG_B, lv->background, TILE_ATTR_FULL(PAL0, false, false, false, ind), 0, 5, false, true);
    ind += lv->background->tileset->numTile;
    SYS_doVBlankProcess(); 


    /* 3. Prepare Logic Data */
    state_ctx.ingame.active_areas = lv->areas;
    state_ctx.ingame.active_count = lv->area_count;
    load_areas(state_ctx.ingame.active_areas, state_ctx.ingame.active_count);

    /* 4. Load Foreground Tileset (DMA) - HEAVY OPERATION */
    VDP_loadTileSet(lv->tileset, ind, DMA);
    
    // We wait one frame here to ensure the TileSet DMA is finished 
    // before we try to use those tiles for the Map.
    SYS_doVBlankProcess(); 

    /* 5. Initialize Map */
    state_ctx.ingame.current_map = MAP_create(lv->map_def, BG_A, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind));
    ind += lv->tileset->numTile;

    /* 6. Entity Spawning */
    // Spawning often involves SPR_addSprite, which uses more VRAM/bandwidth
    spawn_player(0);
    spawn_platforms(lv);

    /* 7. Final View Sync */
    if (player_id != -1) {
        update_camera(entities[player_id], state_ctx.ingame.current_map, true);
    }

    /* 8. Hardware Push & Start */
    SPR_update();           // Update sprites before fading in
    SYS_doVBlankProcess();  // Sync one last time

    FADE_in(15, true);      // Smoothly reveal the level
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
    debug_draw();

    /* 5. Engine Sync */
    SPR_update(); 
    SYS_doVBlankProcess();

    /* 6. State Transitions */
    if (LIVES < 1) {
        STATE_set(&State_GameOver);
    }
}

static void exit() {
    /* 1. Cleanup Dynamic Memory */
    if (state_ctx.ingame.current_map) {
        MAP_release(state_ctx.ingame.current_map);
        MEM_free(state_ctx.ingame.current_map);
        state_ctx.ingame.current_map = NULL;
    }

    /* 2. Hardware Cleanup */
    SPR_reset();
    VDP_resetSprites();
    
    ind = TILE_USER_INDEX; // Reset VRAM index for next load

    /* 3. Plane Cleanup */
    VDP_setTextPlane(BG_A); 
    VDP_setWindowOff();
    VDP_setHorizontalScroll(BG_A, 0);
    VDP_setVerticalScroll(BG_A, 0);
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    
    SPR_update();
}