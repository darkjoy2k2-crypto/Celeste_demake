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
    s16 x;              /**< Startposition X (Tiles) */
    s16 y;              /**< Startposition Y (Tiles) */
    fix16 speed;        /**< Tempo (FIX16). 0 = Statisch */
    u16 flags;          /**< Verhalten (PLAT_FLAG_X, _SINUS, _BREAKABLE...) */
    s16 amplitude;      /**< SINUS: Taktung/Bauchigkeit der Welle */
    s16 range;          /**< Reichweite in Pixeln (Linear oder Sinus-Radius) */
    s16 timer_a;        /**< Zeit bis Zerbrechen (Frames) */
    s16 timer_b;        /**< Zeit bis Respawn (Frames) */
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