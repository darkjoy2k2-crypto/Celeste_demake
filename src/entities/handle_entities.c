#include "entities/handle_entities.h"
#include "globals.h"
#include "title.h"
#include "entities/player/update_player.h"
#include "entities/update_platform.h"

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

int create_entity(s16 x, s16 y, u8 w, u8 h, f16 vx, f16 vy, EntityType type) {
    for (u16 i = 0; i < MAX_ENTITIES; i++) {
        if (entity_used[i] == 0) {
            entity_used[i] = 1;
            
            Entity* e = entities[i];
            e->type = type;

            e->x = x;
            e->y = y;
            e->x_f32 = FIX32(x);
            e->y_f32 = FIX32(y);

            e->vx = vx;
            e->vy = vy;

            e->x_old = x;
            e->y_old = y;
            e->x_old_f32 = e->x_f32;
            e->y_old_f32 = e->y_f32;

            e->width = w;
            e->height = h;
            
            if (type == ENTITY_PLAYER) {
                Player* p = (Player*)e;
                
                /* Funktionszeiger zuweisen */
                e->update = ENTITY_UPDATE_player;
                
                p->physics_state = 0; 
                CLEAR_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT);
                
                p->timer_stamina = 300;
                p->count_shot_jump = 2;
                e->sprite = SPR_addSprite(&player_sprite, x, y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
                
                player_id = i;
            }
            else {
                /* Fallback für unbekannte Typen */
                e->update = NULL;
            }
            
            return i;
        }
    }
    return -1;
}
void destroy_entity(int index) {
    if (index >= 0 && index < MAX_ENTITIES) {
        if (entity_used[index]) {
            if (entities[index]->sprite) {
                SPR_releaseSprite(entities[index]->sprite);
            }
            entity_used[index] = 0;
            entities[index]->type = ENTITY_NONE;
            entities[index]->sprite = NULL;
        }
    }
}

Platform* create_platform(
    s16 _originX, s16 _originY, fix16 _speed, PlatformBehavior _platformBehavior) {

    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (entity_used[i] == 0) {
            entity_used[i] = 1;

                Entity* e = entities[i];
                Platform* self = (Platform*)e;

                self->ent.type = ENTITY_PLATFORM;

                self->origin_x = _originX;
                self->ent.x = self->ent.x_old = _originX << 3;

                self->origin_y = _originY;
                self->ent.y = self->ent.y_old = _originY << 3;

                self->ent.x_old_f32 = self->ent.x_f32 = FIX32(self->ent.x);
                self->ent.y_old_f32 = self->ent.y_f32 = FIX32(self->ent.y);

                self->behavior = _platformBehavior;
                
                if (self->behavior == PB_SINUS_WIDE_X || PB_SINUS_WIDE_Y)
                    self->ent.width = 32;
                else
                    self->ent.width = 16;

                self->ent.height = 16;


                self->ent.type = ENTITY_PLATFORM;
                self->speed = _speed;
                self->platform_state = 0;
                self->wait_timer = 0;

                self->ent.update = (void (*)(Entity*))ENTITY_UPDATE_platform;

                entities[i] = (Entity*)self;
                
                if (self->behavior == PB_SINUS_WIDE_X || PB_SINUS_WIDE_Y )
                    self->ent.sprite = SPR_addSprite(&stone2_sprite, self->ent.x, self->ent.y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE)); 
                else
                    self->ent.sprite = SPR_addSprite(&stone_sprite, self->ent.x, self->ent.y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE)); 

                    SPR_setPriority(self->ent.sprite, 0);

                return self;
            
        }
    }
    return NULL;
}