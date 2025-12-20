#include "camera.h"

Vect2D_s16 camera_position = {0, 0};

static const s16 DEADZONE_X = 30;
static const s16 DEADZONE_Y = 20;

static const s16 HUD_H = 16;
static const s16 SCREEN_H = 224;
static const s16 CENTER_Y_OFFSET = 120; // HUD_H + ((SCREEN_H - HUD_H) / 2)

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

    if (a->cam_mode == CAM_MODE_FIXED) {
        target.x = a->cam.x;
        target.y = a->cam.y - HUD_H; 
    } else {
        target.x = px - 160;
        target.y = py - CENTER_Y_OFFSET;
    }

    if (target.x < a->min.x) target.x = a->min.x;
    if (target.x > (a->max.x - 320)) target.x = a->max.x - 320;

    s16 min_y_limit = a->min.y - HUD_H;
    if (target.y < min_y_limit) target.y = min_y_limit;

    s16 max_y_limit = a->max.y - SCREEN_H; 
    if (target.y > max_y_limit) target.y = max_y_limit;

    if (a->cam_mode != CAM_MODE_FIXED) {
        s16 diff_x = px - (camera_position.x + 160);
        s16 diff_y = py - (camera_position.y + CENTER_Y_OFFSET);

        if (abs(diff_x) < DEADZONE_X && abs(diff_y) < DEADZONE_Y) {
            SPR_setPosition(player->sprite, px - camera_position.x - 8, py - camera_position.y - 8);
            return; 
        }
    }

    s16 step_x = (target.x - camera_position.x);
    s16 step_y = (target.y - camera_position.y);

    if (step_x != 0) {
        s16 move_x = step_x >> 4;
        if (move_x == 0) move_x = (step_x > 0) ? 1 : -1;
        camera_position.x += move_x;
    }
    
    if (step_y != 0) {
        s16 move_y = step_y >> 4;
        if (move_y == 0) move_y = (step_y > 0) ? 1 : -1;
        camera_position.y += move_y;
    }

    if (camera_position.y < min_y_limit) camera_position.y = min_y_limit;
    if (camera_position.y > max_y_limit) camera_position.y = max_y_limit;

    if (level_map != NULL) {
        MAP_scrollTo(level_map, camera_position.x, camera_position.y);
    }

    VDP_setHorizontalScroll(BG_B, -camera_position.x >> 2);
    VDP_setVerticalScroll(BG_B, 128); 

    SPR_setPosition(player->sprite, px - camera_position.x - 8, py - camera_position.y - 8);
}