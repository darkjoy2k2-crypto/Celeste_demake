#include <genesis.h>
#include "globals.h"
#include "area.h"
#include "states.h"
#include "level.h"
#include "entity_list.h"
#include "title.h"
#include "camera.h"
#include "hud.h"
#include "player_controls.h"

extern const Area level_1_areas[];
extern const u16 level_1_area_count;



static void enter() {
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    setup_hud();
    init_entities();
    init_camera();

    VDP_drawImageEx(BG_B, &layer_bg, TILE_ATTR_FULL(PAL0, false, false, false, ind), 0, 5, false, true);
    ind += layer_bg.tileset->numTile;

    load_areas(level_1_areas, level_1_area_count);

    const Area* start_area = get_area(0);
    if (start_area) {
        s16 spawn_x = start_area->spawn.x << 3; 
        s16 spawn_y = start_area->spawn.y << 3;

        player_id = create_entity(spawn_x, spawn_y, 13, 13, F16_0, F16_0, ENTITY_PLAYER);

        if (player_id != -1) {
            Player* pl = (Player*) entities[player_id];
            pl->state = P_FALLING;
            pl->state_old = P_FALLING;
            pl->timer_stamina = 100;
            pl->facing = 0;
            pl->is_dying = false;
            pl->timer_grace = 0;
            pl->timer_buffer = 0;
            pl->timer_shot_jump = 0;
        }

        camera_position.x = spawn_x - 160;
        camera_position.y = spawn_y - 112;
    }

    create_entity(16 + 8, 160 + 8, 16, 16, FIX16(0.1), FIX16(0),ENTITY_PLATFORM);
    create_entity(32 + 8, 160 + 8, 16, 16, FIX16(0.1), FIX16(0), ENTITY_PLATFORM);

        VDP_loadTileSet(&our_tileset, ind, DMA);
        level_1_map = MAP_create(&our_level_map, BG_A, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind));
        ind += our_tileset.numTile;
                
        if (player_id != -1) {
            update_camera(entities[player_id], level_1_map, true);
        }

    if (player_id != -1) {
        Entity* e = entities[player_id];
        // Zugriff wieder ohne .ent
        SPR_setPosition(e->sprite, e->x - camera_position.x - 8, e->y - camera_position.y - 8);
    }

    SPR_update();
    SYS_doVBlankProcess();

    FADE_in(15);
    JOY_init();
}



static void update() {
        handle_all_entities(); 

        if (player_id != -1) {
            Entity* e = entities[player_id];
            update_camera(e, level_1_map, false);
        }


        debug_draw();
        handle_all_animations();

        SPR_update(); 
        SYS_doVBlankProcess();
}

static void exit() {
    SPR_reset(); // Alles aufräumen für den nächsten State
    //MEM_free(); // Optional: Falls du dynamischen Speicher nutzt
}

const GameState State_InGame = { enter, update, exit };