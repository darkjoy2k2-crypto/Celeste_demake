#include "physics.h"
#include "entity_list.h"
#include "checkTileCollisions.h"
#include "checkPlatformCollision.h"
#include "player_update.h"
#include "globals.h"
#include "area.h"
#include "debug.h"

#define VELOCITY_DEADZONE 20 

void apply_velocity_to_position(Player* player)
{


    // Nach der Reibungs-Berechnung:
    if (F16_abs(player->ent.vx) < VELOCITY_DEADZONE) {
        player->ent.vx = 0;
    }

    // Falls der Ball auch vertikal "zittert" (z.B. auf Schrägen):
    if (F16_abs(player->ent.vy) < VELOCITY_DEADZONE && player->state == P_GROUNDED) {
        player->ent.vy = 0;
    }    

    player->ent.x_f32 += F16_toFix32(player->ent.vx + player->solid_vx);
    player->ent.y_f32 += F16_toFix32(player->ent.vy + player->solid_vy);
    player->ent.x = F32_toInt(player->ent.x_f32);
    player->ent.y = F32_toInt(player->ent.y_f32);
}

void handle_all_entities()
{
    Entity* pEnt = entities[player_id];
    Player* player = (Player*) pEnt;

    // 1. Plattformen bewegen
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        if (entity_used[i] == 1 && entities[i]->type == ENTITY_PLATFORM)
        {

            update_moving_platform(entities[i], 156 + 16 * i, 16, 48);
            /*
            entities[i]->x_f32 += F16_toFix32(entities[i]->vx);
            entities[i]->x = F32_toInt(entities[i]->x_f32);            
            entities[i]->y_f32 += F16_toFix32(entities[i]->vy);
            entities[i]->y = F32_toInt(entities[i]->y_f32);*/
        }
    }

    handle_platform_collision(pEnt);

    update_player_state_and_physics(pEnt);

    fix32 val = player->ent.x_f32;
    int pixel = F32_toInt(val);
    int sub = F32_toInt(F32_mul(val - FIX32(pixel), FIX32(100)));
    debug_set(0, pixel);
    debug_set(1, sub);


    // 4. Position aus Velocity berechnen
    apply_velocity_to_position(player);

    val = player->ent.x_f32;
    pixel = F32_toInt(val);
    sub = F32_toInt(F32_mul(val - FIX32(pixel), FIX32(100)));
    debug_set(2, pixel);
    debug_set(3, sub);

    // 5. Tile-Kollision (korrigiert Position bei Kollision)
    check_tile_collision(pEnt);

    // 6. Area update
    update_area(pEnt);
}



void update_moving_platform(Entity *plat, s16 start_pos, u16 speed, u16 amplitude)
{
    fix32 old_pos = plat->x_f32;

    // Wir holen uns den hochpräzisen fix32 Offset
    fix32 offset_f32 = getSinusValueF32(vtimer, speed, amplitude);
    
    // Neue Subpixel-Position setzen
    plat->x_f32 = FIX32(start_pos) + offset_f32;

    // Delta berechnen (wichtig für die Mitnahme des Balls)
    plat->vx = F32_toFix16(plat->x_f32 - old_pos);

    // Ganzzahl für Logik und Sprite-Hardware (ohne Rundung für Konsistenz)
    plat->x = F32_toInt(plat->x_f32);
}