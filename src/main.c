#include <genesis.h>
#include "title.h" 
#include "level.h"
#include "entity_list.h"
#include "checkCollisions.h"
#include "physics.h"
#include "debug.h"
#include "camera.h"
#include "area.h"
#include "fade.h"
#include "sprites.h"

extern const Area level_1_areas[];
extern const u16 level_1_area_count;

Map* level_1_map;
bool show_level = true;
int i = 0;
int player_id = 0;
u16 ind = TILE_USER_INDEX;



int main() {
    SPR_init(); 
    VDP_setScreenWidth320();

    VDP_setTextPlane(WINDOW); 
    VDP_setWindowHPos(0, 0);
    VDP_setWindowVPos(0, 2); // 2 Zeilen = 16 Pixel
VDP_setTextPriority(1);
VDP_setTextPalette(PAL0);   
   
   
    // um ein massives Rechteck zu haben. 
    // Falls Tile 0 bei dir transparent ist,<n< füllen wir es hier mit Farbe:
    u32 tiles[8] = { 
        0x11111111, 0x11111111, 0x11111111, 0x11111111, 
        0x11111111, 0x11111111, 0x11111111, 0x11111111 
    };
    VDP_loadTileData(tiles, 1, 1, CPU); // Lädt ein massives Tile an Index 1

    u16 priority_attr = TILE_ATTR_FULL(PAL0, 1, 0, 0, 1); // Nutzt Tile Index 1
    VDP_fillTileMapRect(WINDOW, priority_attr, 0, 0, 40, 2);

FADE_init();

    VDP_drawImageEx(BG_B, &layer_bg, TILE_ATTR_FULL(PAL0, false, false, false, ind), 0, 5, false, true);
    ind += layer_bg.tileset->numTile;
    
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
        
    init_entities();
    init_camera();
    debug_init();
    
    load_areas(level_1_areas, level_1_area_count);

    const Area* start_area = get_area(0);
    if (start_area) {
        s16 spawn_x = start_area->spawn.x << 3; 
        s16 spawn_y = start_area->spawn.y << 3;

        player_id = create_entity(spawn_x, spawn_y, 13, 13, ENTITY_PLAYER);

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

    create_entity(16 + 8, 160 + 8, 16, 16, ENTITY_PLATFORM);
    create_entity(32 + 8, 160 + 8, 16, 16, ENTITY_PLATFORM);

    if (show_level){
        VDP_loadTileSet(&our_tileset, ind, DMA);
        level_1_map = MAP_create(&our_level_map, BG_A, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind));
        ind += our_tileset.numTile;
        
        MAP_scrollTo(level_1_map, camera_position.x, camera_position.y);
        
        if (player_id != -1) {
            center_camera(entities[player_id], level_1_map);
        }
    }

    if (player_id != -1) {
        Entity* e = entities[player_id];
        // Zugriff wieder ohne .ent
        SPR_setPosition(e->sprite, e->x - camera_position.x - 8, e->y - camera_position.y - 8);
    }

    SPR_update();
    SYS_doVBlankProcess();

    FADE_in(15);

    while(1) {
        handle_all_entities(); 

        if (player_id != -1) {
            Entity* e = entities[player_id];
            update_camera(e, level_1_map);
        }


        debug_draw();
        handle_all_animations();

        SPR_update(); 
        SYS_doVBlankProcess();
    }

    return (0);
}