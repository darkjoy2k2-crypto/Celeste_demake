#include "entities/update_platform.h"
#include "globals.h"

void ENTITY_UPDATE_platform(Entity* _e) {
    if (!GameSync) return;
    Platform* self = (Platform*)_e;

    // --- 1. VISUELLE LOGIK (DEIN SCREENSHOT-EFFEKT) ---
    // Animation 1 = Gestrichelte Umrandung (Ghost)
    // Animation 0 = Voller Block (Solid)
    
    if (CHECK_P_FLAG(self->flags, PLAT_FLAG_INVISIBLE)) {
        SPR_setAnim(self->ent.sprite, 1); 
    } else {
        SPR_setAnim(self->ent.sprite, 0);
    }

    // --- 2. STATUS-MASCHINE ---
    switch (self->state) {
        case PLAT_IDLE:
            if (self->touched) {
                // Bei Berührung wird aus Ghost (1) -> Solid (0)
                if (CHECK_P_FLAG(self->flags, PLAT_FLAG_INVISIBLE)) {
                    CLEAR_P_FLAG(self->flags, PLAT_FLAG_INVISIBLE);
                }
                
                // Trigger für Lifts (PLAT_FLAG_TOUCH_START)
                if (CHECK_P_FLAG(self->flags, PLAT_FLAG_TOUCH_START)) {
                    SET_P_FLAG(self->status_bits, 0x0001);
                }
                
                // Zerbrechliche Blöcke (PLAT_FLAG_BREAKABLE)
                if (CHECK_P_FLAG(self->flags, PLAT_FLAG_BREAKABLE)) {
                    self->wait_timer = self->timer_a;
                    self->state = PLAT_BREAKING;
                }
            }
            break;

        case PLAT_BREAKING:
            if (self->wait_timer > 0) {
                self->wait_timer--;
                // Optional: Beim Zerbrechen wieder auf Umrandung (1) wechseln
                SPR_setAnim(self->ent.sprite, 1);
                if (self->wait_timer % 4 < 2) self->ent.x++; else self->ent.x--;
            } else {
                self->enabled = false;
                SPR_setVisibility(self->ent.sprite, HIDDEN);
                if (CHECK_P_FLAG(self->flags, PLAT_FLAG_RESPAWN)) {
                    self->wait_timer = self->timer_b;
                    self->state = PLAT_HIDDEN;
                } else self->state = PLAT_DISABLED;
            }
            break;

        case PLAT_HIDDEN:
            if (self->wait_timer > 0) self->wait_timer--;
            else { 
                self->enabled = true; 
                SPR_setVisibility(self->ent.sprite, VISIBLE);
                self->state = PLAT_IDLE; 
            }
            break;

        case PLAT_DISABLED: return;
    }

    // --- 3. BEWEGUNG (Deine vtimer-Logik) ---
    self->ent.x_old = self->ent.x;
    self->ent.y_old = self->ent.y;

    bool is_moving = !(CHECK_P_FLAG(self->flags, PLAT_FLAG_TOUCH_START)) || (self->status_bits & 0x0001);

    if (self->enabled && is_moving) {
        s16 speed_val = F16_toInt(self->speed);
        fix32 offset_f32 = 0;

        if (CHECK_P_FLAG(self->flags, PLAT_FLAG_SINUS)) {
            offset_f32 = getSinusValueF32(vtimer, speed_val, self->range);
        } else {
            s32 total_way = self->range << 1;
            if (total_way > 0) {
                s32 pos = (vtimer * speed_val) % total_way;
                if (pos > self->range) pos = total_way - pos;
                offset_f32 = FIX32(pos);
            }
            
            // ONCE-Halt (für Lifts)
            if (CHECK_P_FLAG(self->flags, PLAT_FLAG_ONCE)) {
                if ((vtimer * speed_val) >= (u32)self->range) {
                    offset_f32 = FIX32(self->range);
                    self->status_bits &= ~0x0001;
                }
            }
        }

        self->ent.x_f32 = FIX32(self->origin_x << 3);
        self->ent.y_f32 = FIX32(self->origin_y << 3);

        if (CHECK_P_FLAG(self->flags, PLAT_FLAG_X)) self->ent.x_f32 += offset_f32;
        if (CHECK_P_FLAG(self->flags, PLAT_FLAG_Y)) self->ent.y_f32 += offset_f32;
        
        self->ent.x = F32_toInt(self->ent.x_f32);
        self->ent.y = F32_toInt(self->ent.y_f32);
    }

    // --- 4. SYNC ---
    self->ent.vx = FIX16(self->ent.x - self->ent.x_old);
    self->ent.vy = FIX16(self->ent.y - self->ent.y_old);

    if (self->enabled) {
        SPR_setPosition(self->ent.sprite, self->ent.x, self->ent.y);
    }

    self->touched = false;
}