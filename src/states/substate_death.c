#include <genesis.h>
#include "area.h"
#include "fade.h"
#include "globals.h"
#include "entities/handle_entities.h"
#include "entities/sprites.h"
#include "entities/update_camera.h"
#include "states/states.h"

extern Map* level_1_map;

static Player* p;

static void enter() {
    p = (Player*) entities[player_id];

    // Initial "Hit" logic
    PAL_set_colors(PAL2, 1, COL_BALL_RED, 3);
    p->timer_death = 60;
    p->ent.vy = FIX16(-6);
    
    // Start fade immediately upon entering the death state
    FADE_out(30, true);
}

static void update() {
    // Physics and visual frame-steps
    p->ent.vy += GRAVITY_FALL;
    p->ent.y_f32 += F16_toFix32(p->ent.vy); 
    p->ent.y = F32_toInt(p->ent.y_f32);

    handle_all_animations();
    update_camera(entities[player_id], level_1_map, true);

    p->timer_death--;

    // When timer hits 0, the state manager will call exit()
    if (p->timer_death <= 0) {
        STATE_exitSubState();
    }
    SPR_update(); 
    SYS_doVBlankProcess();
}

static void exit() {
    const Area* death_area = p->current_area;

    /* Teleport player to Spawn */
    if (death_area != NULL) {
        p->ent.x = death_area->spawn.x << 3;
        p->ent.y = death_area->spawn.y << 3;
        p->ent.x_f32 = FIX32(p->ent.x);
        p->ent.y_f32 = FIX32(p->ent.y);
    } 

    p->ent.vx = F16_0;
    p->ent.vy = F16_0;
    p->state = P_FALLING;
    p->count_shot_jump = shot_jump_count; 
    p->timer_shot_jump = 0;
    LIVES--;
    p->physics_state = 0; 

    update_camera(entities[player_id], level_1_map, true);
                p->ent.spr_visible = true;
    // Fade back into the game after teleporting
    FADE_in(30, true);

    for (u16 i = 0; i < MAX_ENTITIES; i++) {
        if (entity_used[i] == 1) {

            Entity* e = entities[i];

            if (e->type == ENTITY_PICKUP ){

                Pickup* p = (Pickup*) entities[i];
                if ( (p->kind == PICKUP_HEART || PICKUP_HEART_FLEEING) && !p->counted){
                    p->collected = false;
                    p->ent.x = p->ent.x_old = p->origin_x;
                    p->ent.y = p->ent.y_old = p->origin_y;
                    p->ent.x_f32 = p->ent.x_old_f32 = FIX32(p->ent.x);
                    p->ent.y_f32 = p->ent.y_old_f32 = FIX32(p->ent.y);
                    p->state = PICK_IDLE;
                p->ent.spr_visible = true;
                } 
            }
            if (e->type == ENTITY_PLATFORM){
                Platform* p = (Platform*) entities[i];
                if (!CHECK_P_FLAG(p->flags, PLAT_FLAG_BREAKABLE)){
                    p->state = PLAT_IDLE;
                    p->enabled = true;
                    p->touched = false;
                    p->wait_timer = 0;     
                }

                p->ent.spr_visible = CHECK_P_FLAG(p->flags, PLAT_FLAG_INVISIBLE) ? false : true;
            }


        }
    }
}

const GameState SubState_Death = {enter, update, exit};