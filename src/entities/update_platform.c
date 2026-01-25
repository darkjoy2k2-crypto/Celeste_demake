#include "entities/update_platform.h"
#include "globals.h"

void ENTITY_UPDATE_platform(Entity* _e) {
    if (!GameSync) return;
    Platform* self = (Platform*)_e;

    // --- 1. STATUS-MASCHINE ---
    switch (self->state) {
        case PLAT_IDLE:
            if (self->touched) {
                if (CHECK_P_FLAG(self->flags, PLAT_FLAG_INVISIBLE)) CLEAR_P_FLAG(self->flags, PLAT_FLAG_INVISIBLE);
                if (CHECK_P_FLAG(self->flags, PLAT_FLAG_TOUCH_START)) SET_P_FLAG(self->status_bits, 0x0001); 
                if (CHECK_P_FLAG(self->flags, PLAT_FLAG_BREAKABLE)) {
                    self->wait_timer = self->timer_a;
                    self->state = PLAT_BREAKING;
                }
            }
            break;

        case PLAT_BREAKING:
            if (self->wait_timer > 0) {
                self->wait_timer--;
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

    // --- 2. BEWEGUNG ---
    self->ent.x_old = self->ent.x;
    self->ent.y_old = self->ent.y;

    bool is_moving = !(CHECK_P_FLAG(self->flags, PLAT_FLAG_TOUCH_START)) || (self->status_bits & 0x0001);

    if (self->enabled && is_moving) {
        fix32 offset_f32 = 0;

        if (CHECK_P_FLAG(self->flags, PLAT_FLAG_SINUS)) {
            // Sinus nutzt vtimer -> globaler Takt, kein Überlauf-Problem hier
            s16 speed_val = F16_toInt(self->speed);
            offset_f32 = getSinusValueF32(vtimer, speed_val, self->range);
        } else {
            // LINEAR (Lift & Pendel)
            self->anim_timer += self->speed;
            
            fix32 current_progress = F16_toFix32(self->anim_timer);
            fix32 range_f32 = FIX32(self->range);

            if (CHECK_P_FLAG(self->flags, PLAT_FLAG_ONCE)) {
                // Einmalige Fahrt (Lift)
                if (current_progress >= range_f32) {
                    offset_f32 = range_f32;
                    self->status_bits &= ~0x0001;
                } else {
                    offset_f32 = current_progress;
                }
            } else {
                // Pendel-Logik mit FIX für den Versatz-Fehler (Modulo-Ersatz)
                fix32 total_way = range_f32 * 2;
                if (total_way > 0) {
                    // RESET-LOGIK: Wenn ein Zyklus durch ist, Timer zurücksetzen
                    // Das verhindert den Versatz nach 4-5 Durchgängen
                    if (current_progress >= total_way) {
                        self->anim_timer = 0;
                        current_progress = 0;
                    }

                    if (current_progress > range_f32) {
                        offset_f32 = total_way - current_progress;
                    } else {
                        offset_f32 = current_progress;
                    }
                }
            }
        }

        // POSITIONS-UPDATE
        fix32 base_x = FIX32(self->origin_x << 3);
        fix32 base_y = FIX32(self->origin_y << 3);

        if (CHECK_P_FLAG(self->flags, PLAT_FLAG_X)) self->ent.x_f32 = base_x + offset_f32;
        else self->ent.x_f32 = base_x;

        if (CHECK_P_FLAG(self->flags, PLAT_FLAG_Y)) self->ent.y_f32 = base_y - offset_f32;
        else self->ent.y_f32 = base_y;

        self->ent.x = F32_toInt(self->ent.x_f32);
        self->ent.y = F32_toInt(self->ent.y_f32);
    }

    // --- 3. PHYSIK & SPRITE ---
    self->ent.vx = FIX16(self->ent.x - self->ent.x_old);
    self->ent.vy = FIX16(self->ent.y - self->ent.y_old);

    SPR_setPosition(self->ent.sprite, self->ent.x, self->ent.y);
    self->touched = false;
}