#include "entities/handle_entities.h"
#include "globals.h"
#include "title.h"
#include "entities/player/update_player.h"
#include "entities/update_platform.h"
#include "level.h"
#include "area.h"
#include "entities/update_camera.h"

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
                
                if (self->behavior == PB_SINUS_WIDE_X || self->behavior == PB_SINUS_WIDE_Y)
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
                
                if (self->behavior == PB_SINUS_WIDE_X || self->behavior == PB_SINUS_WIDE_Y )
                    self->ent.sprite = SPR_addSprite(&stone2_sprite, self->ent.x, self->ent.y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE)); 
                else
                    self->ent.sprite = SPR_addSprite(&stone_sprite, self->ent.x, self->ent.y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE)); 

                    SPR_setPriority(self->ent.sprite, 0);

                return self;
            
        }
    }
    return NULL;
}


void spawn_player(u16 spawn_in_area) {

    const Area* start_area = get_area(spawn_in_area);

    if (start_area) {
        s16 spawn_x = start_area->spawn.x << 3; 
        s16 spawn_y = start_area->spawn.y << 3;

        player_id = create_entity(spawn_x, spawn_y, 13, 13, F16_0, F16_0, ENTITY_PLAYER);
    
        if (player_id != -1) {
            Player* pl = (Player*) entities[player_id];
            pl->state = P_FALLING;
            pl->state_old = P_FALLING;
            pl->physics_state = 0; 
            CLEAR_P_FLAG(pl->physics_state, P_FLAG_FACING_LEFT);
            
            // Critical: Reset gameplay timers
            pl->timer_stamina = 100;
            pl->timer_grace = 0;
            pl->timer_buffer = 0;
            pl->timer_shot_jump = 0;
            pl->count_shot_jump = 0;
            pl->current_area = (Area*)start_area;
        }
        camera_position.x = spawn_x - 160;
        camera_position.y = spawn_y - 112;
    }
}




void spawn_platforms(const LevelDefinition* lv) {
    for (u16 i = 0; i < lv->platform_count; i++) {
        const PlatformDef* p = &lv->platforms[i];
        Platform* plat = create_platform(p->x, p->y, p->speed, p->behavior);
        plat->touched = false;
        plat->enabled = true;
        plat->wait_timer = 0;
    }
}