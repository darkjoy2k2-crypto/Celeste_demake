#include "tiles.h"
#include "fade.h"
#include "area.h"
#include "genesis.h"
#include "level.h"
#include "debug.h"
#include "checkCollisions.h"

bool is_tile_solid_only(u16 tile_index, Entity* entity, s16 world_x, s16 world_y, CollisionSide side) {
    
    u8 local_x = world_x & 7;
    u8 local_y = world_y & 7;

    // Wir casten auf Player, um auf is_dying und trampolin zuzugreifen
    Player* p = (Player*) entity;

    switch(tile_index) {
        case TILE_EMPTY:
            return false;

        case TILE_DEATH_B:
            if (side == SIDE_BOTTOM && local_y > 4) {
                p->is_dying = true;
            }
            return false;

        case TILE_DEATH_T:
            if (side == SIDE_TOP && local_y < 5) {
                p->is_dying = true;
            }
            return false;

        case TILE_DEATH_L:
            if (side == SIDE_RIGHT && local_x > 4) {
                p->is_dying = true;
            }
            return false;

        case TILE_DEATH_R:
            if (side == SIDE_LEFT && local_x < 5) {
                p->is_dying = true;
            }
            return false;

        case TILE_TRAMPOLINE:
            if (side == SIDE_BOTTOM && local_y >= 6) {
                p->trampolin = true;
            }
            return false;

        case TILE_ONEWAY:
            // vy liegt in der Basis-Struktur (p->ent.vy)
            if (side == SIDE_BOTTOM && p->ent.vy >= F16_0) {
                if (local_y <= 2) return true;
            }
            return false;

        default:
            return true;
    }
}