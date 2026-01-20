#pragma once

#include <genesis.h>
#include "entities/handle_entities.h"
#include "area.h"

extern Vect2D_s16 camera_position;

void init_camera();
void update_camera(Entity* player, Map* level_map, bool instant);

