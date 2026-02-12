#include "entities/update_platform.h"
#include "globals.h"

void ENTITY_UPDATE_platform(Entity* _e) {
    if (!GameSync) return;
    Platform* self = (Platform*)_e;

    // =========================================================================
    // SEKTION 0: INITIALISIERUNG
    // =========================================================================
    // Wir prüfen nur das oberste Bit. Wenn es 0 ist, initialisieren wir.
    if (!(self->status_bits & 0x8000)) { 
        s32 dx = (s32)self->target_x - (s32)self->origin_x;
        s32 dy = (s32)self->target_y - (s32)self->origin_y;
        s32 dist_sq = (dx * dx) + (dy * dy);

        self->dir_x = F16_0;
        self->dir_y = F16_0;

        if (dist_sq > 0) {
            fix16 f_dist_sq = FIX16(dist_sq); 
            if (f_dist_sq < 0) f_dist_sq = 0x7FFF; 

            fix16 dist = F16_sqrt(f_dist_sq);
            
            if (dist > F16_0) {
                self->dir_x = F16_div(FIX16(dx), dist);
                self->dir_y = F16_div(FIX16(dy), dist);
            }
        }

        // Positionen einmalig festlegen
        self->ent.x_f32 = FIX32(self->origin_x);
        self->ent.y_f32 = FIX32(self->origin_y);
        self->ent.x = self->origin_x;
        self->ent.y = self->origin_y;
        self->ent.x_old = self->origin_x;
        self->ent.y_old = self->origin_y;

        self->enabled = true;
        self->ent.spr_visible = true;
        self->anim_timer_x = FIX16(0);
        
        // WICHTIG: Wir setzen NUR das Init-Bit dazu, anstatt alles zu löschen!
        self->status_bits |= 0x8000; 

        if (self->ent.sprite) SPR_setAnim(self->ent.sprite, 0);
    }

    // =========================================================================
    // SEKTION 1: STATUS-MASCHINE (Zerbrechliche Plattformen)
    // =========================================================================
    switch (self->state) {
        case PLAT_IDLE:
            if (self->touched) {
                // Aktivierung für bewegliche Plattformen (Bit 0x0001)
                if (CHECK_P_FLAG(self->flags, PLAT_FLAG_TOUCH_START)) {
                    self->status_bits |= 0x0001;
                }
                // Logik für zerbrechliche Plattformen
                if (CHECK_P_FLAG(self->flags, PLAT_FLAG_BREAKABLE)) {
                    self->wait_timer = 60;
                    self->state = PLAT_BREAKING;
                }
            }
            break;

        case PLAT_BREAKING:
            if (self->wait_timer > 0) {
                self->wait_timer--;
                // Flackereffekt vor dem Zerbrechen
                self->ent.spr_visible = (vtimer % 20 < 10);
            } else {
                self->enabled = false;
                self->ent.spr_visible = false;
                if (self->ent.sprite) SPR_setAnim(self->ent.sprite, 2);
                
                self->state = CHECK_P_FLAG(self->flags, PLAT_FLAG_RESPAWN) ? PLAT_HIDDEN : PLAT_DISABLED;
                if (self->state == PLAT_HIDDEN) self->wait_timer = 240;
            }
            break;

        case PLAT_HIDDEN:
            if (self->wait_timer > 0) {
                self->wait_timer--;
            } else { 
                self->enabled = true; 
                self->ent.spr_visible = true;
                if (self->ent.sprite) SPR_setAnim(self->ent.sprite, 0);
                self->state = PLAT_IDLE; 
            }
            break;

        default: break;
    }

    // =========================================================================
    // SEKTION 2: BEWEGUNG
    // =========================================================================
    self->ent.x_old = self->ent.x;
    self->ent.y_old = self->ent.y;

    // Nur bewegen, wenn kein Touch-Start-Zwang aktiv ODER wenn bereits berührt (Bit 0x0001)
    bool move_active = !CHECK_P_FLAG(self->flags, PLAT_FLAG_TOUCH_START) || (self->status_bits & 0x0001);

    if (self->enabled && move_active) {
        if (self->amplitude > 0) {
            self->amplitude--;
        } else {
            fix16 current_speed = self->speed;
            
            if (CHECK_P_FLAG(self->flags, PLAT_FLAG_SINUS_X) || CHECK_P_FLAG(self->flags, PLAT_FLAG_SINUS_Y)) {
                u16 angle = F16_toRoundedInt(self->anim_timer_x) & 1023;
                current_speed = F16_mul(self->speed, sinFix16(angle));
                self->anim_timer_x += FIX16(2); 
            }

            bool is_returning = (self->status_bits & 0x0002);
            fix16 vx = is_returning ? -self->dir_x : self->dir_x;
            fix16 vy = is_returning ? -self->dir_y : self->dir_y;

            self->ent.x_f32 += F16_toFix32(F16_mul(vx, current_speed));
            self->ent.y_f32 += F16_toFix32(F16_mul(vy, current_speed));
            
            self->ent.x = F32_toRoundedInt(self->ent.x_f32);
            self->ent.y = F32_toRoundedInt(self->ent.y_f32);

            s16 tx = is_returning ? self->origin_x : self->target_x;
            s16 ty = is_returning ? self->origin_y : self->target_y;
            s16 tol = F16_toRoundedInt(current_speed) + 1;

            if (abs(self->ent.x - tx) <= tol && abs(self->ent.y - ty) <= tol) {
                self->ent.x = tx; 
                self->ent.y = ty;
                self->ent.x_f32 = FIX32(tx); 
                self->ent.y_f32 = FIX32(ty);
                self->anim_timer_x = FIX16(0);
                
                if (is_returning) {
                    self->status_bits &= ~0x0002;
                    self->status_bits &= ~0x0001; // Erlaubt erneutes Aktivieren durch Touch
                } else {
                    self->status_bits |= 0x0002;
                    self->amplitude = 30; // Pause am Ziel
                }
            }
        }
    }

    // =========================================================================
    // SEKTION 3: PHYSIK-OUTPUT
    // =========================================================================
    self->ent.vx = self->enabled ? FIX16(self->ent.x - self->ent.x_old) : F16_0;
    self->ent.vy = self->enabled ? FIX16(self->ent.y - self->ent.y_old) : F16_0;

    if (self->ent.sprite) {
        SPR_setPosition(self->ent.sprite, self->ent.x, self->ent.y);
    }
    
    // WICHTIG:Touched-Status für den Handler zurücksetzen
    self->touched = false;
}