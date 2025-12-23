#include "tiles.h"
#include "fade.h"
#include "area.h"
#include "genesis.h"

void handle_tile_action(u16 tile_index, Entity* entity, s16 world_y) {
    if (entity->is_dying) return;

    s16 local_y = world_y & 7;

    switch(tile_index) {
        case TILE_DEATH:
            entity->is_dying = true;
            break;

        case TILE_TRAMPOLINE:
            if (local_y >= 5) {
                entity->trampolin = true;
            }
            break;
    }
}

bool is_tile_solid_only(u16 tile_index, Entity* entity, s16 world_y, bool is_foot) {
    switch(tile_index) {
        case TILE_EMPTY:
        case TILE_DEATH:      // Tod ist für die Physik nicht solide
        case TILE_TRAMPOLINE: // Trampolin ist hier "Luft", Logik kommt über handle_tile_action
            return false;

        case TILE_ONEWAY:
            if (is_foot && entity->vy >= F16_0) {
                s16 local_y = world_y & 7;
                if (local_y <= 2) return true;
            }
            return false;

        default:
            return true; // Alles andere (Wände etc.) ist solide
    }
}