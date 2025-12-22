#include "tiles.h"
#include "fade.h"
#include "area.h"
#include "genesis.h"

void handle_tile_action(u16 tile_index, Entity* entity, s16 world_y) {
    if (entity->is_dying) return;

    s16 local_y = world_y & 7;

    switch(tile_index) {
        case TILE_DEATH:
            if (entity->is_dying) return;
            entity->is_dying = true;

            s16 last_x = entity->x;
            s16 last_y = entity->y;

            int area_id = get_current_area_id(last_x, last_y);
            const Area* current = get_area(area_id != -1 ? area_id : 0);

            char msg[32];
            sprintf(msg, "Death ID:%d PX:%d PY:%d  ", area_id, last_x, last_y);
            VDP_drawText(msg, 1, 1);

            FADE_out(15);

            if (current != NULL) {
                entity->x = current->spawn.x * 8;
                entity->y = current->spawn.y * 8;
                entity->x_f32 = FIX32(entity->x);
                entity->y_f32 = FIX32(entity->y);
            }
            
            entity->vx = F16_0;
            entity->vy = F16_0;
            entity->state = P_FALLING;
            entity->trampolin = false;

            FADE_in(15);
            
            entity->is_dying = false;
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