#include "player.h"
#include "genesis.h"
#include "globals.h"
#include "debug.h"
#include "level.h"
#include "area.h"
#include "fade.h"

static void update_player_stamina_visuals(Player* p) {
    if (p->timer_stamina <= 0) {
        PAL_set_colors(PAL1, 1, COL_BALL_ORANGE, 3);
        return;
    }

    if (p->timer_stamina < 200) {
        u16 interval = (p->timer_stamina < 100) ? 14 : 30;
        
        if ((vtimer % interval) < (interval / 2)) {
            PAL_set_colors(PAL1, 1, COL_BALL_ORANGE, 3);
        } else {
            PAL_restore_direct(PAL1, 1, 3);
        }
    }
}

static void apply_ground_physics(Player* p) {
    p->timer_grace = TIMER_GRACE_MAX;
    if (p->count_shot_jump != 2 || p->timer_stamina < 300) {
        p->count_shot_jump = 2;
        p->timer_stamina = 300;
        PAL_restore_direct(PAL1, 1, 3);
    } 

    SET_P_FLAG(p->physics_state, P_FLAG_ON_GROUND);

    p->ent.vy += GRAVITY_GROUNDED;
    p->ent.vx = F16_mul(p->ent.vx, FRICTION);
    if (abs16(p->ent.vx) < FIX16(0.2)) p->ent.vx = F16_0;
}

static void apply_air_physics(Player* p) {
    CLEAR_P_FLAG(p->physics_state, P_FLAG_ON_GROUND);
    p->ent.vy += (p->state == P_JUMPING) ? GRAVITY_JUMP : GRAVITY_FALL;
    p->ent.vx = F16_mul(p->ent.vx, FRICTION_AIR);
    if (p->ent.vy > 0 && p->state == P_JUMPING) p->state = P_FALLING;
}

static void apply_wall_physics(Player* p) {
    update_player_stamina_visuals(p);
    p->count_shot_jump = 2;
    p->ent.vx = F16_0; 
}

void kill_player(Player* p) {
    PAL_set_colors(PAL1, 1, COL_BALL_RED, 3);

    p->timer_death = 60;
    p->ent.vy = FIX16(-6);
    do {
        p->ent.vy += GRAVITY_FALL;
        p->ent.y_f32 += F16_toFix32(p->ent.vy); 
        p->ent.y = F32_toInt(p->ent.y_f32);
        
        debug_draw();
        handle_all_animations();
        update_camera(entities[player_id], level_1_map, true);
        SPR_update(); 
        SYS_doVBlankProcess();    
        
        p->timer_death--;
        if (p->timer_death == 45) FADE_out(30);
    } while (p->timer_death > 0);

    p->ent.x = p->current_area->spawn.x << 3;
    p->ent.y = p->current_area->spawn.y << 3;
    p->ent.x_f32 = FIX32(p->ent.x);
    p->ent.y_f32 = FIX32(p->ent.y);
    p->ent.vx = F16_0;
    p->ent.vy = F16_0;
    p->state = P_FALLING;
    p->count_shot_jump = 2; 
    p->timer_shot_jump = 0;
    
    p->physics_state = 0;
    
    FADE_in(30);
}

void update_player_state_and_physics(Entity* e) {
    Player* p = (Player*) e;
    const Area* a = p->current_area;

    if (a != NULL && e->y > a->max.y) {
        SET_P_FLAG(p->physics_state, P_FLAG_DYING);
    }

    if (CHECK_P_FLAG(p->physics_state, P_FLAG_DYING)) {
        kill_player(p); 
        return; 
    }

    e->x_old_f32 = e->x_f32; e->y_old_f32 = e->y_f32;
    e->x_old = F32_toInt(e->x_old_f32); e->y_old = F32_toInt(e->y_old_f32);

    if (p->timer_grace > 0) p->timer_grace--;
    if (p->timer_buffer > 0) p->timer_buffer--;

    switch(p->state) {
        case P_GROUNDED:
        case P_IDLE:
        case P_RUNNING:   apply_ground_physics(p); break;
        case P_ON_WALL:    apply_wall_physics(p);   break;
        case P_JUMPING:
        case P_FALLING:   apply_air_physics(p);    break;
        case P_SHOT_JUMP: break;
        default: break; 
    }

    if (e->vy > FIX16(5)) e->vy = FIX16(5);

    e->x_f32 += F16_toFix32(e->vx + p->solid_vx);
    e->y_f32 += F16_toFix32(e->vy + p->solid_vy);
    e->x = F32_toInt(e->x_f32); e->y = F32_toInt(e->y_f32);

    p->state_old = p->state;
    
    CLEAR_P_FLAG(p->physics_state, P_FLAG_ON_WALL);
    p->solid_vx = 0; 
    p->solid_vy = 0;
}