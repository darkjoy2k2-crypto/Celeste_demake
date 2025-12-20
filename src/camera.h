#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <genesis.h>
#include "entity_list.h"
#include "area.h"

extern Vect2D_s16 camera_position;

void init_camera();
void update_camera(Entity* player, Map* level_map);

#endif