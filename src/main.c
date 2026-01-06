#include <genesis.h>
#include "debug.h"
#include "globals.h"
#include "states.h"


int i = 0;

int main() {
    SPR_init(); 
    VDP_setScreenWidth320();

    VDP_setTextPlane(WINDOW); 
    VDP_setWindowHPos(0, 0);
    VDP_setWindowVPos(0, 2); // 2 Zeilen = 16 Pixel
    VDP_setTextPriority(1);
    VDP_setTextPalette(PAL0);   
   
   
    // um ein massives Rechteck zu haben. 
    // Falls Tile 0 bei dir transparent ist,<n< füllen wir es hier mit Farbe:
    u32 tiles[8] = { 
        0x11111111, 0x11111111, 0x11111111, 0x11111111, 
        0x11111111, 0x11111111, 0x11111111, 0x11111111 
    };
    VDP_loadTileData(tiles, 1, 1, CPU); // Lädt ein massives Tile an Index 1

    u16 priority_attr = TILE_ATTR_FULL(PAL0, 1, 0, 0, 1); // Nutzt Tile Index 1
    VDP_fillTileMapRect(WINDOW, priority_attr, 0, 0, 40, 2);

    FADE_init();


    
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
        
    init_entities();
    init_camera();
    debug_init();
    


    STATE_set(&State_InGame);

    while(1) {

        STATE_update();


    }

    return (0);
}