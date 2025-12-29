#include "tiles.h"
#include "fade.h"
#include "area.h"
#include "genesis.h"
#include "level.h"
#include "debug.h"
#include "checkCollisions.h"



bool is_tile_solid_only(u16 tile_index, Entity* entity, s16 world_x,s16 world_y, CollisionSide side) {
    
    u8 local_x = world_x & 7;
    u8 local_y = world_y & 7;

    switch(tile_index) {
        case TILE_EMPTY:
            return false;

        case TILE_DEATH_B:
            if (side == SIDE_BOTTOM && local_y > 4) {
                entity->is_dying = true;
            }
            return false;

        case TILE_DEATH_T:
            if (side == SIDE_TOP && local_y < 5) {
                entity->is_dying = true;
            }
            return false;

        case TILE_DEATH_L:
            if (side == SIDE_RIGHT && local_x > 4) {
                entity->is_dying = true;
            }
            return false;

        case TILE_DEATH_R:
            if (side == SIDE_LEFT && local_x < 5) {
                entity->is_dying = true;
            }
            return false;

        case TILE_TRAMPOLINE:
            if (side == SIDE_BOTTOM && local_y >= 6) {
                entity->trampolin = true;
            }            return false;

        case TILE_ONEWAY:
            if (side == SIDE_BOTTOM && entity->vy >= F16_0) {
                if (local_y <= 2) return true;
            }
            return false;

        default:
            return true;
    }
}