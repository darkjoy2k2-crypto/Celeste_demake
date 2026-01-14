#include "physics.h"
#include "entity_list.h"
#include "checkTileCollisions.h"
#include "checkPlatformCollision.h"
#include "player.h"
#include "globals.h"
#include "area.h"
#include "debug.h"


void reset_physics(Player* player){
    player->is_on_ground = false; 
    player->is_on_wall   = false;
}

void update_moving_platform(Entity *plat, s16 start_pos, u16 speed, u16 amplitude)
{
    fix32 old_pos = plat->x_f32;
    fix32 offset_f32 = getSinusValueF32(vtimer, speed, amplitude);
    plat->x_f32 = FIX32(start_pos) + offset_f32;
    plat->vx = F32_toFix16(plat->x_f32 - old_pos);
    plat->x = F32_toInt(plat->x_f32);
}

move_platforms(){
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (entity_used[i] == 1 && entities[i]->type == ENTITY_PLATFORM) {
            update_moving_platform(entities[i], 156 + 16 * i, 16, 48);
        }
    }
}

void handle_all_entities()
{
    Entity* pEnt = entities[player_id];
    Player* player = (Player*) pEnt;

    reset_physics(player);
    move_platforms();
    handle_player_input(player);
    handle_platform_collision(pEnt); 
    check_tile_collision(pEnt);
    update_player_state_and_physics(pEnt);
    update_area(pEnt);
}

