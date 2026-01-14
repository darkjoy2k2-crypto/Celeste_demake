#include <genesis.h>
#include "checkTileCollisions.h"
#include "tiles.h"
#include "entity_list.h" 
#include "level.h"
#include "debug.h"
#include "globals.h"


void handle_platform_collision(Entity *entity){

    Player* p = (entity->type == ENTITY_PLAYER) ? (Player*)entity : NULL;
    
    p->solid_vx = p->solid_vy = F16_0;

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
    u8 h_pw = p->ent.width >> 1;
    u8 h_ph = p->ent.height >> 1;
    u8 h_ew = plat->width >> 1;
    u8 h_eh = plat->height >> 1;

    s16 p_x = F32_toInt(p->ent.x_f32);
    s16 p_y = F32_toInt(p->ent.y_f32);
    s16 pl_x = F32_toInt(plat->x_f32);
    s16 pl_y = F32_toInt(plat->y_f32);

    s16 dx = p_x - pl_x;
    s16 dy = p_y - pl_y;
    s16 abs_dx = (dx < 0) ? -dx : dx;
    s16 abs_dy = (dy < 0) ? -dy : dy;

    bool pos_changed_x = false;
    bool pos_changed_y = false;

// --- 1. X-SWEEP ---
// Wir erhöhen die Toleranz von 4 auf 1, damit der Bereich größer wird
if (p->state != P_GROUNDED && abs_dy < (h_ph + h_eh - 1)) 
{
    s16 gapX = abs_dx - (h_pw + h_ew);
    
    // Wir erlauben einen winzigen Puffer (1px), um das "Zittern" abzufangen
    if (gapX <= 1) 
    {
        p->is_on_wall = true;
        
        // Nur wenn wir wirklich überlappen (gapX < 0), korrigieren wir die Position
        if (gapX < 0) {
            if (dx > 0) p_x = pl_x + h_ew + h_pw;
            else p_x = pl_x - h_ew - h_pw;
            pos_changed_x = true;
        }

        // Falls die Plattform sich bewegt, muss der Ball die Geschwindigkeit erben,
        // damit er im P_ON_WALL State nicht wegrutscht
        if (plat->vy != 0) p->solid_vy = plat->vy;
        if (plat->vx != 0) p->solid_vx = plat->vx;
    }
}

    // --- 2. Y-SWEEP ---
    if (abs_dx < (h_pw + h_ew - 2)) 
    {
        s16 fall_margin = (p->ent.vy > F16_0) ? F16_toInt(p->ent.vy) + 3 : 4;
        
        if (abs_dy < (h_ph + h_eh + fall_margin)) 
        {
            if (dy > 0) // Kopfstoss
            {
                if (p->ent.vy < F16_0 && dy < (h_ph + h_eh)) 
                {
                    p_y = pl_y + h_eh + h_ph;
                    p->ent.vy = 0;
                    pos_changed_y = true;
                }
            }
            else // Landen/Stehen
            {
                s16 target_y = pl_y - h_eh - h_ph;

                if (p->state == P_GROUNDED) 
                {
                    // Nur korrigieren, wenn die Abweichung zu gross wird (Hysterese)
                    if (abs16(p_y - target_y) > 0) 
                    {
                        p_y = target_y;
                        pos_changed_y = true;
                    }
                } 
                else if (p->ent.vy >= F16_0) 
                {
                    p_y = target_y;
                    pos_changed_y = true;
                    p->state = P_GROUNDED;
                }

                if (p->state == P_GROUNDED)
                {
                    p->is_on_ground = true;   
                    p->ent.vy = F16_0;
                    p->solid_vx = (plat->vx != 0) ? plat->vx : 0;
                    p->solid_vy = (plat->vy != 0) ? plat->vy : 0;
                }
            }
        }
    }

    // --- FINALE ÜBERNAHME ---
    // Wir nutzen konsistent F32_toInt, um Driften durch wechselnde Rundungsmodi zu vermeiden
    if (pos_changed_x) {
        p->ent.x = p_x;
        p->ent.x_f32 = FIX32(p_x);
    } else {
        p->ent.x = F32_toInt(p->ent.x_f32);
    }

    if (pos_changed_y) {
        p->ent.y = p_y;
        p->ent.y_f32 = FIX32(p_y);
    } else {
        p->ent.y = F32_toInt(p->ent.y_f32);
    }
}