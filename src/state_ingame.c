#include <genesis.h>
#include "globals.h"
#include "area.h"
#include "debug.h"
#include "fade.h"
#include "states.h"
#include "sprites.h"
#include "level.h"
#include "entity_list.h"
#include "physics.h"
#include "title.h"
#include "camera.h"
#include "hud.h"
#include "player_controls.h"

extern const Area level_1_areas[];
extern const u16 level_1_area_count;

static void enter() {
    /* Wichtig: Alles auf Null setzen, um Müll im RAM zu vermeiden */
    memset(&state_ctx, 0, sizeof(state_ctx));
    
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    setup_hud();
    init_entities();
    init_camera();

    /* Hintergrund laden */
    VDP_drawImageEx(BG_B, &layer_bg, TILE_ATTR_FULL(PAL0, false, false, false, ind), 0, 5, false, true);
    ind += layer_bg.tileset->numTile;

    /* Areas registrieren */
    state_ctx.ingame.active_areas = level_1_areas;
    state_ctx.ingame.active_count = level_1_area_count;
    load_areas(state_ctx.ingame.active_areas, state_ctx.ingame.active_count);

    /* Player Erstellung basierend auf Area 0 */
    const Area* start_area = get_area(0);
    if (start_area) {
        s16 spawn_x = start_area->spawn.x << 3; 
        s16 spawn_y = start_area->spawn.y << 3;

        player_id = create_entity(spawn_x, spawn_y, 13, 13, F16_0, F16_0, ENTITY_PLAYER);

        if (player_id != -1) {
            Player* pl = (Player*) entities[player_id];
            pl->state = P_FALLING;
            pl->state_old = P_FALLING;
            
            /* Initialisierung der Bits */
            pl->physics_state = 0; 
            CLEAR_P_FLAG(pl->physics_state, P_FLAG_FACING_LEFT);
            
            pl->timer_stamina = 100;
            pl->timer_grace = 0;
            pl->timer_buffer = 0;
            pl->timer_shot_jump = 0;
            pl->count_shot_jump = 0;
            
            /* Area Pointer im Player setzen */
            pl->current_area = (Area*)start_area;
        }

        /* Kamera grob vorpositionieren */
        camera_position.x = spawn_x - 160;
        camera_position.y = spawn_y - 112;
    }

    /* Plattformen erzeugen */
    create_entity(16 + 8, 160 + 8, 16, 16, FIX16(0.1), FIX16(0), ENTITY_PLATFORM);
    create_entity(32 + 8, 160 + 8, 16, 16, FIX16(0.1), FIX16(0), ENTITY_PLATFORM);

    /* Level Map laden und in Union speichern */
    VDP_loadTileSet(&our_tileset, ind, DMA);
    state_ctx.ingame.current_map = MAP_create(&our_level_map, BG_A, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind));
    ind += our_tileset.numTile;
                
    if (player_id != -1) {
        /* Nutzt jetzt den Map-Pointer aus der Union */
        update_camera(entities[player_id], state_ctx.ingame.current_map, true);
    }

    SPR_update();
    SYS_doVBlankProcess();

    FADE_in(15);
    JOY_init();
    debug_set_ram();
}

static void update() {

    Entity* e = entities[player_id];    
    
    handle_player_entity(); 

    if (player_id != -1) {
        /* WICHTIG: Nutzt state_ctx.ingame.current_map */
        update_camera(e, state_ctx.ingame.current_map, false);
    }

    
    debug_draw();
    handle_all_animations();

    SPR_update(); 
    SYS_doVBlankProcess();
}

static void exit() {
    if (state_ctx.ingame.current_map) {
        MEM_free(state_ctx.ingame.current_map);
        state_ctx.ingame.current_map = NULL;
    }
    SPR_reset();
}

const GameState State_InGame = { enter, update, exit };