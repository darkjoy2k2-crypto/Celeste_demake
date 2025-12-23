#include <genesis.h>
#include "area.h"
#include "entity_list.h" // KORREKTUR: Alle Entity-Definitionen kommen von hier
#include "player_update.h" 
#include "debug.h"

// ====================================================================
// 1. PHYSIK-KONSTANTEN (ANGEPASST AN IHRE VORGABEN)
// ====================================================================

#define JUMP_VELOCITY_START FIX16(3)
#define JUMP_VELOCITY_START_GC FIX16(2.8)
#define EDGE_JUMP_START     FIX16(2.5)
#define HORIZONTAL_ACCEL    FIX16(0.8) // REDUZIERT
#define RUN_SPEED_MAX       FIX16(9) // REDUZIERT
#define FALL_SPEED_MAX      FIX16(10)
#define GRAVITY_JUMP        FIX16(0.15)
#define GRAVITY_FALL        FIX16(0.3)
#define CLIMB_SPEED         FIX16(1)
#define FLY_SPEED           FIX16(4)
#define FLY_SPEED_NORMED    F16_mul(FLY_SPEED, FIX16(0.707))

#define GROUND_FRICTION     FIX16(0.7) 
#define GRACE_PERIOD_FRAMES 5  

// ====================================================================
// 2. FORWARD DECLARATIONS
// ====================================================================

static void buffered_jump(Entity* player);
static void grace_jump(Entity* player);
static void joy_check_directions(Entity* player, u16 joy_state);
static void joy_check_falling(Entity* player, u16 joy_state);
static void joy_check_grab_directions(Entity* player, u16 joy_state);
static void check_grace_jump(Entity* player);
static void check_normal_jump(Entity* player, u16 joy_state);
static void edge_jump(Entity* player, u16 joy_state);
static void jump(Entity* player, fix16 intensity);
static void check_is_on_wall(Entity* player, u16 joy_state);
static void wall_move(Entity* player, u16 joy_state);

/**
 * @brief Die Haupt-Update-Funktion des Players (State Machine).
 * Definiert die gesamte Steuerlogik.
 *
 * @param player Pointer auf die Player-Entity.
 */
static void buffered_jump(Entity* player){
    if (player->state == P_GROUNDED && player->state_old == P_FALLING && player->timer_buffer > 0)
    {
        player->state = P_JUMPING;
        player->vy = -JUMP_VELOCITY_START; 
        player->timer_buffer = 0;
        VDP_drawText("BUFFERED JUMP", 0, 1);
    }
}

static void grace_jump(Entity* player){
    if (player->state_old == P_GROUNDED && player->state == P_FALLING)
    {
        player->timer_grace = GRACE_PERIOD_FRAMES;
    }
}

void joy_check_directions(Entity* player, u16 joy_state){
    if (joy_state & BUTTON_LEFT) {
        player->vx = player->vx - HORIZONTAL_ACCEL;
    } else if (joy_state & BUTTON_RIGHT) {
        player->vx = player->vx + HORIZONTAL_ACCEL;
    }
}

void joy_check_falling(Entity* player, u16 joy_state){
    if ((joy_state & BUTTON_MODE) && !(player->state_old_joy & BUTTON_MODE)){
    player->state = P_FLYING;
    }

    if (joy_state & BUTTON_LEFT) {
        player->vx -= HORIZONTAL_ACCEL;
    } else if (joy_state & BUTTON_RIGHT) {
        player->vx  = player->vx + HORIZONTAL_ACCEL;
    }
    if ((joy_state & BUTTON_A) && !(player->state_old_joy & BUTTON_A)){
        if (player->timer_grace > 0){
            VDP_drawText("GRACE JUMP", 0, 1);
            jump(player, FIX16(1));
        } 
        player->timer_buffer = GRACE_PERIOD_FRAMES;
    }
 

}

void joy_check_grab_directions(Entity* player, u16 joy_state){
        edge_jump(player, joy_state);

}

void check_grace_jump(Entity* player){if (player->timer_grace > 0) jump(player, FIX16(1)); }

void check_normal_jump(Entity* player, u16 joy_state){
    if ((joy_state & BUTTON_A) && !(player->state_old_joy & BUTTON_A))
        jump(player, FIX16(1));
}
    

void edge_jump(Entity* player, u16 joy_state){

    player->vy -= EDGE_JUMP_START;
    player->state = P_FALLING;
}

void jump(Entity* player, fix16 intensity){
    player->timer_edgegrab = 10;
    player->state = P_JUMPING;
    player->timer_buffer = 0;
    player->vy = F16_mul( -JUMP_VELOCITY_START, intensity); 
}

void check_is_on_wall(Entity* player, u16 joy_state){
    if (player->is_on_wall
        && player->timer_stamina > 0
    && (joy_state & BUTTON_B))
    {
        player->vx = F16_0;
        player->vy = F16_0; // Stoppt die Fall-Bewegung sofort
        player->state = P_ON_WALL;
    } 

}

void wall_move(Entity* player, u16 joy_state)
{  

    if ((joy_state & BUTTON_UP)){
        player->vy = -CLIMB_SPEED;player->timer_stamina -= 10;
    }
    else if ((joy_state & BUTTON_DOWN) ){
        player->vy = CLIMB_SPEED;player->timer_stamina -= 10;
    }
    else player->vy = F16_0;

    if (!(joy_state & BUTTON_B) || !player->is_on_wall){
            if (player->vy <= 0)
            player->vx = F16_mul(CLIMB_SPEED, FIX16(player->facing));
            player->state = P_FALLING;
    }
    if (joy_state & BUTTON_A && !(player->state_old_joy & BUTTON_A )){
        player->vx = F16_mul(JUMP_VELOCITY_START, FIX16(-player->facing));
        player->vy = -JUMP_VELOCITY_START;
        player->state = P_JUMPING;
    }
}

void check_for_shot(Entity* player, u16 joy_state){

    if ((joy_state & BUTTON_C) && !(player->state_old_joy & BUTTON_C)
    && player->timer_shot_jump == 0)
    
    {
            player->vx = F16_0;
            player->vy = F16_0;
            player->timer_shot_jump = 17;
            player->state = P_SHOT_JUMP;
    }

    if (player->timer_shot_jump < 14 && player->state == P_SHOT_JUMP){
        player->dontbreakjump = true;
        if ((joy_state & BUTTON_UP) && (joy_state & BUTTON_LEFT) ){
            player->vx = -FLY_SPEED_NORMED;
            player->vy = -FLY_SPEED_NORMED;
        } else

        if ((joy_state & BUTTON_UP) && (joy_state & BUTTON_RIGHT) ){
            player->vx = FLY_SPEED_NORMED;
            player->vy = -FLY_SPEED_NORMED;
        }else
        if ((joy_state & BUTTON_DOWN) && (joy_state & BUTTON_LEFT) ){
            player->vx = -FLY_SPEED_NORMED;
            player->vy = FLY_SPEED_NORMED;
        } else

        if ((joy_state & BUTTON_DOWN) && (joy_state & BUTTON_RIGHT) ){
            player->vx = FLY_SPEED_NORMED;
            player->vy = FLY_SPEED_NORMED;
        }else
        if ((joy_state & BUTTON_LEFT)){
            player->vy = F16_0;
            player->vx = -FLY_SPEED;
        }else
        if ((joy_state & BUTTON_RIGHT)){
            player->vy = F16_0;
            player->vx = FLY_SPEED;
        }else
        if ((joy_state & BUTTON_UP)){
            player->vx = F16_0;
            player->vy = -FLY_SPEED_NORMED;
        }else{
            player->state = P_FALLING;
            player->timer_shot_jump = 0;
            player->dontbreakjump = false;
        }
    if ((joy_state & BUTTON_A) && !(player->state_old_joy & BUTTON_A))
        jump(player, FIX16(1));

    }

}
void check_fly_controls(Entity* player, u16 joy_state){
    if (joy_state & BUTTON_UP){
        player->vy = player->vy - FLY_SPEED;
    }
    if (joy_state & BUTTON_DOWN){
        player->vy =  player->vy + FLY_SPEED;
    }
    if (joy_state & BUTTON_LEFT){
        player->vx = player->vx - FLY_SPEED;

    }
    if (joy_state & BUTTON_RIGHT){
        player->vx = player->vx + FLY_SPEED;
    }

    if ((joy_state & BUTTON_C) && !(player->state_old_joy & BUTTON_C)){
    player->state = P_FALLING;
    }
}


void update_player_state_and_physics(Entity* player) {

    u16 joy_state = JOY_readJoypad(JOY_1);
    
    // --- 1. GLOBALE INPUT- UND TIMER-VERARBEITUNG (Coyote Time, Jump Buffer) ---
    player->x_old_f32 = player->x_f32;
    player->y_old_f32 = player->y_f32;
    
    player->x_old = F32_toInt(player->x_f32);
    player->y_old = F32_toInt(player->y_f32);
    
    // Timer-Updates
    if (player->timer_grace > 0) player->timer_grace--;
    if (player->timer_buffer > 0) player->timer_buffer--;
    if (player->timer_edgegrab > 0) player->timer_edgegrab--;
    if (player->timer_wall > 0) player->timer_wall--;
    if (player->timer_shot_jump > 0) player->timer_shot_jump--;
    
    if (player->is_dying) {

            FADE_out(15);

            player->x = player->current_area->spawn.x << 3;
            player->y = player->current_area->spawn.y << 3 ;
            player->x_f32 = FIX32(player->x);
            player->y_f32 = FIX32(player->y);
            
            player->vx = F16_0;
            player->vy = F16_0;
            player->state = P_FALLING;
            player->trampolin = false;

            FADE_in(15);
            
            player->is_dying = false;
    }  

    switch (player->state) {
        case P_IDLE:
        case P_RUNNING:
        case P_GROUNDED:
            player->timer_stamina = 300;
            check_for_shot(player, joy_state);

            if (player->trampolin) {
                jump(player, FIX16(1.2));
                player->state = P_JUMPING;
                player->dontbreakjump = true;
                break;
            }else{
                player->dontbreakjump = false;
            }
            buffered_jump(player);
            joy_check_directions(player, joy_state);
            check_normal_jump(player, joy_state);
            check_grace_jump(player);

            player->vx = F16_mul(player->vx, GROUND_FRICTION);
            if (player->vy > F16_0) player->state = P_FALLING;
            break;

        case P_JUMPING:
            joy_check_directions(player, joy_state);
            player->vy = player->vy + GRAVITY_JUMP; 
            if (!(joy_state & BUTTON_A) && !player->dontbreakjump){
                player->vy = GRAVITY_FALL; 
            }
            if (player->vy > F16_0) player->state = P_FALLING;
            player->vx = F16_mul(player->vx, GROUND_FRICTION);
            check_for_shot(player, joy_state);

            check_is_on_wall(player, joy_state);
            break;

        case P_FALLING:
            if (player->trampolin) {
                jump(player, FIX16(1.2));
                player->state = P_JUMPING;
                player->dontbreakjump = true;
                break;
            }else{
                player->dontbreakjump = false;
            }



            joy_check_directions(player, joy_state);
            joy_check_falling(player, joy_state);
            grace_jump(player);
            player->vy = player->vy + GRAVITY_FALL;
            player->vx = F16_mul(player->vx, GROUND_FRICTION);
            //VDP_drawText("FALLING           ",0,1);
            check_for_shot(player, joy_state);
            check_is_on_wall(player, joy_state);

            break;
        case P_ON_WALL:
            wall_move(player, joy_state);
            player->timer_stamina--;
            if (player->timer_stamina < 0){
                player->state = P_FALLING;
            }
            break;    
        case P_EDGE_GRAB:

            joy_check_grab_directions(player, joy_state);
            player->state = P_FALLING;
            break;
        case P_SHOT_JUMP:
            check_for_shot(player, joy_state);
            if (player->timer_shot_jump == 0){
                player->state = P_FALLING;
            }
            break;
        case P_FLYING:
                player->vx = player->vy = F16_0;

                check_fly_controls(player, joy_state);
            break;
        default:
            break;
    }

    if (player->vx > RUN_SPEED_MAX) player->vx = RUN_SPEED_MAX;
    if (player->vx < -RUN_SPEED_MAX) player->vx = -RUN_SPEED_MAX;
    if (player->vy > FALL_SPEED_MAX) player->vy = FALL_SPEED_MAX;
   
    player->state_old_joy = joy_state;
    
    player->state_old = player->state;


}

