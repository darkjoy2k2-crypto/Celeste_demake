#include "debug.h"
#include "genesis.h"
#include "globals.h"

char info_str[32];
char ram_str[10];
char vram_str[10];

int debug_value[10];

void debug_init() {
    for(int i = 0; i < 10; i++) {
        debug_value[i] = 0;
    }
}

void debug_set(int place, int value) {
    if (place >= 0 && place < 10) {
        debug_value[place] = value;
    }
    
    // Wir bauen den String hier nicht mehr mit sprintf zusammen, 
    // sondern zeichnen die Werte in debug_draw einzeln für mehr Stabilität.
}

void debug_set_ram() {
    // uintToStr ist wesentlich sicherer und schneller als sprintf
    uintToStr(MEM_getFree(), ram_str, 1);
    
    // Berechnung der freien Tiles (Standard VRAM Limit ist 2048)
    uintToStr(2048 - ind, vram_str, 1);
}

void debug_draw() {
    // Zeichne die ersten 5 Debug-Werte nebeneinander
    for(int i = 0; i < 5; i++) {
        char val_str[8];
        intToStr(debug_value[i], val_str, 1);
        VDP_drawText(val_str, 1 + (i * 5), 0);
    }

    // RAM und VRAM an unterschiedlichen Positionen zeichnen (Y=1 und Y=2)
    VDP_drawText("RAM ", 25, 0);
    VDP_drawText(ram_str, 30, 0);
    
    VDP_drawText("VRAM", 25, 1);
    VDP_drawText(vram_str, 30, 1);
}