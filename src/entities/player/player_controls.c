#include "entities/player/player_controls.h"
#include "genesis.h"
#include "globals.h"
#include "fade.h"
#include "debug.h"

/* =============================================================================
   SHOT JUMP INPUT (Rückstoß-Mechanik)
   ============================================================================= */
static void handle_shot_jump_input(Player* p, u16 joy, u16 joy_old) {
    /* "Just Pressed" Erkennung für den Schuss-Knopf */
    if ((joy & button_map[ACTION_SHOT]) && !(joy_old & button_map[ACTION_SHOT]) && 
        p->state != P_SHOT_JUMP && p->count_shot_jump > 0) {
        
        p->ent.vx = F16_0;
        p->ent.vy = F16_0;
        p->timer_shot_jump = TIMER_SHOT_JUMP_MAX;
        p->count_shot_jump--; 

        /* Farb-Feedback basierend auf verbleibender Munition */
        if (p->count_shot_jump == 1)
            PAL_set_colors(PAL1, 1, COL_BALL_YELLOW, 3);
        if (p->count_shot_jump == 0)
            PAL_set_colors(PAL1, 1, COL_BALL_BLUE, 3);
            
        p->state = P_SHOT_JUMP;
    }

    if (p->state == P_SHOT_JUMP) {
        SET_P_FLAG(p->physics_state, P_FLAG_DONT_BREAK);
        
        if (p->timer_shot_jump < 14) {
            /* Richtungsabfrage über das neue Button-Mapping */
            bool up    = (joy & button_map[ACTION_UP]);
            bool down  = (joy & button_map[ACTION_DOWN]);
            bool left  = (joy & button_map[ACTION_LEFT]);
            bool right = (joy & button_map[ACTION_RIGHT]);

            /* Diagonale Schüsse */
            if (up && left)        { p->ent.vx = -SHOT_JUMP_SPEED_DIAG; p->ent.vy = -SHOT_JUMP_SPEED_DIAG; }
            else if (up && right)  { p->ent.vx = SHOT_JUMP_SPEED_DIAG;  p->ent.vy = -SHOT_JUMP_SPEED_DIAG; }
            else if (down && left) { p->ent.vx = -SHOT_JUMP_SPEED_DIAG; p->ent.vy = SHOT_JUMP_SPEED_DIAG; }
            else if (down && right){ p->ent.vx = SHOT_JUMP_SPEED_DIAG;  p->ent.vy = SHOT_JUMP_SPEED_DIAG; }
            /* Kardinale Schüsse */
            else if (left)         { p->ent.vx = -SHOT_JUMP_SPEED;      p->ent.vy = F16_0; }
            else if (right)        { p->ent.vx = SHOT_JUMP_SPEED;       p->ent.vy = F16_0; }
            else if (up)           { p->ent.vx = F16_0;                 p->ent.vy = -SHOT_JUMP_SPEED; }
            /* Abbruch wenn keine Richtung gedrückt */
            else { 
                p->state = P_FALLING; 
                p->timer_shot_jump = 0; 
                CLEAR_P_FLAG(p->physics_state, P_FLAG_DONT_BREAK);
            }
        }
        
        if (p->timer_shot_jump == 0) {
            p->state = P_FALLING;
            CLEAR_P_FLAG(p->physics_state, P_FLAG_DONT_BREAK);
        }
    }
}

/* =============================================================================
   JUMP INPUT (Sprung & Walljump)
   ============================================================================= */
static void handle_jump_input(Player* p, u16 joy, u16 joy_old) {
    /* Input Buffer für flüssigeres Springen */
    if (joy & button_map[ACTION_JUMP] && !(joy_old & button_map[ACTION_JUMP])) 
        p->timer_buffer = TIMER_BUFFER_MAX;

    if (p->timer_buffer > 0) {
        /* Normaler Sprung oder Coyote Time */
        if (p->state == P_GROUNDED || p->timer_grace > 0) {
            p->ent.vy = JUMP_FORCE;


            p->ent.vx += F16_mul(p->solid_vx, FIX16(2));
            p->ent.vy += F16_mul(p->solid_vy, FIX16(2));
            

               CLEAR_P_FLAG(p->physics_state, P_FLAG_DONT_BREAK);


            p->state = P_JUMPING;
            p->timer_buffer = 0; 
            p->timer_grace = 0;
        } 
        /* Walljump Logik */
        else if (p->state == P_ON_WALL) {
            p->ent.vy = JUMP_FORCE_WALL_V;
            bool left  = (joy & button_map[ACTION_LEFT]);
            bool right = (joy & button_map[ACTION_RIGHT]);
            
            /* Kick-Away vom der Wand oder vertikaler Climb-Jump */
            if ((CHECK_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT) && right) || 
                (!CHECK_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT) && left)) {
                p->ent.vx = (CHECK_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT)) ? JUMP_FORCE_WALL_H : -JUMP_FORCE_WALL_H;
            } else {
                p->ent.vx = (CHECK_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT)) ? FIX16(0.7) : FIX16(-0.7);
            }
            p->state = P_JUMPING;
            p->timer_buffer = 0;
            CLEAR_P_FLAG(p->physics_state, P_FLAG_DONT_BREAK);
        }
    }
}

/* =============================================================================
   HORIZONTAL MOVE
   ============================================================================= */
static void handle_horizontal_move(Player* p, u16 joy) {
    // Wenn wir steppen, darf diese Logik VX NICHT überschreiben!
    if (CHECK_P_FLAG(p->physics_state, P_FLAG_STEPPING)) return;

    if (p->state != P_ON_WALL && p->state != P_SHOT_JUMP) {
        if (joy & button_map[ACTION_LEFT]) {
            if (p->ent.vx > -MOVE_SPEED_MAX) p->ent.vx -= MOVE_SPEED;
            SET_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT);
        }
        else if (joy & button_map[ACTION_RIGHT]) {
            if (p->ent.vx < MOVE_SPEED_MAX) p->ent.vx += MOVE_SPEED;
            CLEAR_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT);
        }
        else {
            // Diese Reibung hat dein VX = 3 bisher sofort auf 0 gesetzt:
            fix16 friction = (p->state == P_GROUNDED) ? FRICTION_AIR : FRICTION;
            p->ent.vx = F16_mul(p->ent.vx, friction);
            if (abs16(p->ent.vx) < FIX16(0.1)) p->ent.vx = F16_0;
        }
    }
}

/* =============================================================================
   MAIN INPUT HANDLER
   ============================================================================= */
void handle_player_input(Player* p) {
    if (CHECK_P_FLAG(p->physics_state, P_FLAG_DYING)) return;
    
    u16 joy = JOY_readJoypad(JOY_1);
    u16 joy_old = p->state_old_joy;

    if (p->timer_shot_jump > 0) p->timer_shot_jump--;

    handle_shot_jump_input(p, joy, joy_old);
    if (p->state == P_SHOT_JUMP) { p->state_old_joy = joy; return; }

    u8 state_before_jump = p->state;
    handle_jump_input(p, joy, joy_old);
    
    // Falls ein Walljump stattgefunden hat, hier abbrechen
    if (state_before_jump == P_ON_WALL && p->state == P_JUMPING) {
        p->state_old_joy = joy;
        return;
    }

    bool is_at_wall = CHECK_P_FLAG(p->physics_state, P_FLAG_ON_WALL);
    bool is_holding_dash = (joy & button_map[ACTION_DASH]);

    if (is_at_wall && is_holding_dash && p->timer_stamina > 0) {
        p->state = P_ON_WALL;
        p->timer_stamina--;

        if (joy & button_map[ACTION_UP]) {
            if (p->ent.vy > -CLIMB_SPEED) p->ent.vy -= FIX16(0.5);
            if (p->ent.vy < -MOVE_SPEED_MAX) p->ent.vy = -MOVE_SPEED_MAX;
        }
        else if (joy & button_map[ACTION_DOWN]) {
            if (p->ent.vy < CLIMB_SPEED) p->ent.vy += FIX16(0.5);
        }
        else {
            if (p->ent.vy < F16_0)
                p->ent.vy = F16_mul(p->ent.vy, WALL_FRICTION);

            if (abs16(p->ent.vy) <= FIX16(0.1)) p->ent.vy = F16_0;
        }
        p->ent.vx = F16_0;
        p->ent.vy += FIX16(0.15); 
    }
    else {
        // State-Wechsel von Wall zu Air abfangen
        if (p->state == P_ON_WALL) p->state = P_FALLING;

        // EXKLUSIVER LEDGE-HOP CHECK
        if (p->state_old == P_ON_WALL && p->state == P_FALLING 
            && is_holding_dash && (joy & button_map[ACTION_UP])) {

            p->ent.y = p->ent.y - 4; // Impuls nach oben
            p->ent.y_f32 = FIX32(p->ent.y); // Impuls nach oben
            p->ent.y_old = p->ent.y;
            p->ent.y_old_f32 = p->ent.y_f32;

            p->ent.vx += CHECK_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT) ? FIX16(-3) : FIX16(3);
            
            p->state = P_FALLING;
            SET_P_FLAG(p->physics_state, P_FLAG_STEPPING);
        }
        
        // Horizontal Move wird innerhalb der Funktion durch das Flag geschützt
        handle_horizontal_move(p, joy);
    }
    
    // Jump-Release Logik (nur wenn kein Stepping aktiv ist, um den Hop nicht zu kappen)
    if (!(joy & button_map[ACTION_JUMP]) && (p->state == P_JUMPING) && 
        !CHECK_P_FLAG(p->physics_state, P_FLAG_DONT_BREAK) &&
        !CHECK_P_FLAG(p->physics_state, P_FLAG_STEPPING)) {
        if (p->ent.vy < JUMP_RELEASE_LIMIT) p->ent.vy = JUMP_RELEASE_LIMIT;
    }

    p->state_old_joy = joy;
}