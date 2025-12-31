#include "camera.h"
#include "entity_list.h"

Vect2D_s16 camera_position = {0, 0};

static const s16 HUD_H = 16;
static const s16 SCREEN_W = 320;
static const s16 SCREEN_H = 224;
static const s16 CENTER_Y_OFFSET = 120; 

void init_camera() {
    camera_position.x = 0;
    camera_position.y = 0;
}

void center_camera(Entity* player, Map* level_map) {
    if (player == NULL) return;

    Player* p = (Player*) player;
    s16 px = p->ent.x;
    s16 py = p->ent.y;

    int area_id = get_current_area_id(px, py);
    if (area_id == -1) return;

    const Area* a = get_area(area_id);
    
    s16 target_x = px - (SCREEN_W / 2);
    s16 target_y = py - CENTER_Y_OFFSET;

    s16 min_x = a->cam_min.x;
    s16 max_x = a->cam_max.x - SCREEN_W;
    s16 min_y = a->cam_min.y - HUD_H;
    s16 max_y = a->cam_max.y - SCREEN_H;

    if (max_x < min_x) max_x = min_x;
    if (max_y < min_y) max_y = min_y;

    if (target_x < min_x) target_x = min_x;
    else if (target_x > max_x) target_x = max_x;

    if (target_y < min_y) target_y = min_y;
    else if (target_y > max_y) target_y = max_y;

    camera_position.x = target_x;
    camera_position.y = target_y;

    if (level_map != NULL) {
        MAP_scrollTo(level_map, camera_position.x, camera_position.y);
    }

    VDP_setHorizontalScroll(BG_B, -camera_position.x >> 2);
    VDP_setVerticalScroll(BG_B, 128); 

    SPR_setPosition(p->ent.sprite, px - camera_position.x - 8, py - camera_position.y - 8);
}




void update_all_entities_sprites() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (entity_used[i] == 1 && entities[i] != NULL) {
            Entity* e = entities[i];
            if (e->sprite != NULL) {
                s16 x = e->x - camera_position.x - 8;
                s16 y = e->y - camera_position.y - 8;

                if (x < -16 || x > SCREEN_W || y < -16 || y > SCREEN_H) {
                    SPR_setVisibility(e->sprite, HIDDEN);;
                }
                else {
                    SPR_setVisibility(e->sprite, VISIBLE);
                    SPR_setPosition(e->sprite, x, y);
                }
            }
        }
    }
}




void update_camera(Entity* player, Map* level_map) {
    if (player == NULL) return;

    Player* p = (Player*) player;
    const Area* a = p->current_area;
    
    if (a == NULL) return;

    s16 px = p->ent.x;
    s16 py = p->ent.y;
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

    // SPR_setPosition(p->ent.sprite, px - camera_position.x - 8, py - camera_position.y - 8);

    update_all_entities_sprites();

}