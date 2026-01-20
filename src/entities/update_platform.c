#include "entities/update_platform.h"
#include "globals.h"

void ENTITY_UPDATE_platform(Entity* self) {
    if (self->x_old_f32 == 0) {
        self->x_old_f32 = self->x_f32;
    }

    fix32 old_pos = self->x_f32;
    
    fix32 offset_f32 = getSinusValueF32(vtimer, 16, 48);
    
    self->x_f32 = self->x_old_f32 + offset_f32;
    self->vx = F32_toFix16(self->x_f32 - old_pos);
    self->x = F32_toInt(self->x_f32);
}