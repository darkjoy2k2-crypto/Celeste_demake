#include "entities/update_platform.h"
#include "globals.h"

void ENTITY_UPDATE_platform(Entity* _e) {
    Platform* self = (Platform*)_e;

    self->ent.x_old_f32 = self->ent.x_f32;
    self->ent.y_old_f32 = self->ent.y_f32;
    self->ent.x_old = self->ent.x;
    self->ent.y_old = self->ent.y;

    switch (self->behavior) {
        case PB_SINUS_WIDE_Y:
        case PB_SINUS_Y:
        {
            s16 speed_val = F16_toInt(self->speed);
            fix32 offset_f32 = getSinusValueF32(vtimer, speed_val, 48);
            
            self->ent.y_f32 = FIX32(self->origin_y << 3) + offset_f32;
            self->ent.x_f32 = FIX32(self->origin_x << 3);
        }
        break;

        case PB_SINUS_WIDE_X:
        case PB_SINUS_X:
        {
            s16 speed_val = F16_toInt(self->speed);
            fix32 offset_f32 = getSinusValueF32(vtimer, speed_val, 48);
            
            self->ent.x_f32 = FIX32(self->origin_x << 3) + offset_f32;
            self->ent.y_f32 = FIX32(self->origin_y << 3);
        }
        break;

        case PB_LINEAR:
        {
            self->ent.x_f32 = self->ent.x_f32 + F16_toFix32(self->speed);
            self->ent.y_f32 = self->ent.y_f32;
        }
        break;
    }

    self->ent.x = F32_toInt(self->ent.x_f32);
    self->ent.y = F32_toInt(self->ent.y_f32);

    self->ent.vx = FIX16(self->ent.x - self->ent.x_old);
    self->ent.vy = FIX16(self->ent.y - self->ent.y_old);

}