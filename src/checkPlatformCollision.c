#include <genesis.h>
#include "checkTileCollisions.h"
#include "tiles.h"
#include "entity_list.h" 
#include "level.h"
#include "debug.h"
#include "globals.h"

#define abs16(x) ((x) < 0 ? -(x) : (x))

void handle_platform_collision(Entity *entity){

    Player* p = (entity->type == ENTITY_PLAYER) ? (Player*)entity : NULL;

    for (int i = 0; i < MAX_ENTITIES; i++){
        if (entity_used[i] == 1 && entities[i] != NULL){
            if (entities[i]->type == ENTITY_PLATFORM ){
                check_platform_collision(p, entities[i]);
            }
        }
    }
}

void check_platform_collision(Player *p, Entity *plat)
{
    fix32 h_pw = FIX32(p->ent.width) >> 1;
    fix32 h_ph = FIX32(p->ent.height) >> 1;
    fix32 h_ew = FIX32(plat->width) >> 1;
    fix32 h_eh = FIX32(plat->height) >> 1;

    fix32 dx = p->ent.x_f32 - plat->x_f32;
    fix32 dy = p->ent.y_f32 - plat->y_f32;
    fix32 abs_dx = (dx < 0) ? -dx : dx;
    fix32 abs_dy = (dy < 0) ? -dy : dy;

    // --- 1. X-SWEEP (Seitlich) ---
    // Wir ignorieren die X-Korrektur, wenn wir sehr nah an der Oberkante sind
    // Das verhindert das "Hängenbleiben" an Ecken beim Landen.
    if (abs_dy < (h_ph + h_eh - FIX32(4))) // Puffer vergrößert auf 4
    {
        fix32 gapX = abs_dx - (h_pw + h_ew);
        if (gapX < FIX32(2)) // Toleranz für is_on_wall
        {
            if (p->state != P_GROUNDED) {
                p->is_on_wall = true;
                if (gapX < 0) { // Nur bei echter Überlappung teleportieren
                    if (dx > 0) p->ent.x_f32 = plat->x_f32 + h_ew + h_pw;
                    else p->ent.x_f32 = plat->x_f32 - h_ew - h_pw;
                }
            }
            if (abs16(plat->vx) > abs16(p->solid_vx)) p->solid_vx = plat->vx;
        }
    }

    // --- 2. Y-SWEEP (Kopfstoß & Landen) ---
    if (abs_dx < (h_pw + h_ew - FIX32(2))) // Leicht eingerückt für smoothe Kanten
    {
        fix32 fall_margin = (p->ent.vy > 0) ? F16_toFix32(p->ent.vy) : FIX32(2);
        
        if (abs_dy < (h_ph + h_eh + fall_margin)) 
        {
            if (dy > 0) // Ball ist UNTER der Plattform (Kopfstoß-Bereich)
            {
                // Kopfstoß-Logik: Nur korrigieren, wenn wir uns nach oben bewegen
                // UND wir tief genug unter der Plattform sind
                if (p->ent.vy < 0 && dy < (h_ph + h_eh)) 
                {
                    p->ent.y_f32 = plat->y_f32 + h_eh + h_ph;
                    p->ent.vy = 0;
                }
            }
            else // Ball ist ÜBER der Plattform (Landen/Stehen)
            {
                if (p->ent.vy >= 0 || p->state == P_GROUNDED) 
                {
                    p->ent.y_f32 = plat->y_f32 - h_eh - h_ph;
                    p->ent.vy = 0;
                    p->state = P_GROUNDED;
                    
                    if (abs16(plat->vx) > abs16(p->solid_vx)) p->solid_vx = plat->vx;
                    if (abs16(plat->vy) > abs16(p->solid_vy)) p->solid_vy = plat->vy;
                }
            }
        }
    }

    p->ent.x = F32_toInt(p->ent.x_f32);
    p->ent.y = F32_toInt(p->ent.y_f32);
}