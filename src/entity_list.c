#include "entity_list.h"
#include "../res/title.h" "

EntitySlot entity_pool[MAX_ENTITIES];
Entity* entities[MAX_ENTITIES];
u8 entity_used[MAX_ENTITIES];

void init_entities() {
    for (u16 i = 0; i < MAX_ENTITIES; i++) {
        entity_used[i] = 0;
        entities[i] = &entity_pool[i].entity;
        entity_pool[i].entity.type = ENTITY_NONE;
        entity_pool[i].entity.sprite = NULL;
    }
}

int create_entity(s16 x, s16 y, u8 w, u8 h, EntityType type) {
    for (u16 i = 0; i < MAX_ENTITIES; i++) {
        if (entity_used[i] == 0) {
            entity_used[i] = 1;
            
            Entity* e = entities[i];
            e->type = type;
            e->x = x;
            e->y = y;
            e->x_f32 = FIX32(x);
            e->y_f32 = FIX32(y);
            e->width = w;
            e->height = h;
            
            if (type == ENTITY_PLAYER) {
                e->sprite = SPR_addSprite(&player_sprite, x, y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
                
            }

            if (type == ENTITY_PLATFORM) {
                e->sprite = SPR_addSprite(&stone_sprite, x, y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE)); 
                SPR_setPriority(e->sprite, 0);
            }
            
            return i;
        }
    }
    return -1;
}

