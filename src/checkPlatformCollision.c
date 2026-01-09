#include <genesis.h>
#include "checkTileCollisions.h"
#include "tiles.h"
#include "entity_list.h" 
#include "level.h"
#include "debug.h"
#include "globals.h"

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

    fix32 gapX = abs_dx - (h_pw + h_ew);
    fix32 gapY = abs_dy - (h_ph + h_eh);

    if (gapX >= 0 || gapY >= 0) return;

    if (gapX > gapY)
    {
        if (dx > 0) p->ent.x_f32 = plat->x_f32 + h_ew + h_pw;
        else p->ent.x_f32 = plat->x_f32 - h_ew - h_pw;
        
        p->ent.vx = F16_0;
        p->is_on_wall = true;
    }
    else
    {
        if (dy > 0)
        {
            p->ent.y_f32 = plat->y_f32 + h_eh + h_ph;
            p->ent.vy = 0;
        }
        else 
        {
            p->ent.y_f32 = plat->y_f32 - h_eh - h_ph;

            p->ent.x_f32 += F16_toFix32(plat->vx);

            p->ent.vy = plat->vy;
            p->state = P_GROUNDED;
        }
    }

    p->ent.x = F32_toInt(p->ent.x_f32);
    p->ent.y = F32_toInt(p->ent.y_f32);
}