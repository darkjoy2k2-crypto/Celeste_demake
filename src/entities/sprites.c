#include "entities/sprites.h"

#include <genesis.h>
#include "entities/handle_entities.h"
#include "globals.h"

static inline void update_animation(Entity* e) {
    switch (e->type)
    {
        case ENTITY_PLAYER:
        {
            /* Caching des Player-Pointers für Zustandsabfragen */
            Player* p = (Player*)e;
            s16 dx = 0; 

            /* Einfache Lauf-Animation basierend auf X-Differenz */


            if (CHECK_P_FLAG(p->physics_state, P_FLAG_DYING) ){
                e->anim_index += 1; 
                if (e->anim_index >= 59) {     
                    e->anim_index = 0;
                }   
                SPR_setAnimAndFrame(e->sprite,4,  e->anim_index / 15);



            } else if (CHECK_P_FLAG(p->physics_state, P_FLAG_ON_WALL) ){
                if (abs16(p->ent.vy) > FIX16(0.3)){
                    e->anim_index += 1; 
                    if (e->anim_index >= 119) {     
                        e->anim_index = 0;
                    }   
                    SPR_setAnimAndFrame(e->sprite,3,  e->anim_index / 20);
                
                }
                if (CHECK_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT)) {
                    SPR_setHFlip(e->sprite, TRUE);
                } else {
                    SPR_setHFlip(e->sprite, FALSE);
                }
            }
            
            else if (p->ent.vy < FIX16(0.5)){
                e->anim_index += 1; 
                if (e->anim_index >= 79) {     
                    e->anim_index = 0;
                }   
                SPR_setAnimAndFrame(e->sprite,2,  e->anim_index / 20);
            } else if (p->ent.vy > FIX16(0.5)){
                e->anim_index += 1; 
                if (e->anim_index >= 79) {     
                    e->anim_index = 0;
                }   
                SPR_setAnimAndFrame(e->sprite,2, 4 + e->anim_index / 20);

            } else if (p->ent.vx == F16_0){
                e->anim_index += 1; 
                if (e->anim_index >= 119) {                
                    e->anim_index = 0;
                }                    
                SPR_setAnimAndFrame(e->sprite,1,  e->anim_index / 20);


} else {
    // dx is the difference in pixels
    dx = e->x_old - e->x; 
    e->anim_index += abs(dx); 

    // STRICT BOUNDS: Never let it reach 36.
    // 35 is the absolute maximum for a 6-frame animation with a /6 divisor.
    if (e->anim_index >= 36) {
        e->anim_index = 0;
    }

    // Calculation: 35 / 6 = 5 (Valid). 36 / 6 = 6 (CRASH).
    SPR_setAnimAndFrame(e->sprite, 0, e->anim_index / 6);
    
    if (CHECK_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT)) {
        SPR_setHFlip(e->sprite, TRUE);
    } else {
        SPR_setHFlip(e->sprite, FALSE);
    }
}




        }
        break;

case ENTITY_PLATFORM:
        {
            Platform* plat = (Platform*)e;
            
            if (!plat->enabled) {
                // Wenn kaputt/deaktiviert: Animation 1 (oder verstecken)

                    SPR_setAnim(e->sprite, 1);
                

            } 
            else if (CHECK_P_FLAG(plat->flags, PLAT_FLAG_INVISIBLE)) {
                // Wenn noch nicht entdeckt: Animation 1 (Geisterbild/Umrandung)
                SPR_setAnim(e->sprite, 1);
            }
            else {
                // Aktiv und entdeckt: Animation 0 (Voll gezeichnet)
                SPR_setAnim(e->sprite, 0);
            }
        }
        break;
        
        case ENTITY_PICKUP:
        {
            Pickup* p = (Pickup*)e;
            
            SPR_setAnim(p->ent.sprite, 0);
        }
        break;

        default:
        break;
    }
}

void handle_all_animations() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        /* entity_used entfernt, direkter Check auf NULL */
        if (entities[i] != NULL) {
            update_animation(entities[i]);
        }
    }
}