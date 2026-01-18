#pragma once

#include <genesis.h>

struct Area;
typedef struct Area Area;

#define MAX_ENTITIES 10

/* =============================================================================
   ENTITY TYPEN & PLAYER STATES
   ============================================================================= */
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

/* =============================================================================
   BASIS ENTITY STRUKTUR
   ============================================================================= */
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

/* =============================================================================
   PLAYER STRUKTUR (Erweitert die Entity)
   ============================================================================= */
typedef struct {
    Entity ent; 
    PlayerState state;     
    PlayerState state_old;
    
    fix16 solid_vx, solid_vy;
    
    /* Input & Physik Flags (u16 Bitmaske statt einzelner Bools) */
    u16 state_old_joy;
    u16 physics_state;   /* Hier liegen P_FLAG_ON_GROUND, P_FLAG_FACING_LEFT etc. */
    
    /* Ressourcen & Timer */
    u16 count_shot_jump;
    s16 timer_grace;
    s16 timer_buffer;
    s16 timer_stamina;
    s16 timer_shot_jump;
    s16 timer_death;
    s16 timer_edgegrab;  
    
    /* Welt-Bezug */
    Area* current_area;
} Player;

/* =============================================================================
   SPEICHER-MANAGEMENT
   ============================================================================= */
typedef union {
    Entity entity;
    Player player;
} EntitySlot;

extern Entity* entities[MAX_ENTITIES];
extern u8 entity_used[MAX_ENTITIES];

void init_entities();
int create_entity(s16 x, s16 y, u8 w, u8 h, f16 vx, f16 vy, EntityType type);