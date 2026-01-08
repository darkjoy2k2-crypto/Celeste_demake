#include <genesis.h>
#include "globals.h"

void setup_hud(){
    VDP_setTextPlane(WINDOW); 
    VDP_setWindowHPos(0, 0);
    VDP_setWindowVPos(0, 2);
    VDP_setTextPriority(1);
    VDP_setTextPalette(PAL2);
    
    u32 tiles[8] = { 
        0x11111111, 0x11111111, 0x11111111, 0x11111111, 
        0x11111111, 0x11111111, 0x11111111, 0x11111111 
    };

    // Wir laden die Tiles an die aktuelle Position von 'ind'
    VDP_loadTileData(tiles, ind, 1, CPU);

    u16 current_hud_attr = TILE_ATTR_FULL(PAL2, 1, 0, 0, ind);
    hud_tile_base = ind;
    // Wir nutzen 'ind' für das Attribut
    VDP_fillTileMapRect(WINDOW, current_hud_attr, 0, 0, 40, 2);
    // WICHTIG: Den globalen Index um die Anzahl der verbrauchten Tiles erhöhen
    // Da wir 1 Tile geladen haben:
    ind += 1;
}