#pragma once

#include <genesis.h>
#include "level.h"

struct Area;

// --- KONSTANTEN ---

#define SET_P_FLAG(mask, flag)   ((mask) |= (flag))
#define CLEAR_P_FLAG(mask, flag) ((mask) &= ~(flag))
#define CHECK_P_FLAG(mask, flag) ((mask) & (flag))
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define MAX_ENTITIES 15
#define P_FLAG_ON_GROUND      (1 << 0)
#define P_FLAG_ON_WALL        (1 << 1)
#define P_FLAG_TRAMPOLINE     (1 << 2)
#define P_FLAG_DYING          (1 << 3)
#define P_FLAG_DONT_BREAK     (1 << 4)
#define P_FLAG_FACING_LEFT   (1 << 5)
#define P_FLAG_EDGE_GRAB     (1 << 6)
#define P_FLAG_STEPPING      (1 << 7)

// --- FLAGS (Nutzen deine Macros SET_P_FLAG etc.) ---
#define PLAT_FLAG_NONE            0x0000
#define PLAT_FLAG_X               (1 << 0)
#define PLAT_FLAG_Y               (1 << 1)
#define PLAT_FLAG_SINUS           (1 << 2)
#define PLAT_FLAG_INVISIBLE       (1 << 3)
#define PLAT_FLAG_TOUCH_START     (1 << 4)
#define PLAT_FLAG_ONCE            (1 << 5)
#define PLAT_FLAG_BREAKABLE       (1 << 6)
#define PLAT_FLAG_RESPAWN         (1 << 7)
#define PLAT_FLAG_STOMP_BREAK     (1 << 8)

// --- ENTITY TYPEN ---
typedef enum {
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_PLATFORM,
    ENTITY_ENEMY
} EntityType;

// --- PLAYER ZUSTÄNDE ---
typedef enum {
    P_IDLE, P_RUNNING, P_JUMPING, P_FALLING, P_GROUNDED,
    P_ON_WALL, P_EDGE_GRAB, P_DASHING, P_FLYING, P_SHOT_JUMP
} PlayerState;

// --- PLATFORM LEBENSZYKLUS ---
typedef enum {
    PLAT_IDLE,      // Normalzustand (wartend oder bewegend)
    PLAT_BREAKING,  // Timer A läuft (Zittern/Warnung)
    PLAT_HIDDEN,    // Deaktiviert (wartet auf Respawn Timer B)
    PLAT_DISABLED   // Permanent aus (nach ONCE oder STOMP)
} PlatformState;

// --- BASIS ENTITY STRUKTUR ---
typedef struct Entity {
    EntityType type;
    s16 x, y;
    s16 x_old, y_old;
    fix32 x_f32, y_f32;
    fix32 x_old_f32, y_old_f32;
    fix16 vx, vy;
    u8 width, height;
    Sprite* sprite;
    fix16 anim_index;
    void (*update)(struct Entity* self);
} Entity;

// --- PLAYER STRUKTUR ---
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

// --- PLATFORM STRUKTUR ---
typedef struct {
    Entity ent;
    fix16 speed;
    u16 flags;              // PLAT_FLAG_... (Eigenschaften/Gene)
    PlatformState state;    // Aktueller Lebenszustand
    u16 status_bits;        // Temporäre Bits (z.B. Bit 0 = IsMoving)
    s16 origin_x, origin_y;
    s16 wait_timer;         // Zeitgeber für Zustandswechsel
    fix16 anim_timer;         // Zeitgeber für Bewegungskurve
    s16 amplitude;          
    s16 range;              
    s16 timer_a;            
    s16 timer_b;            
    bool enabled;           // Kollision aktiv?
    bool touched;           // Kontakt in diesem Frame?
} Platform;

// --- POOL MANAGEMENT ---
typedef union {
    Entity entity;
    Player player;
    Platform platform;
} EntitySlot;

extern Entity* entities[MAX_ENTITIES];
extern EntitySlot entity_pool[MAX_ENTITIES]; 
extern u8 entity_used[MAX_ENTITIES];

// --- FUNKTIONEN ---
void init_entities();
int  create_entity(s16 x, s16 y, u8 w, u8 h, f16 vx, f16 vy, EntityType type);
Platform* create_platform(const PlatformDef* def);
void destroy_entity(int index);

void spawn_player(u16 spawn_in_area);
void spawn_platforms(const LevelDefinition* lv);

// Deklaration der Update-Funktion (damit create_platform sie kennt)
void ENTITY_UPDATE_platform(Entity* _e);