#include "entities/handle_entities.h"
#include "globals.h"
#include "background.h"
#include "entities/player/update_player.h"
#include "entities/update_platform.h"
#include "sprites.h"
#include "area.h"
#include "entities/update_camera.h"
#include <string.h>

EntitySlot entity_pool[MAX_ENTITIES];
Entity* entities[MAX_ENTITIES];
u8 entity_used[MAX_ENTITIES];

void init_entities() {
    for (u16 i = 0; i < MAX_ENTITIES; i++) {
        entity_used[i] = 0;
        entities[i] = &entity_pool[i].entity;
        entity_pool[i].entity.type = ENTITY_NONE;
        entity_pool[i].entity.sprite = NULL;
        entity_pool[i].entity.update = NULL;
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
            e->spr_visible = true;
            e->update = NULL;
            e->sprite = NULL;
            return i;
        }
    }
    return -1;
}

Platform* create_platform(const PlatformDef* def) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (entity_used[i] == 0) {
            entity_used[i] = 1;
            Platform* self = &entity_pool[i].platform;
            
            // 1. Den gesamten Speicherblock nullen
            memset(self, 0, sizeof(Platform));

            self->ent.type = ENTITY_PLATFORM;
            self->ent.update = (void (*)(Entity*))ENTITY_UPDATE_platform;

            // Koordinaten setzen (Pixel zu Subpixel)
            self->origin_x = def->x << 3;
            self->origin_y = def->y << 3;
            self->target_x = def->target_x << 3;
            self->target_y = def->target_y << 3;

            self->ent.x = self->origin_x;
            self->ent.y = self->origin_y;
            self->ent.x_f32 = self->ent.x_old_f32 = FIX32(self->ent.x);
            self->ent.y_f32 = self->ent.y_old_f32 = FIX32(self->ent.y);

            // --- KRITISCHER FIX: MATHE-VARIABLEN RESETTEN ---
            self->anim_timer_x = F16_0; // Phase zwingend auf 0
            self->status_bits = 0;     // Alle Logik-Bits (inkl. 0x8000) auf 0
            self->dir_x = F16_0;       // Vektoren nullen (werden im ersten Update berechnet)
            self->dir_y = F16_0;
            // ------------------------------------------------

            self->flags     = def->flags;
            self->speed     = def->speed;
            self->amplitude = def->amplitude;
            self->state     = PLAT_IDLE;
            self->enabled   = true;
            self->touched   = false;
            
            self->ent.width = (self->flags & PLAT_FLAG_WIDE) ? 32 : 16;
            self->ent.height = 16;

            // Sprite-Initialisierung
            if (self->flags & PLAT_FLAG_CAMO) {
                self->ent.width = self->ent.height = 32;
                self->ent.sprite = SPR_addSprite(&breakable_sprite, self->ent.x, self->ent.y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
            } else if (self->ent.width > 16) {
                self->ent.sprite = SPR_addSprite(&stone2_sprite, self->ent.x, self->ent.y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
            } else {
                self->ent.sprite = SPR_addSprite(&stone_sprite, self->ent.x, self->ent.y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
            }

            self->ent.spr_visible = (self->flags & PLAT_FLAG_INVISIBLE) ? false : true;
            
            entities[i] = (Entity*)self; 
            return self;
        }
    }
    return NULL;
}

void spawn_platforms(const LevelDefinition* lv) {
    for (u16 i = 0; i < lv->platform_count; i++) {
        create_platform(&lv->platforms[i]);
    }        
}

Pickup* create_pickup(const PickupDef* def) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (entity_used[i] == 0) {
            entity_used[i] = 1;
            Pickup* self = &entity_pool[i].pickup;
            memset(self, 0, sizeof(Pickup));

            self->ent.type = ENTITY_PICKUP;
            self->ent.update = ENTITY_UPDATE_pickup;
            self->trigger = ENTITY_TRIGGER_pickup;
            
            self->ent.x = self->origin_x = def->x << 3;
            self->ent.y = self->origin_y = def->y << 3;
            self->state = PICK_IDLE;
            self->ent.x_f32 = self->ent.x_old_f32 = FIX32(self->ent.x);
            self->ent.y_f32 = self->ent.y_old_f32 = FIX32(self->ent.y);
            
            self->ent.width = 16;
            self->ent.height = 16;
            self->kind = def->kind;

            if (self->kind == PICKUP_HEART || self->kind == PICKUP_HEART_FLEEING)  {
                self->ent.sprite = SPR_addSprite(&heart_sprite, self->ent.x, self->ent.y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
            } else if (self->kind == PICKUP_BALLOON) {
                self->ent.sprite = SPR_addSprite(&ballon_sprite, self->ent.x, self->ent.y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
            } else if (self->kind == PICKUP_SPRING){
                self->ent.sprite = SPR_addSprite(&spring_sprite, self->ent.x, self->ent.y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
            } else if (self->kind == PICKUP_NEXTLEVEL){
                self->ent.sprite = SPR_addSprite(&stone_sprite, self->ent.x, self->ent.y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
            }
            
            self->ent.spr_visible = true;            
            entities[i] = (Entity*)self;
            return self;
        }
    }
    return NULL;
}

void spawn_pickups(const LevelDefinition* lv) {
    for (u16 i = 0; i < lv->pickup_count; i++) {
        create_pickup(&lv->pickups[i]);
    }
}

void spawn_player(u16 spawn_in_area) {
    const Area* start_area = get_area(spawn_in_area);
    if (start_area) {
        s16 spawn_x = start_area->spawn.x << 3; 
        s16 spawn_y = start_area->spawn.y << 3;
        player_id = create_entity(spawn_x, spawn_y, 13, 13, F16_0, F16_0, ENTITY_PLAYER);
        
        if (player_id != -1) {
            Player* pl = (Player*) entities[player_id];
            pl->ent.update = (void (*)(struct Entity*))ENTITY_UPDATE_player;
            pl->physics_state = 0; 
            pl->timer_stamina = 500;
            pl->state = P_FALLING;
            pl->state_old = P_FALLING;
            pl->physics_state = 0; 
            pl->timer_grace = 0;
            pl->timer_buffer = 0;
            pl->timer_shot_jump = 0;
            pl->count_shot_jump = shot_jump_max;
            pl->current_area = (Area*)start_area;
            pl->ent.sprite = SPR_addSprite(&player_sprite, pl->ent.x, pl->ent.y, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
        }
        camera_position.x = spawn_x - 160;
        camera_position.y = spawn_y - 112;
    }
}

void clear_entities() {
    for (u16 i = 0; i < MAX_ENTITIES ; i++) {
        if (entities[i] != NULL && entities[i]->sprite != NULL) {
            SPR_releaseSprite(entities[i]->sprite);
            entities[i]->sprite = NULL;
        }
        
        entity_used[i] = 0;
        entities[i] = &entity_pool[i].entity;
        entities[i]->type = ENTITY_NONE;
        entities[i]->sprite = NULL;
        entities[i]->update = NULL;
    }
    player_id = -1;
}