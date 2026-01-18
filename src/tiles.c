#include "tiles.h"
#include "fade.h"
#include "area.h"
#include "genesis.h"
#include "level.h"
#include "debug.h"
#include "checkTileCollisions.h"
#include "globals.h"

/* =============================================================================
   TILE KOLLISIONS-EIGENSCHAFTEN
   ============================================================================= */
bool is_tile_solid_only(u16 tile_index, Entity* entity, s16 world_x, s16 world_y, CollisionSide side) {
    
    u8 local_x = world_x & 7;
    u8 local_y = world_y & 7;

    /* Cast auf Player, um auf physics_state zuzugreifen */
    Player* p = (Player*) entity;

    switch(tile_index) {
        case TILE_EMPTY:
            return false;

        /* --- STACHELN / TODES-TILES --- */
        case TILE_DEATH_B:
            if (side == SIDE_BOTTOM && local_y > 1) {
                SET_P_FLAG(p->physics_state, P_FLAG_DYING);
            }
            return false;

        case TILE_DEATH_T:
            if (side == SIDE_TOP && local_y < 6) {
                SET_P_FLAG(p->physics_state, P_FLAG_DYING);
            }
            return false;

        case TILE_DEATH_L:
            if (side == SIDE_RIGHT && local_x > 1) {
                SET_P_FLAG(p->physics_state, P_FLAG_DYING);
            }
            return false;

        case TILE_DEATH_R:
            if (side == SIDE_LEFT && local_x < 6) {
                SET_P_FLAG(p->physics_state, P_FLAG_DYING);
            }
            return false;

        /* --- SPEZIAL-TILES --- */
        case TILE_TRAMPOLINE:
            if (side == SIDE_BOTTOM && local_y >= 6) {
                SET_P_FLAG(p->physics_state, P_FLAG_TRAMPOLINE);
            }
            return false;

        case TILE_ONEWAY:
            /* vy Zugriff über Basis-Entität */
            if (side == SIDE_BOTTOM && p->ent.vy >= F16_0) {
                if (local_y <= 2) return true;
            }
            return false;

        default:
            /* Alle nicht definierten Tiles (wie Stein/Wand) gelten als solid */
            return true;
    }
}