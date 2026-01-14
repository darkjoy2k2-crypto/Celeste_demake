#include "player.h"
#include "entity_list.h"
#include "genesis.h"
#include "globals.h"
#include "debug.h"
#include "level.h"
#include "area.h"
#include "fade.h"

void kill_player(Player* player) {
    player->timer_death = 60;

    player->ent.vy = FIX16(-6);

    do {
        player->ent.vy += GRAVITY_FALL;
        player->ent.y_f32 += F16_toFix32(player->ent.vy); 
        player->ent.y = F32_toInt(player->ent.y_f32);

        debug_draw();
        handle_all_animations();
        update_camera(entities[player_id], level_1_map, true);
        SPR_update(); 
        SYS_doVBlankProcess();    player->timer_death--;

        if (player->timer_death == 45) FADE_out(30);


    } while ( player->timer_death > 0);

    player->ent.x = player->current_area->spawn.x << 3;
    player->ent.y = player->current_area->spawn.y << 3;
    player->ent.x_f32 = FIX32(player->ent.x);
    player->ent.y_f32 = FIX32(player->ent.y);
    player->ent.vx = F16_0;
    player->ent.vy = F16_0;
    player->state = P_FALLING;
    player->trampolin = false;
    FADE_in(30);
    player->is_dying = false;
}

void update_player_state_and_physics(Entity* entity) {
    Player* player = (Player*) entity;
    const Area* a = player->current_area;

    if (a != NULL) 
        if (entity->y > a->max.y) 
            player->is_dying = true;

    if (player->is_dying) kill_player(player);

    entity->x_old_f32 = entity->x_f32;
    entity->y_old_f32 = entity->y_f32;
    entity->x_old = F32_toInt(entity->x_old_f32);
    entity->y_old = F32_toInt(entity->y_old_f32);

    if (player->timer_grace > 0) player->timer_grace--;
    if (player->timer_buffer > 0) player->timer_buffer--;

    switch(player->state)
    {
        case P_ON_WALL:
            // Hier keine Schwerkraft auf vy anwenden
            break;

        case P_GROUNDED:
            player->timer_grace = TIMER_GRACE_MAX;
            entity->vy += GRAVITY_GROUNDED;
            entity->vx = F16_mul(entity->vx, FRICTION);
            if (abs16(entity->vx) < FIX16(0.2)) entity->vx = F16_0;
            break;

        case P_JUMPING:
            player->timer_grace = 0;
            entity->vy += GRAVITY_JUMP;
            entity->vx = F16_mul(entity->vx, FRICTION_AIR);
            if (entity->vy > 0) player->state = P_FALLING;
            break;

        case P_FALLING:
            entity->vy += GRAVITY_FALL;
            entity->vx = F16_mul(entity->vx, FRICTION_AIR);
            break;

        default: break;
    }

    entity->x_f32 += F16_toFix32(entity->vx);
    entity->y_f32 += F16_toFix32(entity->vy);
    entity->x_f32 += F16_toFix32(player->solid_vx);
    entity->y_f32 += F16_toFix32(player->solid_vy);

    entity->x = F32_toInt(entity->x_f32);
    entity->y = F32_toInt(entity->y_f32);

    player->state_old = player->state;
    player->is_on_wall = false; // Reset für Kollisions-Check im nächsten Frame
}