#include "camera.h"

Vect2D_s16 camera_position = {0, 0};

static const s16 HUD_H = 16;
static const s16 SCREEN_W = 320;
static const s16 SCREEN_H = 224;
static const s16 CENTER_Y_OFFSET = 120; 

void init_camera() {
    camera_position.x = 0;
    camera_position.y = 0;
}

void update_camera(Entity* player, Map* level_map) {
    if (player == NULL) return;

    s16 px = player->x;
    s16 py = player->y;

    int area_id = get_current_area_id(px, py);
    if (area_id == -1) return;

    const Area* a = get_area(area_id);
    Vect2D_s16 target;

    target.x = px - (SCREEN_W / 2);
    target.y = py - CENTER_Y_OFFSET;

    s16 min_x = a->cam_min.x;
    s16 max_x = a->cam_max.x - SCREEN_W;
    s16 min_y = a->cam_min.y - HUD_H;
    s16 max_y = a->cam_max.y - SCREEN_H;

    if (max_x < min_x) max_x = min_x;
    if (max_y < min_y) max_y = min_y;

    if (target.x < min_x) target.x = min_x;
    if (target.x > max_x) target.x = max_x;
    if (target.y < min_y) target.y = min_y;
    if (target.y > max_y) target.y = max_y;

    s16 step_x = (target.x - camera_position.x);
    s16 step_y = (target.y - camera_position.y);

    if (step_x != 0) {
        s16 move_x = step_x >> 3; 
        if (move_x == 0) move_x = (step_x > 0) ? 1 : -1;
        camera_position.x += move_x;
    }
    
    if (step_y != 0) {
        s16 move_y = step_y >> 3;
        if (move_y == 0) move_y = (step_y > 0) ? 1 : -1;
        camera_position.y += move_y;
    }

    if (level_map != NULL) {
        MAP_scrollTo(level_map, camera_position.x, camera_position.y);
    }

    VDP_setHorizontalScroll(BG_B, -camera_position.x >> 2);
    VDP_setVerticalScroll(BG_B, 128); 

    SPR_setPosition(player->sprite, px - camera_position.x - 8, py - camera_position.y - 8);
}