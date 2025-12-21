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

extern const Area level_1_areas[];
extern const u16 level_1_area_count;

Map* level_1_map;
bool show_level = true;
int i = 0;
int player_id = 0;
u16 ind = TILE_USER_INDEX;

void update_animation(Entity* e){
    int dx = e->x_old - e->x;
    e->anim_index += dx ; 

    if (e->anim_index < 0) e->anim_index += 50;
    if (e->anim_index > 49) e->anim_index -= 50;

    SPR_setAnimAndFrame(e->sprite, 0, e->anim_index / 6);
}

int main() {
    SPR_init(); 
    VDP_setScreenWidth320();
    
    VDP_setTextPlane(WINDOW); 
    VDP_setWindowHPos(0, 0);
    VDP_setWindowVPos(0, 34);

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

        camera_position.x = spawn_x - 160;
        camera_position.y = spawn_y - 112;
    }

if (show_level){
        VDP_loadTileSet(&our_tileset, ind, DMA);
        level_1_map = MAP_create(&our_level_map, BG_A, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind));
        ind += our_tileset.numTile;
        
        // 1. Position the map
        MAP_scrollTo(level_1_map, camera_position.x, camera_position.y);
        center_camera(&entities[player_id], level_1_map);
    }

    // 2. Position the sprites (Player) before the fade starts
    if (player_id != -1) {
        Entity* p = &entities[player_id];
        // Ensure sprite is placed relative to camera immediately
        SPR_setPosition(p->sprite, p->x - camera_position.x - 8, p->y - camera_position.y - 8);
    }

    // 3. Force one frame to "snap" everything into place while CRAM is still black
    SPR_update();
    SYS_doVBlankProcess();

    // 4. Now it is safe to fade in
    FADE_in(30);

    while(1) {
        debug_draw();
        handle_all_entities();

        if (player_id != -1) {
            Entity* p = &entities[player_id];
            
            update_animation(p);
            update_camera(p, level_1_map);

            p->x_old = p->x;
            p->y_old = p->y;

            char msg[32];
            int cid = get_current_area_id(p->x, p->y);
            sprintf(msg, "ID:%d PX:%d PY:%d  ", cid, p->x, p->y);
            VDP_drawText(msg, 1, 1);
        }

        SPR_update(); 
        SYS_doVBlankProcess();
    }

    return (0);
}