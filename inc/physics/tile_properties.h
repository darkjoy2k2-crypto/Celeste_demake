#pragma once

#include <genesis.h>
#include "entities/handle_entities.h"
#include "physics/handle_tile_collision.h"

#define TILE_EMPTY      0
#define TILE_ONEWAY     6
#define TILE_TRAMPOLINE 7
#define TILE_DEATH_B    10
#define TILE_DEATH_T    11
#define TILE_DEATH_L    12
#define TILE_DEATH_R    13

bool is_tile_solid_only(u16 tile_index, Entity* entity, s16 world_x, s16 world_y, CollisionSide side);

