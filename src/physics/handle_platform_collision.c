#include <genesis.h>
#include "physics/handle_tile_collision.h"
#include "physics/tile_properties.h"
#include "entities/handle_entities.h" 
#include "level.h"
#include "debug.h"
#include "globals.h"

/* =============================================================================
   EINZEL-KOLLISIONSPRÜFUNG
   Arbeitet jetzt mit Player* und Platform* und nutzt die echten Mittelpunkte.
   ============================================================================= */
/* =============================================================================
   EINZEL-KOLLISIONSPRÜFUNG
   ============================================================================= */
static inline void check_platform_collision(Player *p, Platform *plat)
{
    Entity *e = &p->ent;
    Entity *pl = &plat->ent;

    /* 1. GANZ OBEN: Wir merken uns den Ursprungswert des Players */
    fix32 original_x_f32 = e->x_f32;

    /* Halbe Breiten/Höhen */
    u8 h_pw = e->width >> 1;
    u8 h_ph = e->height >> 1;
    u8 h_ew = pl->width >> 1;
    u8 h_eh = pl->height >> 1;

    /* Integer-Positionen */
    s16 p_x  = F32_toInt(e->x_f32);
    s16 p_y  = F32_toInt(e->y_f32);
    s16 pl_x = F32_toInt(pl->x_f32);
    s16 pl_y = F32_toInt(pl->y_f32);

    /* Distanzen basierend auf MITTEN */
    s16 dx = (p_x + h_pw) - (pl_x + h_ew); 
    s16 dy = (p_y + h_ph) - (pl_y + h_eh);
    s16 abs_dx = (dx < 0) ? -dx : dx;
    s16 abs_dy = (dy < 0) ? -dy : dy;

    bool pos_changed_x = false;
    bool pos_changed_y = false;

    /* --- X-SWEEP --- */
    if (p->state != P_GROUNDED && abs_dy < (h_ph + h_eh - 1))
    {
        s16 gapX = abs_dx - (h_pw + h_ew);
        s16 stick_tolerance = (p->state == P_ON_WALL) ? 3 : 1;

        if (gapX <= stick_tolerance)
        {
            SET_P_FLAG(p->physics_state, P_FLAG_ON_WALL);
            plat->touched = true;
            if (gapX < 0)
            {
                if (dx > 0) p_x = pl_x + pl->width; 
                else        p_x = pl_x - e->width;
                pos_changed_x = true;
            }
            p->solid_vy = pl->vy;
            p->solid_vx = pl->vx;
        }
    }

    /* --- Y-SWEEP --- */
    if (abs_dx < (h_pw + h_ew - 2))
    {
        s16 fall_margin = (e->vy > F16_0) ? F16_toInt(e->vy) + 3 : 4;

        if (abs_dy < (h_ph + h_eh + fall_margin))
        {
            if (dy > 0) // Kopfstoß
            {
                if (e->vy < F16_0 && abs_dy < (h_ph + h_eh))
                {
                    p_y = pl_y + pl->height;
                    e->vy = 0;
                    pos_changed_y = true;

                    /* 2. HARD RESET AUF X: Wenn kein Wandkontakt, scheiß auf den X-Sweep */
                    if (!CHECK_P_FLAG(p->physics_state, P_FLAG_ON_WALL))
                    {
                        e->x_f32 = original_x_f32; // Zurück auf Ursprungsposition
                        p_x = F32_toInt(original_x_f32);
                        pos_changed_x = true;
                    }
                }
            }
            else // Landen
            {
                s16 target_y = pl_y - e->height;
                if (p->state == P_GROUNDED || e->vy >= F16_0)
                {
                    p_y = target_y;
                    pos_changed_y = true;
                    p->state = P_GROUNDED;
                    SET_P_FLAG(p->physics_state, P_FLAG_ON_GROUND);
                    plat->touched = true;
                    e->vy = F16_0;
                    p->solid_vx = pl->vx;
                    p->solid_vy = pl->vy;
                }
            }
        }
    }

    /* --- FINALE ÜBERNAHME --- */
    if (pos_changed_x) {
        e->x = p_x;
        /* Falls kein Hard-Reset stattfand, synchen wir hier normal */
        if (e->x_f32 != original_x_f32) e->x_f32 = FIX32(p_x);
    }
    if (pos_changed_y) {
        e->y = p_y;
        e->y_f32 = FIX32(p_y);
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
            /* Cast von Entity* zu Platform* für die neue Funktionssignatur */
            Platform* plat = (Platform*) entities[i];

            if (plat->enabled)
                check_platform_collision(p, plat);
        }
    }
}