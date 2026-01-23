#pragma once

#include <genesis.h>
#include "area.h"


// Globale Level-Konstanten
// --- KONSTANTEN ---
#define MAP_W 102
#define MAP_H 73 
#define cam_min_x 102
#define cam_max_x 203
#define cam_min_y 65
#define cam_max_y 176


// Die Map selbst (extern, damit nur eine Instanz existiert)
extern const fix32 OFFSET;

typedef struct {
    const Area* areas;
    u16 area_count;
    const MapDefinition* map_def;
    const TileSet* tileset;
    const Image* background;
    const u16* collision_data;
} LevelDefinition;

extern u16 current_level_index;
extern const LevelDefinition levels[];