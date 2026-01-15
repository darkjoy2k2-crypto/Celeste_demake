#include "player_controls.h"
#include "genesis.h"
#include "globals.h"
#include "fade.h"

static void handle_shot_jump_input(Player* p, u16 joy, u16 joy_old) {
    if ((joy & BTN_ACTION) && !(joy_old & BTN_ACTION) && p->state != P_SHOT_JUMP && p->count_shot_jump > 0) {
        p->ent.vx = F16_0;
        p->ent.vy = F16_0;
        p->timer_shot_jump = TIMER_SHOT_JUMP_MAX;
        p->count_shot_jump--; 
        if (p->count_shot_jump == 1)
            PAL_set_colors(PAL1, 1, COL_BALL_YELLOW, 3);
        if (p->count_shot_jump == 0)
            PAL_set_colors(PAL1, 1, COL_BALL_BLUE, 3);
        p->state = P_SHOT_JUMP;
    }

    if (p->state == P_SHOT_JUMP) {
        p->dontbreakjump = true;
        if (p->timer_shot_jump < 14) {
            bool up = (joy & BTN_UP), down = (joy & BTN_DOWN), left = (joy & BTN_LEFT), right = (joy & BTN_RIGHT);
            if (up && left) { p->ent.vx = -SHOT_JUMP_SPEED_DIAG; p->ent.vy = -SHOT_JUMP_SPEED_DIAG; }
            else if (up && right) { p->ent.vx = SHOT_JUMP_SPEED_DIAG; p->ent.vy = -SHOT_JUMP_SPEED_DIAG; }
            else if (down && left) { p->ent.vx = -SHOT_JUMP_SPEED_DIAG; p->ent.vy = SHOT_JUMP_SPEED_DIAG; }
            else if (down && right) { p->ent.vx = SHOT_JUMP_SPEED_DIAG; p->ent.vy = SHOT_JUMP_SPEED_DIAG; }
            else if (left) { p->ent.vx = -SHOT_JUMP_SPEED; p->ent.vy = F16_0; }
            else if (right) { p->ent.vx = SHOT_JUMP_SPEED; p->ent.vy = F16_0; }
            else if (up) { p->ent.vx = F16_0; p->ent.vy = -SHOT_JUMP_SPEED; }
            else { 
                p->state = P_FALLING; 
                p->timer_shot_jump = 0; 
                p->dontbreakjump = false;
            }
        }
        if (p->timer_shot_jump == 0) {
            p->state = P_FALLING;
            p->dontbreakjump = false;
        }
    }
}

static void handle_jump_input(Player* p, u16 joy, u16 joy_old) {
    if (joy & BTN_JUMP && !(joy_old & BTN_JUMP)) p->timer_buffer = TIMER_BUFFER_MAX;

    if (p->timer_buffer > 0) {
        if (p->state == P_GROUNDED || p->timer_grace > 0) {
            p->ent.vy = JUMP_FORCE;
            p->ent.vx += p->solid_vx;
            p->ent.vy += p->solid_vy;
            p->state = P_JUMPING;
            p->timer_buffer = 0; 
            p->timer_grace = 0;
            p->dontbreakjump = false;
        } 
        else if (p->state == P_ON_WALL) {
            p->ent.vy = JUMP_FORCE_WALL_V;
            if ((p->facing == 1 && (joy & BTN_LEFT)) || (p->facing == -1 && (joy & BTN_RIGHT))) {
                p->ent.vx = (p->facing == 1) ? -JUMP_FORCE_WALL_H : JUMP_FORCE_WALL_H;
            } else {
                p->ent.vx = (p->facing == 1) ? FIX16(-0.7) : FIX16(0.7);
            }
            p->state = P_JUMPING;
            p->timer_buffer = 0;
            p->dontbreakjump = false;
        }
    }
}

static void handle_horizontal_move(Player* p, u16 joy) {
    s16 dx = 0;
    if (joy & BTN_LEFT) dx = -1;
    else if (joy & BTN_RIGHT) dx = 1;

    if (p->state != P_ON_WALL && p->state != P_SHOT_JUMP) {
        if (dx != 0 && abs16(p->ent.vx) < MOVE_SPEED_MAX) {
            if (dx > 0) p->ent.vx += MOVE_SPEED;
            else p->ent.vx -= MOVE_SPEED;
            p->facing = dx;
        }
    }
}

void handle_player_input(Player* p) {
    if (p->is_dying) return;
    u16 joy = JOY_readJoypad(JOY_1);
    u16 joy_old = p->state_old_joy;

    if (p->timer_shot_jump > 0) p->timer_shot_jump--;

    handle_shot_jump_input(p, joy, joy_old);
    if (p->state == P_SHOT_JUMP) { p->state_old_joy = joy; return; }

    // Wir speichern uns den alten Zustand, um zu sehen, ob wir diesen Frame springen
    u8 state_before_jump = p->state;
    handle_jump_input(p, joy, joy_old);
    
    // Wenn wir gerade gesprungen sind, überspringen wir die restliche Logik (vor allem Climb)
    if (state_before_jump == P_ON_WALL && p->state == P_JUMPING) {
        p->state_old_joy = joy;
        return;
    }

    handle_horizontal_move(p, joy);
    
    // CLIMB LOGIK
    if (p->is_on_wall && (joy & BTN_DASH) && p->timer_stamina > 0) {
        p->state = P_ON_WALL;
        p->timer_stamina--;
        if (joy & BTN_UP) p->ent.vy = -CLIMB_SPEED;
        else if (joy & BTN_DOWN) p->ent.vy = CLIMB_SPEED;
        else p->ent.vy = F16_0; // Hier wurde der Jump-Impuls früher überschrieben!
    }

    // HELPING HOP (Kante)
    if (!p->is_on_wall && p->state == P_ON_WALL && (joy & BTN_UP)) {
        p->ent.vy = HELPING_HOP;
        p->ent.vx = FIX16(p->facing);
        p->state = P_FALLING;
    }
    
    // ABBRUCH CLIMB
    if (p->state == P_ON_WALL && (!(joy & BTN_DASH) || !p->is_on_wall || p->timer_stamina <= 0)) {
        p->state = P_FALLING;
    }

    // VARIABLE SPRUNGHÖHE
    if (!(joy & BTN_JUMP) && (p->state == P_JUMPING) && !p->dontbreakjump) {
        if (p->ent.vy < JUMP_RELEASE_LIMIT) p->ent.vy = JUMP_RELEASE_LIMIT;
    }

    p->state_old_joy = joy;
}