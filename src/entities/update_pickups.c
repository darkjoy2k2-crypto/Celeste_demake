#include "entities/update_platform.h"
#include "globals.h"
#include "states/states.h"

void ENTITY_UPDATE_pickup(Entity* _e) {
    if (!GameSync) return;
    Pickup* self = (Pickup*)_e;
    Player* p = (Player*) entities[player_id];

    switch (self->kind){
        case PICKUP_HEART:
            if (p->state_old != P_GROUNDED && p->state == P_GROUNDED){
                if (self->collected && !self->counted ) {
                    self->counted = true;
                }
            }
            
        break;
        case PICKUP_BALLOON:

            if (p->state_old != P_GROUNDED && p->state == P_GROUNDED && self->collected) {
                self->collected = false;
                SPR_setVisibility(self->ent.sprite, VISIBLE);
            }

            if (self->collected) return;

            fix32 float_offset = sinFix32(vtimer << 4); 
            
            s16 draw_y = F32_toInt(self->ent.y_f32) + F32_toInt(float_offset >> 1);

            if (vtimer & 64) {
                SPR_setHFlip(self->ent.sprite, TRUE);
            } else {
                SPR_setHFlip(self->ent.sprite, FALSE);
            }
            
            self->ent.x = F32_toInt(self->ent.x_f32);
            self->ent.y = draw_y; 
            SPR_setPosition(self->ent.sprite, self->ent.x, self->ent.y);
        break;

case PICKUP_SPRING:
    if (self->anim_running) {
        self->time++;

        // Geschwindigkeit (15 Frames warten für 1 Sekunde Rücklauf ist okay)
        if (self->time > 1){
            self->time = 0;

            if (self->anim_frame == 1 && self->anim_direction == 1)
                p->ent.vy = JUMP_FORCE_SPRING;                

            self->anim_frame += self->anim_direction;

            // HARTE GRENZEN (0 bis 7)
            if (self->anim_direction == 1) {
                // Wenn wir ausfahren
                if (self->anim_frame >= 7) {
                    self->anim_frame = 7;
                    self->anim_direction = -1; // Umkehren
                    p->ent.vx = F16_0;

                }
            } else {
                // Wenn wir einfahren
                if (self->anim_frame <= 0) {
                    self->anim_frame = 0;
                    self->anim_running = false; // STOPP
                    self->anim_direction = 1;
                }
            }

            // Sicherstellen, dass SGDK niemals einen illegalen Frame bekommt
            if (self->anim_frame > 7) self->anim_frame = 7;
            if (self->anim_frame < 0) self->anim_frame = 0;

            SPR_setFrame(self->ent.sprite, self->anim_frame);
                    }

        }

break;
        default:
        break;
    }



}

void ENTITY_TRIGGER_pickup(Entity* _e) {
    Pickup* self = (Pickup*)_e;
    Player* p = (Player*) entities[player_id];

    if (self->collected) return;
        

    switch (self->kind){
        case PICKUP_HEART:
        case PICKUP_COIN:
            SPR_setVisibility(self->ent.sprite, HIDDEN);
            self->collected = true;
            break;

        case PICKUP_BALLOON:
            SPR_setVisibility(self->ent.sprite, HIDDEN);
            self->collected = true;
            p->count_shot_jump++;

            break;

        case PICKUP_SPRING:
            if (self->anim_running) return;
            self->anim_running = true;
            self->anim_frame = 0;
            self->anim_direction = 1;

                    
            break;

        case PICKUP_NEXTLEVEL:
            current_level_index++;
            STATE_set(&State_InGame);
        break;
        default:
    }
}
