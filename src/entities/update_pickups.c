#include "entities/update_platform.h"
#include "globals.h"

void ENTITY_UPDATE_pickup(Entity* _e) {
    if (!GameSync) return;
    Pickup* self = (Pickup*)_e;

    if (self->kind == PICKUP_BALLOON) {
        Player* p = (Player*) entities[player_id];
        
        // Reset Logik (wie gehabt)
        if (p->state_old != P_GROUNDED && p->state == P_GROUNDED && self->collected) {
            self->collected = false;
            SPR_setVisibility(self->ent.sprite, VISIBLE);
        }

        if (self->collected) return;

        // --- OPTIMIERTE BEWEGUNG ---
        // Sinus-Geschwindigkeit: Wir schieben den vtimer bitweise (schneller als Mult)
        // sinFix32 nutzt intern eine Tabelle, das ist okay.
        fix32 float_offset = sinFix32(vtimer << 4); 
        
        // Amplitude: * 4 ist einfach ein Shift um 2 nach links (schnell)
        s16 draw_y = F32_toInt(self->ent.y_f32) + F32_toInt(float_offset >> 1);

        // --- OPTIMIERTER FLIP ---
        // Wir prüfen Bit 6 des vtimers (0100 0000 = 64). 
        // Das Bit wechselt alle 64 Frames von 0 auf 1.
        if (vtimer & 64) {
            SPR_setHFlip(self->ent.sprite, TRUE);
        } else {
            SPR_setHFlip(self->ent.sprite, FALSE);
        }

        self->ent.x = F32_toInt(self->ent.x_f32);
        self->ent.y = draw_y; 

        SPR_setPosition(self->ent.sprite, self->ent.x, self->ent.y);
    }
}

void ENTITY_TRIGGER_pickup(Entity* _e) {
    Pickup* self = (Pickup*)_e;
    SPR_setVisibility(self->ent.sprite, HIDDEN);
    self->collected = true;

    Player* p = (Player*) entities[player_id];
    p->count_shot_jump++;
}
