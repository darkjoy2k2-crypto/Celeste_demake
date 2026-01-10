#include <genesis.h>
#include "area.h"
#include "entity_list.h"
#include "player_update.h" 
#include "debug.h"
#include "fade.h"
#include "globals.h"

#define JUMP_VELOCITY_START FIX16(3)
#define JUMP_VELOCITY_START_GC FIX16(2.8)
#define EDGE_JUMP_START     FIX16(2.5)
#define HORIZONTAL_ACCEL    FIX16(0.8) 
#define RUN_SPEED_MAX       FIX16(9) 
#define FALL_SPEED_MAX      FIX16(10)
#define GRAVITY_JUMP        FIX16(0.15)
#define GRAVITY_FALL        FIX16(0.3)
#define CLIMB_SPEED         FIX16(1)
#define FLY_SPEED           FIX16(4)
#define FLY_SPEED_NORMED    F16_mul(FLY_SPEED, FIX16(0.707))
#define GROUND_FRICTION     FIX16(0.7) 
#define GRACE_PERIOD_FRAMES 5  

static void buffered_jump(Player* player);
static void grace_jump(Player* player);
static void joy_check_directions(Player* player, u16 joy_state);
static void joy_check_falling(Player* player, u16 joy_state);
static void joy_check_grab_directions(Player* player, u16 joy_state);
static void check_grace_jump(Player* player);
static void check_normal_jump(Player* player, u16 joy_state);
static void edge_jump(Player* player, u16 joy_state);
static void jump(Player* player, fix16 intensity);
static void check_is_on_wall(Player* player, u16 joy_state);
static void wall_move(Player* player, u16 joy_state);
static void check_for_shot(Player* player, u16 joy_state);
static void check_fly_controls(Player* player, u16 joy_state);

static void buffered_jump(Player* player){
    if (player->state == P_GROUNDED && player->state_old == P_FALLING && player->timer_buffer > 0)
    {
        player->state = P_JUMPING;
        player->ent.vy = -JUMP_VELOCITY_START; 
        player->timer_buffer = 0;
    }
}

static void grace_jump(Player* player){
    if (player->state_old == P_GROUNDED && player->state == P_FALLING)
    {
        player->timer_grace = GRACE_PERIOD_FRAMES;
    }
}

static void joy_check_directions(Player* player, u16 joy_state){
    if (joy_state & BUTTON_LEFT) {
        player->ent.vx = player->ent.vx - HORIZONTAL_ACCEL;
    } else if (joy_state & BUTTON_RIGHT) {
        player->ent.vx = player->ent.vx + HORIZONTAL_ACCEL;
    }
}

static void joy_check_falling(Player* player, u16 joy_state){
    if ((joy_state & BUTTON_MODE) && !(player->state_old_joy & BUTTON_MODE)){
        player->state = P_FLYING;
        return;
    }

    if (joy_state & BUTTON_LEFT) {
        player->ent.vx -= HORIZONTAL_ACCEL;
    } else if (joy_state & BUTTON_RIGHT) {
        player->ent.vx = player->ent.vx + HORIZONTAL_ACCEL;
    }
    if ((joy_state & BUTTON_A) && !(player->state_old_joy & BUTTON_A)){
        if (player->timer_grace > 0){
            jump(player, FIX16(1));
        } 
        player->timer_buffer = GRACE_PERIOD_FRAMES;
    }
}

static void joy_check_grab_directions(Player* player, u16 joy_state){
    edge_jump(player, joy_state);
}

static void check_grace_jump(Player* player){
    if (player->timer_grace > 0) jump(player, FIX16(1)); 
}

static void check_normal_jump(Player* player, u16 joy_state){
    if ((joy_state & BUTTON_A) && !(player->state_old_joy & BUTTON_A))
        jump(player, FIX16(1));
}

static void edge_jump(Player* player, u16 joy_state){
    player->ent.vy -= EDGE_JUMP_START;
    player->state = P_FALLING;
}

static void jump(Player* player, fix16 intensity){
    player->timer_edgegrab = 10;
    player->state = P_JUMPING;
    player->timer_buffer = 0;
    player->ent.vy = F16_mul(-JUMP_VELOCITY_START, intensity); 
}

static void check_is_on_wall(Player* player, u16 joy_state){
    if (player->is_on_wall && player->timer_stamina > 0 && (joy_state & BUTTON_B))
    {
        player->ent.vx = F16_0;
        player->ent.vy = F16_0; 
        player->state = P_ON_WALL;
    } 
}

static void wall_move(Player* player, u16 joy_state)
{  
    if ((joy_state & BUTTON_UP)){
        player->ent.vy = -CLIMB_SPEED; player->timer_stamina -= 10;
    }
    else if ((joy_state & BUTTON_DOWN) ){
        player->ent.vy = CLIMB_SPEED; player->timer_stamina -= 10;
    }
    else player->ent.vy = F16_0;

    if (!(joy_state & BUTTON_B) || !player->is_on_wall){
        if (player->ent.vy <= 0)
            player->ent.vx = F16_mul(CLIMB_SPEED, FIX16(player->facing));
        player->state = P_FALLING;
    }
    if (joy_state & BUTTON_A && !(player->state_old_joy & BUTTON_A )){
        player->ent.vx = F16_mul(JUMP_VELOCITY_START, FIX16(-player->facing));
        player->ent.vy = -JUMP_VELOCITY_START;
        player->state = P_JUMPING;
    }
}

static void check_for_shot(Player* player, u16 joy_state){
    if ((joy_state & BUTTON_C) && !(player->state_old_joy & BUTTON_C) && player->timer_shot_jump == 0)
    {
        player->ent.vx = F16_0;
        player->ent.vy = F16_0;
        player->timer_shot_jump = 17;
        player->state = P_SHOT_JUMP;
    }

    if (player->timer_shot_jump < 14 && player->state == P_SHOT_JUMP){
        player->dontbreakjump = true;
        if ((joy_state & BUTTON_UP) && (joy_state & BUTTON_LEFT) ){
            player->ent.vx = -FLY_SPEED_NORMED; player->ent.vy = -FLY_SPEED_NORMED;
        } else if ((joy_state & BUTTON_UP) && (joy_state & BUTTON_RIGHT) ){
            player->ent.vx = FLY_SPEED_NORMED; player->ent.vy = -FLY_SPEED_NORMED;
        } else if ((joy_state & BUTTON_DOWN) && (joy_state & BUTTON_LEFT) ){
            player->ent.vx = -FLY_SPEED_NORMED; player->ent.vy = FLY_SPEED_NORMED;
        } else if ((joy_state & BUTTON_DOWN) && (joy_state & BUTTON_RIGHT) ){
            player->ent.vx = FLY_SPEED_NORMED; player->ent.vy = FLY_SPEED_NORMED;
        } else if ((joy_state & BUTTON_LEFT)){
            player->ent.vy = F16_0; player->ent.vx = -FLY_SPEED;
        } else if ((joy_state & BUTTON_RIGHT)){
            player->ent.vy = F16_0; player->ent.vx = FLY_SPEED;
        } else if ((joy_state & BUTTON_UP)){
            player->ent.vx = F16_0; player->ent.vy = -FLY_SPEED_NORMED;
        } else {
            player->state = P_FALLING;
            player->timer_shot_jump = 0;
            player->dontbreakjump = false;
        }
        if ((joy_state & BUTTON_A) && !(player->state_old_joy & BUTTON_A))
            jump(player, FIX16(1));
    }
}

static void check_fly_controls(Player* player, u16 joy_state){
    if (joy_state & BUTTON_UP) player->ent.vy -= FLY_SPEED;
    if (joy_state & BUTTON_DOWN) player->ent.vy += FLY_SPEED;
    if (joy_state & BUTTON_LEFT) player->ent.vx -= FLY_SPEED;
    if (joy_state & BUTTON_RIGHT) player->ent.vx += FLY_SPEED;

    if ((joy_state & BUTTON_C) && !(player->state_old_joy & BUTTON_C)){
        player->state = P_FALLING;
    }
}

void update_player_state_and_physics(Entity* entity) {
    Player* player = (Player*) entity;
    u16 joy_state = JOY_readJoypad(JOY_1);
    
    player->ent.x_old_f32 = player->ent.x_f32;
    player->ent.y_old_f32 = player->ent.y_f32;
    player->ent.x_old = F32_toInt(player->ent.x_f32);
    player->ent.y_old = F32_toInt(player->ent.y_f32);
    
    if (player->timer_grace > 0) player->timer_grace--;
    if (player->timer_buffer > 0) player->timer_buffer--;
    if (player->timer_edgegrab > 0) player->timer_edgegrab--;
    if (player->timer_shot_jump > 0) player->timer_shot_jump--;
    
    if (player->is_dying) {
        player->timer_death = 30;
        FADE_out(15);
        player->ent.x = player->current_area->spawn.x << 3;
        player->ent.y = player->current_area->spawn.y << 3;
        player->ent.x_f32 = FIX32(player->ent.x);
        player->ent.y_f32 = FIX32(player->ent.y);
        player->ent.vx = F16_0;
        player->ent.vy = F16_0;
        player->state = P_FALLING;
        player->trampolin = false;
        FADE_in(15);
        player->is_dying = false;
    }  

    switch (player->state) {
        case P_IDLE:
        case P_RUNNING:
        case P_GROUNDED:
            if (player->ent.vy < FIX16(0.5)) player->ent.vy = FIX16(0.5);            player->timer_stamina = 300;
            check_for_shot(player, joy_state);
            if (player->trampolin) {
                jump(player, FIX16(1.2));
                player->state = P_JUMPING;
                player->dontbreakjump = true;
                break;
            } else {
                player->dontbreakjump = false;
            }
            buffered_jump(player);
            joy_check_directions(player, joy_state);
            check_normal_jump(player, joy_state);
            check_grace_jump(player);

            player->ent.vx = F16_mul(player->ent.vx, GROUND_FRICTION);

            if (player->ent.vy > FIX16(1)) player->state = P_FALLING;
            break;

        case P_JUMPING:
            joy_check_directions(player, joy_state);
            player->ent.vy += GRAVITY_JUMP; 
            if (!(joy_state & BUTTON_A) && !player->dontbreakjump) player->ent.vy = GRAVITY_FALL; 
            if (player->ent.vy > F16_0) player->state = P_FALLING;
            player->ent.vx = F16_mul(player->ent.vx, GROUND_FRICTION);
            check_for_shot(player, joy_state);
            check_is_on_wall(player, joy_state);
            break;

        case P_FALLING:
            if (player->trampolin) {
                jump(player, FIX16(1.2));
                player->state = P_JUMPING;
                player->dontbreakjump = true;
                break;
            } else {
                player->dontbreakjump = false;
            }
            joy_check_directions(player, joy_state);
            joy_check_falling(player, joy_state);
            grace_jump(player);
            player->ent.vy += GRAVITY_FALL;
            player->ent.vx = F16_mul(player->ent.vx, GROUND_FRICTION);
            check_for_shot(player, joy_state);
            check_is_on_wall(player, joy_state);
            break;

        case P_ON_WALL:
            wall_move(player, joy_state);
            player->timer_stamina--;
            if (player->timer_stamina < 0) player->state = P_FALLING;


            break;    

        case P_EDGE_GRAB:
            joy_check_grab_directions(player, joy_state);
            player->state = P_FALLING;
            break;

        case P_SHOT_JUMP:
            check_for_shot(player, joy_state);
            if (player->timer_shot_jump == 0) player->state = P_FALLING;
            break;

        case P_FLYING:
            player->ent.vx = player->ent.vy = F16_0;
            check_fly_controls(player, joy_state);
            break;

        default: break;
    }

    if (player->ent.vx > RUN_SPEED_MAX) player->ent.vx = RUN_SPEED_MAX;
    if (player->ent.vx < -RUN_SPEED_MAX) player->ent.vx = -RUN_SPEED_MAX;
    if (player->ent.vy > FALL_SPEED_MAX) player->ent.vy = FALL_SPEED_MAX;
   
    player->ent.vx += player->solid_vx;
    player->ent.vy += player->solid_vy;


    player->state_old_joy = joy_state;
    player->state_old = player->state;
}