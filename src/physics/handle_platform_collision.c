#include <genesis.h>
#include "physics/handle_tile_collision.h"
#include "physics/tile_properties.h"
#include "entities/handle_entities.h" 
#include "level.h"
#include "debug.h"
#include "globals.h"

/* =============================================================================
   EINZEL-KOLLISIONSPRÜFUNG (funktionsgleich zum ALT-Code)
   ============================================================================= */
static inline void check_platform_collision(Player *p, Entity *plat)
{
    Entity *e = &p->ent;

    /* Halbe Breiten/Höhen */
    u8 h_pw = e->width >> 1;
    u8 h_ph = e->height >> 1;
    u8 h_ew = plat->width >> 1;
    u8 h_eh = plat->height >> 1;

    /* Integer-Positionen */
    s16 p_x  = F32_toInt(e->x_f32);
    s16 p_y  = F32_toInt(e->y_f32);
    s16 pl_x = F32_toInt(plat->x_f32);
    s16 pl_y = F32_toInt(plat->y_f32);

    /* Distanzen */
    s16 dx = p_x - pl_x;
    s16 dy = p_y - pl_y;
    s16 abs_dx = (dx < 0) ? -dx : dx;
    s16 abs_dy = (dy < 0) ? -dy : dy;

    bool pos_changed_x = false;
    bool pos_changed_y = false;

    /* --- X-SWEEP --- */
    if (p->state != P_GROUNDED && abs_dy < (h_ph + h_eh - 1))
    {
        s16 gapX = abs_dx - (h_pw + h_ew);

        if (gapX <= 1)
        {
            SET_P_FLAG(p->physics_state, P_FLAG_ON_WALL);

            if (gapX < 0)
            {
                if (dx > 0) p_x = pl_x + h_ew + h_pw;
                else        p_x = pl_x - h_ew - h_pw;
                pos_changed_x = true;
            }

            p->solid_vy = plat->vy;
            p->solid_vx = plat->vx;
        }
    }

    /* --- Y-SWEEP --- */
    if (abs_dx < (h_pw + h_ew - 2))
    {
        s16 fall_margin = (e->vy > F16_0) ? F16_toInt(e->vy) + 3 : 4;

        if (abs_dy < (h_ph + h_eh + fall_margin))
        {
            if (dy > 0)
            {
                if (e->vy < F16_0 && dy < (h_ph + h_eh))
                {
                    p_y = pl_y + h_eh + h_ph;
                    e->vy = 0;
                    pos_changed_y = true;
                }
            }
            else
            {
                s16 target_y = pl_y - h_eh - h_ph;

                if (p->state == P_GROUNDED)
                {
                    if (abs16(p_y - target_y) > 0)
                    {
                        p_y = target_y;
                        pos_changed_y = true;
                    }
                }
                else if (e->vy >= F16_0)
                {
                    p_y = target_y;
                    pos_changed_y = true;
                    p->state = P_GROUNDED;
                }

                if (p->state == P_GROUNDED)
                {
                    SET_P_FLAG(p->physics_state, P_FLAG_ON_GROUND);
                    e->vy = F16_0;
                    p->solid_vx = plat->vx;
                    p->solid_vy = plat->vy;
                }
            }
        }
    }

    /* --- FINALE ÜBERNAHME (identisch zum ALT-Verhalten) --- */
    if (pos_changed_x)
    {
        e->x = p_x;
        e->x_f32 = FIX32(p_x);
    }
    else
    {
        e->x = F32_toInt(e->x_f32);
    }

    if (pos_changed_y)
    {
        e->y = p_y;
        e->y_f32 = FIX32(p_y);
    }
    else
    {
        e->y = F32_toInt(e->y_f32);
    }
}

/* =============================================================================
   PLATTFORM-HANDLER
   ============================================================================= */
void handle_platform_collision(Entity *entity)
{
    if (entity->type != ENTITY_PLAYER) return;

    Player *p = (Player*)entity;

    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        if (entities[i] != NULL && entities[i]->type == ENTITY_PLATFORM)
        {
            check_platform_collision(p, entities[i]);
        }
    }
}
