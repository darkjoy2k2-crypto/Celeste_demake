#pragma once

#include <genesis.h>

// --- ENTITY SYSTEM ---
typedef enum {
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_PLATFORM,
    ENTITY_PICKUP,
    ENTITY_ENEMY
} EntityType;

// --- PICKUPS / ITEMS ---
typedef enum {
    PICKUP_HEART,
    PICKUP_HEART_FLEEING,
    PICKUP_BALLOON,
    PICKUP_COIN,
    PICKUP_SPRING,
    PICKUP_NEXTLEVEL
} PickupKind;

// --- PLAYER LOGIC ---
typedef enum {
    P_IDLE, P_RUNNING, P_JUMPING, P_FALLING, P_GROUNDED,
    P_ON_WALL, P_EDGE_GRAB, P_DASHING, P_FLYING, P_SHOT_JUMP
} PlayerState;

// --- PLATFORM LOGIC ---
typedef enum {
    PLAT_IDLE,
    PLAT_BREAKING,
    PLAT_HIDDEN,
    PLAT_DISABLED
} PlatformState;

typedef enum {
    PICK_IDLE,
    PICK_FOLLOW,
    PICK_FLEE
} PickupState;