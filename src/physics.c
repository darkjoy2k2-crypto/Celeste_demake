#include "physics.h"
#include "entity_list.h"
#include "checkTileCollisions.h"
#include "checkPlatformCollision.h"
#include "player_update.h"
#include "globals.h"
#include "area.h" // Wichtig für update_area

void handle_all_entities()
{
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        if (entity_used[i] == 1 && entities[i]->type == ENTITY_PLATFORM)
        {
            entities[i]->vx = FIX16(0.2);
            entities[i]->x_f32 += F16_toFix32(entities[i]->vx);
            entities[i]->x = F32_toInt(entities[i]->x_f32);
        }
    }

    Entity* pEnt = entities[player_id];

    update_player_state_and_physics(pEnt);

    check_tile_collision(pEnt);

    handle_platform_collision(pEnt);

    update_area(pEnt);
}
