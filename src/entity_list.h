#ifndef ENTITY_LIST_H
#define ENTITY_LIST_H

#include <genesis.h>

#define F16_1n FIX16(-1)
#define F16_0 FIX16(0)
#define F16_1 FIX16(1)

enum ENTITY_TYPE {
    ENTITY_PLAYER = 0
};

enum ENTITY_STATE {
    E_INACTIVE = 0,
    E_ACTIVE = 1,
    P_IDLE = 2,
    P_DEAD = 3,
    P_ALIVE = 4,
    P_GROUNDED = 5,
    P_JUMPING = 6,
    P_FALLING = 7,
    P_RUNNING = 8,
    P_EDGE_GRAB = 9,
    P_ON_WALL = 10,
    P_SAFE = 11,
    P_SHOT_JUMP = 12,
    P_FLYING = 13
};

typedef struct {
    fix32 x_f32;
    s16 x;

    fix32 y_f32;
    s16 y;
    s16 x_old;
    fix32 x_old_f32;

    s16 y_old;
    fix32 y_old_f32;

    fix16 vx;
    fix16 vy;

    u16 width;
    u16 height;

    u16 state;
    u16 state_old;
    u16 state_old_joy;
    
    s16 anim_index;

    s16 timer_grace;
    s16 timer_buffer;
    s16 timer_edgegrab;
    s16 timer_wall;
    s16 timer_wall_exec;
    s16 timer_shot_jump;
    s16 timer_stamina;

    Sprite* sprite;
    enum ENTITY_TYPE type;

    bool is_on_wall;
    s16 facing;

    bool trampolin;
    bool dontbreakjump;
    
} Entity;

#define MAX_ENTITIES 10
extern Entity entities[MAX_ENTITIES];
extern u8 entity_used[MAX_ENTITIES];

void init_entities();
int create_entity(s16 x, s16 y, u16 width, u16 height, u16 type);
void delete_entity(int id);

#endif
