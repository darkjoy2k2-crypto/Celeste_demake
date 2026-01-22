#include <genesis.h>
#include "globals.h"
#include "area.h"
#include "debug.h"
#include "fade.h"
#include "states/states.h"
#include "entities/sprites.h"
#include "level.h"
#include "entities/handle_entities.h"
#include "title.h"
#include "entities/update_camera.h"
#include "hud.h"
#include "entities/player/player_controls.h"

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
    create_platform(22, 22, FIX16(10) ,PB_SINUS);
    create_platform(30, 22, FIX16(0.5)  ,PB_LINEAR);

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

    FADE_in(15, true);
    JOY_init();
    debug_set_ram();

}

static void update() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        // Jetzt ist entity_used hier bekannt
        if (entity_used[i] && i != player_id) {
            if (entities[i]->update) {
                entities[i]->update(entities[i]);
            }
        }
    }

    // Player separat (wie gehabt)
    if (player_id != -1 && entity_used[player_id]) {
        entities[player_id]->update(entities[player_id]);
    }

    update_camera(entities[player_id], state_ctx.ingame.current_map, false);
    
    debug_draw();
    handle_all_animations();

    SPR_update(); 
    SYS_doVBlankProcess();

    if (LIVES < 1){
        STATE_set(&State_GameOver);
    }
}

static void exit() {
    /* 1. VRAM-Management der Map freigeben (WICHTIG!) */
    if (state_ctx.ingame.current_map) {
        MAP_release(state_ctx.ingame.current_map);
        MEM_free(state_ctx.ingame.current_map);
        state_ctx.ingame.current_map = NULL;
    }

    /* 2. Sprites entladen */
    SPR_reset();

    /* 3. VRAM-Index zurücksetzen */
    /* Wir setzen 'ind' zurück auf den Punkt NACH dem Font */
    /* In deiner main.c war das TILE_USER_INDEX (meist 16 oder höher) */
    ind = TILE_USER_INDEX;

    /* 4. Planes sauber machen, damit beim nächsten Start keine Reste da sind */
    VDP_setTextPlane(BG_A); 
    VDP_setWindowOff();
    
    VDP_setHorizontalScroll(BG_A, 0);
    VDP_setVerticalScroll(BG_A, 0);
    
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    SPR_reset();
    VDP_resetSprites();
SPR_update();


}

const GameState State_InGame = { enter, update, exit };