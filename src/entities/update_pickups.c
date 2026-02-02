#include "entities/update_platform.h"
#include "globals.h"

void ENTITY_UPDATE_pickup(Entity* _e) {
    if (!GameSync) return;
    Pickup* self = (Pickup*)_e;

    // --- 2. BEWEGUNG ---
    self->ent.x_old = self->ent.x;
    self->ent.y_old = self->ent.y;

        // POSITIONS-UPDATE
        fix32 base_x = FIX32(self->ent.x);
        fix32 base_y = FIX32(self->ent.y);

        self->ent.x = F32_toInt(self->ent.x_f32);
        self->ent.y = F32_toInt(self->ent.y_f32);
    

    // --- 3. PHYSIK & SPRITE ---
    self->ent.vx = FIX16(self->ent.x - self->ent.x_old);
    self->ent.vy = FIX16(self->ent.y - self->ent.y_old);

    SPR_setPosition(self->ent.sprite, self->ent.x, self->ent.y);
}