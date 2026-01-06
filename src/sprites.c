#include <genesis.h>
#include "entity_list.h"

void handle_all_animations(){
    for (int i = 0; i < MAX_ENTITIES; i++){
        if (entity_used[i] == 1 && entities[i] != NULL){
            update_animation(entities[i]);
        }
    }
}

void update_animation(Entity* e){
    switch (e->type)
    {
    case ENTITY_PLAYER:
        // Hier greifen wir direkt auf x zu, da 'ent.' nicht existiert
        int dx = e->x_old - e->x; 
        e->anim_index += dx; 

        if (e->anim_index < 0) e->anim_index += 50;
        if (e->anim_index > 49) e->anim_index -= 50;

        SPR_setAnimAndFrame(e->sprite, 0, e->anim_index / 6);
        break;
    case ENTITY_PLATFORM:
        SPR_setAnimAndFrame(e->sprite, 0, 0);
        break;
    default:
        break;
    }
}