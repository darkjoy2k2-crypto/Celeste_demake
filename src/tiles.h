#ifndef _TILES_H_
#define _TILES_H_

#include <genesis.h>
#include "entity_list.h"

#define TILE_EMPTY      0
#define TILE_DEATH      4
#define TILE_ONEWAY     6
#define TILE_TRAMPOLINE 7

void handle_tile_action(u16 tile_index, Entity* entity, s16 world_y);
bool is_tile_solid_only(u16 tile_index, Entity* entity, s16 world_y, bool is_foot);

#endif