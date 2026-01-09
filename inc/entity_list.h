#pragma once

#include <genesis.h>

struct Area;
typedef struct Area Area;

#define MAX_ENTITIES 10
#define F16_0 FIX16(0)
#define F16_01 FIX16(0.1)

typedef enum {
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_PLATFORM,
    ENTITY_ENEMY
} EntityType;

typedef enum {
    P_IDLE,
    P_RUNNING,
    P_JUMPING,
    P_FALLING,
    P_GROUNDED,
    P_ON_WALL,
    P_EDGE_GRAB,
    P_DASHING,
    P_FLYING,
    P_SHOT_JUMP
} PlayerState;

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
} Entity;

typedef struct {
    Entity ent; 
    PlayerState state;     
    PlayerState state_old;
    u16 state_old_joy;
    
    s16 timer_grace;
    s16 timer_buffer;
    s16 timer_stamina;
    s16 timer_shot_jump;
    s16 timer_death;
    s16 timer_edgegrab;  
    
    s16 facing;
    bool is_on_wall;
    bool trampolin;
    bool is_dying;
    bool dontbreakjump;
    
    Area* current_area;
} Player;

typedef union {
    Entity entity;
    Player player;
} EntitySlot;

extern Entity* entities[MAX_ENTITIES];
extern u8 entity_used[MAX_ENTITIES];

void init_entities();
int create_entity(s16 x, s16 y, u8 w, u8 h, EntityType type);

