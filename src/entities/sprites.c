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
            
            /* Einfache Lauf-Animation basierend auf X-Differenz */
            s16 dx = e->x_old - e->x; 
            e->anim_index += abs16(dx); 

            if (e->anim_index > 59) e->anim_index -= 60;

            /* Sprite-Update */
            SPR_setAnimAndFrame(e->sprite, 0, e->anim_index / 6);
            
            /* Flip-Handling über das Facing-Flag */
            if (CHECK_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT)) {
                SPR_setHFlip(e->sprite, TRUE);
            } else {
                SPR_setHFlip(e->sprite, FALSE);
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