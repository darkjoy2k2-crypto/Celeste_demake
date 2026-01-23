#pragma once

#include <genesis.h>
#include "level.h"

struct Area;

#define MAX_ENTITIES 10

typedef enum {
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_PLATFORM,
    ENTITY_ENEMY
} EntityType;

typedef enum {
    P_IDLE, P_RUNNING, P_JUMPING, P_FALLING, P_GROUNDED,
    P_ON_WALL, P_EDGE_GRAB, P_DASHING, P_FLYING, P_SHOT_JUMP
} PlayerState;

typedef enum {
    PB_LINEAR, PB_SINUS_WIDE_X, PB_SINUS_WIDE_Y, PB_SINUS_X,PB_SINUS_Y
} PlatformBehavior;

typedef struct Entity {
    EntityType type;
    s16 x, y;
    s16 x_old, y_old;
    fix32 x_f32, y_f32;
    fix32 x_old_f32, y_old_f32;
    fix16 vx, vy;
    u8 width, height;
    Sprite* sprite;
    int anim_index;
    void (*update)(struct Entity* self);} Entity;

typedef struct {
    Entity ent; 
    PlayerState state;     
    PlayerState state_old;
    fix16 solid_vx, solid_vy;
    u16 state_old_joy;
    u16 physics_state;
    u16 count_shot_jump;
    s16 timer_grace;
    s16 timer_buffer;
    s16 timer_stamina;
    s16 timer_shot_jump;
    s16 timer_death;
    s16 timer_edgegrab;  
    struct Area* current_area;
} Player;

typedef struct {
    Entity ent;
    fix16 speed;
    u16 platform_state;
    s16 origin_x, origin_y;
    s16 wait_timer;    
    PlatformBehavior behavior;
    bool enabled;
    bool touched;
} Platform;

typedef union {
    Entity entity;
    Player player;
    Platform platform;
} EntitySlot;

extern Entity* entities[MAX_ENTITIES];
extern EntitySlot entity_pool[MAX_ENTITIES]; 
extern u8 entity_used[MAX_ENTITIES]; // Deklaration: "Existiert irgendwo"

void init_entities();
int create_entity(s16 x, s16 y, u8 w, u8 h, f16 vx, f16 vy, EntityType type);
Platform* create_platform(s16 _originX, s16 _originY, fix16 _speed, PlatformBehavior _platformBehavior);
void spawn_player(u16 spawn_in_area);
void spawn_platforms(const LevelDefinition* lv);

