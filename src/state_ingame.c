#include <genesis.h>
#include "states.h"
#include "level.h"

static void enter() {
    //VDP_setPalette(PAL1, level_pal);
    //LEVEL_load();
    //PLAYER_init();
}

static void update() {
    //PLAYER_update();
    //LEVEL_update();
    // Hier kommt alles rein, was vorher in deiner main.c Loop stand
}

static void exit() {
    SPR_reset(); // Alles aufräumen für den nächsten State
    //MEM_free(); // Optional: Falls du dynamischen Speicher nutzt
}

const GameState State_InGame = { enter, update, exit };