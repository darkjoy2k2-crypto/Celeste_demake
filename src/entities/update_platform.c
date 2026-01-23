#include "entities/update_platform.h"
#include "globals.h"

void ENTITY_UPDATE_platform(Entity* _e) {
    Platform* self = (Platform*)_e;

    /* 1. Timer-Logik */
    if (self->touched && self->wait_timer == 0) {
        // Starte Timer nur, wenn er nicht schon läuft
        self->wait_timer = 300;
    }

    if (self->wait_timer > 0) {
        self->wait_timer--;

        // Bei 1620: Animation auf 2 (Index 1) und Kollision aus
        if (self->wait_timer <= 240) {
            self->enabled = false;
        }

        // Bei 0: Reset auf Animation 1 (Index 0) und Kollision an
        if (self->wait_timer == 0) {
            self->enabled = true;
        }
    }

    /* 2. Bewegung (nur wenn enabled) */
    self->ent.x_old_f32 = self->ent.x_f32;
    self->ent.y_old_f32 = self->ent.y_f32;
    self->ent.x_old = self->ent.x;
    self->ent.y_old = self->ent.y;

    // Nur bewegen/berechnen, wenn die Plattform aktiv ist
        switch (self->behavior) {
            case PB_SINUS_WIDE_Y:
            case PB_SINUS_Y: {
                s16 speed_val = F16_toInt(self->speed);
                fix32 offset_f32 = getSinusValueF32(vtimer, speed_val, 48);
                self->ent.y_f32 = FIX32(self->origin_y << 3) + offset_f32;
                self->ent.x_f32 = FIX32(self->origin_x << 3);
            } break;

            case PB_SINUS_WIDE_X:
            case PB_SINUS_X: {
                s16 speed_val = F16_toInt(self->speed);
                fix32 offset_f32 = getSinusValueF32(vtimer, speed_val, 48);
                self->ent.x_f32 = FIX32(self->origin_x << 3) + offset_f32;
                self->ent.y_f32 = FIX32(self->origin_y << 3);
            } break;
            // ... PB_LINEAR etc ...
        }


    /* 3. Physische Werte syncen */
    self->ent.x = F32_toInt(self->ent.x_f32);
    self->ent.y = F32_toInt(self->ent.y_f32);
    self->ent.vx = FIX16(self->ent.x - self->ent.x_old);
    self->ent.vy = FIX16(self->ent.y - self->ent.y_old);

    /* 4. Reset Touched für den nächsten Frame */
    self->touched = false;
}