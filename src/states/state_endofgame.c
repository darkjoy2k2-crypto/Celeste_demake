#include <genesis.h>
#include "fade.h"
#include "globals.h"
#include "states/states.h"
#include "states/states.h"

static u16 tick = 0;

static void enter() {
        
    VDP_setTextPalette(PAL2);

    char str_hearts[32];
    char str_time[32];
    char str_death[32];
    
    u32 elapsed_frames = vtimer - TIME; 

    // 2. Dann durch 60 teilen (oder getTickPerSecond())
    u32 total_seconds = elapsed_frames / 60; 

    // 3. Dann die Stunden/Minuten/Sekunden berechnen
    u16 hours = total_seconds / 3600;
    u16 mins = (total_seconds / 60) % 60;
    u16 secs = total_seconds % 60;
    
    sprintf(str_hearts, "%u", HEARTS);
    sprintf(str_time, "%02u:%02u:%02u", hours, mins, secs);    
    sprintf(str_death, "%u", LIVES);    

    VDP_drawText("C O N G R A T U L A T I O N S", 2 , 7);
    VDP_drawText("Yout made it !!!", 8 , 10);
    
    VDP_drawText("- HEARTS -  DEATH  -  TIME  -", 1,19);

    VDP_drawText(str_death, 15, 21);

    VDP_drawText(str_hearts, 6, 21);

    VDP_drawText(str_time, 21, 21);      

    FADE_in(30, false); 
    tick = 0;
}

static void update() {

    u16 joy_state = JOY_readJoypad(JOY_1);

    /* Nach 3 Sekunden (180 Frames) ODER Tastendruck A */
    if (tick > 1800 || (joy_state & BUTTON_A)) {
        STATE_set(&State_Title);
    }

    SYS_doVBlankProcess();
    tick++;
}

static void exit() {

    FADE_out(30, false);

    tick = 0;
}

const GameState State_EndOfGame = { enter, update, exit };