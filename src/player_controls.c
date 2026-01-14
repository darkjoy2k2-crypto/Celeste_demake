#include "player_controls.h"
#include "debug.h"
#include "entity_list.h"
#include "genesis.h"
#include "globals.h"
#include "debug.h"


void handle_player_input(Player* p) 
{
    if (p->is_dying) return;

    u16 joy_old = p->state_old_joy;
    u16 joy = JOY_readJoypad(JOY_1);

    // NEU: Stamina Reset am Boden
    if (p->state == P_GROUNDED) p->timer_stamina = 300;

    // Wall-Climb Einstieg
    if (p->is_on_wall && (joy & BUTTON_B) && p->timer_stamina > 0) {
        p->state = P_ON_WALL;
    }

    // Jump Input & Buffer
    if (joy & BTN_JUMP && !(joy_old & BTN_JUMP)) {
        p->timer_buffer = TIMER_BUFFER_MAX;
    }

    if (p->timer_buffer > 0) {
        bool can_jump = (p->state == P_GROUNDED || p->state == P_IDLE || p->state == P_RUNNING || p->timer_grace > 0);

        if (can_jump) {
            p->ent.vy = JUMP_FORCE;
            p->state = P_JUMPING;
            p->ent.vx += p->solid_vx;
            p->ent.vy += p->solid_vy;
            p->timer_buffer = 0;
            p->timer_grace = 0;
            p->dontbreakjump = false;
        } 
        else if (p->state == P_ON_WALL) {
            p->ent.vy = JUMP_FORCE_WALL_V;
            p->ent.vx = (p->facing == 1) ? -JUMP_FORCE_WALL_H : JUMP_FORCE_WALL_H;
            p->state = P_JUMPING;
            p->timer_buffer = 0;
        }
    }

    // Jump Release (Muss JUMPING und FALLING prüfen, falls man im Sprung loslässt)
    if (!(joy & BTN_JUMP) && (joy_old & BTN_JUMP)) {
        if ((p->state == P_JUMPING || p->state == P_FALLING) && !p->dontbreakjump) {
            if (p->ent.vy < JUMP_RELEASE_LIMIT) {
                p->ent.vy = JUMP_RELEASE_LIMIT;
            }
        }
    }

    // Bewegung
    s16 dx = 0;
    if (joy & BTN_LEFT) dx = -1;
    else if (joy & BTN_RIGHT) dx = 1;

    if (p->state == P_ON_WALL) {
        p->timer_stamina--; 
        if (joy & BTN_UP) p->ent.vy = -CLIMB_SPEED;
        else if (joy & BTN_DOWN) p->ent.vy = CLIMB_SPEED;
        else p->ent.vy = F16_0;

        if (!(joy & BUTTON_B) || !p->is_on_wall || p->timer_stamina <= 0) {
            p->state = P_FALLING;
        }
    } else {
        if (dx != 0 && abs16(p->ent.vx) < MOVE_SPEED_MAX) {
            if (dx > 0) p->ent.vx += MOVE_SPEED;
            else if (dx < 0) p->ent.vx -= MOVE_SPEED;
        }
    }

    p->state_old_joy = joy;
}