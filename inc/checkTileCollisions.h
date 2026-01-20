#pragma once

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

/* Name an die .c Datei anpassen */
void check_tile_collision(Entity* entity);