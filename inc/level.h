#pragma once

#include <genesis.h>
#include "area.h"


// Globale Level-Konstanten
// --- KONSTANTEN ---

#define cam_min_x 102
#define cam_max_x 203
#define cam_min_y 65
#define cam_max_y 176


// Die Map selbst (extern, damit nur eine Instanz existiert)
extern const fix32 OFFSET;

typedef struct {
    s16 x;
    s16 y;
    fix16 speed;
    u16 behavior;
} PlatformDef;

typedef struct {
    // Group 1: 32-bit Pointers (4 bytes each)
    const Area* areas;
    const MapDefinition* map_def;
    const TileSet* tileset;
    const Image* background;
    const u16* collision_data;
    const PlatformDef* platforms;

    // Group 2: 16-bit Integers (2 bytes each)
    u16 width_tiles;
    u16 height_tiles;
    u16 area_count;
    u16 platform_count;
} LevelDefinition;

extern u16 current_level_index;
extern const LevelDefinition levels[];