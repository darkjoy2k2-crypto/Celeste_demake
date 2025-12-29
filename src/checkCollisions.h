#ifndef _CHECK_COLLISIONS_H_
#define _CHECK_COLLISIONS_H_

#include "entity_list.h"

typedef enum {
    SIDE_NONE,
    SIDE_TOP,
    SIDE_BOTTOM,
    SIDE_LEFT,
    SIDE_RIGHT,
    SIDE_PEEK
} CollisionSide;

typedef enum {
    EDGE_NONE = 0,
    EDGE_LEFT = -1,
    EDGE_RIGHT = 1
} EdgeGrabSide;

void check_collision(Entity* entity);

#endif