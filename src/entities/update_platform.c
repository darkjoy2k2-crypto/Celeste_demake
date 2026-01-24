#include "entities/update_platform.h"
#include "globals.h"

void ENTITY_UPDATE_platform(Entity* _e) {
    if (!GameSync) return;
    Platform* self = (Platform*)_e;

    // --- 1. STATUS-MASCHINE (Logik aus dem alten Skript) ---
    switch (self->state) {
        case PLAT_IDLE:
            if (self->touched) {
                if (CHECK_P_FLAG(self->flags, PLAT_FLAG_INVISIBLE)) {
                    CLEAR_P_FLAG(self->flags, PLAT_FLAG_INVISIBLE);
                    SPR_setVisibility(self->ent.sprite, VISIBLE);
                }
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
            else { self->enabled = true; SPR_setVisibility(self->ent.sprite, VISIBLE); self->state = PLAT_IDLE; }
            break;
        case PLAT_DISABLED: return;
    }

    // --- 2. BEWEGUNG (Basierend auf deinem funktionierenden Muster) ---
    
    self->ent.x_old_f32 = self->ent.x_f32;
    self->ent.y_old_f32 = self->ent.y_f32;
    self->ent.x_old = self->ent.x;
    self->ent.y_old = self->ent.y;

    bool is_moving = !(CHECK_P_FLAG(self->flags, PLAT_FLAG_TOUCH_START)) || (self->status_bits & 0x0001);

    if (is_moving) {
        // Wie im alten Skript: Speed zu Int wandeln für die Funktion
        s16 speed_val = F16_toInt(self->speed);
        fix32 offset_f32 = 0;

        if (CHECK_P_FLAG(self->flags, PLAT_FLAG_SINUS)) {
            // Absolute Berechnung über vtimer und range
            offset_f32 = getSinusValueF32(vtimer, speed_val, self->range);
        } else {
            // Linearer Weg (Ping-Pong) absolut berechnet
            s32 total_way = self->range << 1;
            if (total_way > 0) {
                s32 pos = (vtimer * speed_val) % total_way;
                if (pos > self->range) pos = total_way - pos;
                offset_f32 = FIX32(pos);
            }
        }

        // Achsen zuweisen (Immer vom Origin aus, wie im alten Skript)
        self->ent.x_f32 = FIX32(self->origin_x << 3);
        self->ent.y_f32 = FIX32(self->origin_y << 3);

        if (CHECK_P_FLAG(self->flags, PLAT_FLAG_X)) self->ent.x_f32 += offset_f32;
        if (CHECK_P_FLAG(self->flags, PLAT_FLAG_Y)) self->ent.y_f32 += offset_f32;
    }

    // --- 3. PHYSIK SYNC (Deine Schreibweise aus dem alten Skript) ---
    self->ent.x = F32_toInt(self->ent.x_f32);
    self->ent.y = F32_toInt(self->ent.y_f32);
    
    // VX/VY Berechnung exakt wie in deinem Beispiel
    self->ent.vx = FIX16(self->ent.x - self->ent.x_old);
    self->ent.vy = FIX16(self->ent.y - self->ent.y_old);

    if (self->enabled && !CHECK_P_FLAG(self->flags, PLAT_FLAG_INVISIBLE)) {
        SPR_setPosition(self->ent.sprite, self->ent.x, self->ent.y);
    }

    self->touched = false;
}