#include "physics.h"
#include "entity_list.h"
#include "checkTileCollisions.h"
#include "checkPlatformCollision.h"
#include "player.h"
#include "globals.h"
#include "area.h"
#include "debug.h"

/* =============================================================================
   PHYSIK-RESET (Pro Frame)
   ============================================================================= */
void reset_physics(Player* p){
    /* Neutralisiert Boden- und Wand-Flags vor der neuen Kollisionsprüfung */
    CLEAR_P_FLAG(p->physics_state, P_FLAG_ON_GROUND); 
    CLEAR_P_FLAG(p->physics_state, P_FLAG_ON_WALL);
}

/* =============================================================================
   PLATTFORM-BEWEGUNG
   ============================================================================= */
void update_moving_platform(Entity *plat, s16 start_pos, u16 speed, u16 amplitude)
{
    fix32 old_pos = plat->x_f32;
    fix32 offset_f32 = getSinusValueF32(vtimer, speed, amplitude);
    plat->x_f32 = FIX32(start_pos) + offset_f32;
    plat->vx = F32_toFix16(plat->x_f32 - old_pos);
    plat->x = F32_toInt(plat->x_f32);
}

void move_platforms(){
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (entity_used[i] == 1 && entities[i]->type == ENTITY_PLATFORM) {
            update_moving_platform(entities[i], 156 + 16 * i, 16, 48);
        }
    }
}

/* =============================================================================
   ZENTRALE ENTITY-VERARBEITUNG
   ============================================================================= */
void handle_player_entity()
{
    Entity* e = entities[player_id];
    Player* p = (Player*) e;

    /* Reihenfolge strikt beibehalten */
    reset_physics(p);
    move_platforms();

    handle_platform_collision(e); 
    check_tile_collision(e);

    handle_player_input(p);
    update_player_state_and_physics(e);
    update_area(e);
}