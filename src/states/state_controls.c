#include <genesis.h>
#include "debug.h"
#include "fade.h"
#include "globals.h"
#include "states/states.h"
#include "background.h"
#include "fonts.h"
// Leere Funktion, falls doch noch ein Interrupt feuert
static void hint_dummy() { }

static const u16 control_presets[6][3] = {
    {BUTTON_A, BUTTON_B, BUTTON_C},
    {BUTTON_A, BUTTON_C, BUTTON_B},
    {BUTTON_B, BUTTON_A, BUTTON_C},
    {BUTTON_B, BUTTON_C, BUTTON_A},
    {BUTTON_C, BUTTON_A, BUTTON_B},
    {BUTTON_C, BUTTON_B, BUTTON_A}
};

static void apply_preset(u16 idx) {
    button_map[ACTION_JUMP] = control_presets[idx][0];
    button_map[ACTION_DASH] = control_presets[idx][1]; 
    button_map[ACTION_SHOT] = control_presets[idx][2]; 
}

static void drawLabels() {
    VDP_setTextPriority(1); 
    VDP_setTextPalette(PAL2);

    // 1. Alte Texte löschen (Bereiche säubern)
    VDP_drawText("      ", 15, 17);
    VDP_drawText("      ", 17, 19);
    VDP_drawText("      ", 20, 21);

    // Mapping logik (bleibt gleich)
    u16 btnJump = button_map[ACTION_JUMP];
    if (btnJump == BUTTON_A)      VDP_drawText("JUMP", 15, 17);
    else if (btnJump == BUTTON_B) VDP_drawText("JUMP", 17, 19);
    else if (btnJump == BUTTON_C) VDP_drawText("JUMP", 20, 21);

    u16 btnClimb = button_map[ACTION_DASH];
    if (btnClimb == BUTTON_A)      VDP_drawText("CLIMB", 15, 17);
    else if (btnClimb == BUTTON_B) VDP_drawText("CLIMB", 17, 19);
    else if (btnClimb == BUTTON_C) VDP_drawText("CLIMB", 20, 21);

    u16 btnDash = button_map[ACTION_SHOT];
    if (btnDash == BUTTON_A)      VDP_drawText("DASH", 15, 17);
    else if (btnDash == BUTTON_B) VDP_drawText("DASH", 17, 19);
    else if (btnDash == BUTTON_C) VDP_drawText("DASH", 20, 21);
}

static void enter() {
    VDP_loadFont(&TS_FONT_SOLID , DMA);
    // 1. Kontext-Speicher löschen (sauberer Start für die interne Logik)
    memset(&state_ctx, 0, sizeof(state_ctx));

    // 2. Tile-Index sicherheitshalber setzen (Defensives Programmieren)
    // Auch wenn der Titel aufräumt, ist es gut, den eigenen Startpunkt zu kennen
    ind = TILE_USER_INDEX; 

    // 3. Paletten-Vorbereitung
    FADE_set_target(PAL0, pal_bg_controls.data);
    PAL_setColors(0, palette_black, 64, CPU);

    // 4. Planes säubern (Sicherheits-Wisch)
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    // 5. Das eigentliche Bild laden (Nutzt den sauberen ind)
    VDP_drawImageEx(BG_B, &bg_controls, TILE_ATTR_FULL(PAL0, false, false, false, ind), 0, 0, false, true);
    ind += bg_controls.tileset->numTile;    

    // 6. Text-Initialisierung
    VDP_setTextPalette(PAL2); 
    VDP_setTextPriority(1); 

    VDP_drawText("CONTROLS", 12, 3);
    VDP_drawText("MOVE", 2, 19);
    VDP_drawText("  Move left or right", 6, 24); 
    VDP_drawText("<< to remap buttons >>", 6, 26);

    // Aktuelle Belegung anwenden & zeichnen
    apply_preset(current_control_preset);
    
    // Kurze Synchronisation mit der Hardware
    SYS_doVBlankProcess(); 
    drawLabels();

    // Fade-In starten
    FADE_in(15, false);
}


static void update() {
    u16 joy = JOY_readJoypad(JOY_1);
    ControlsStateData *sd = &state_ctx.controls;
    u16 changed = joy & ~sd->last_joy_state;

    if (changed & BUTTON_RIGHT) {
        current_control_preset = (current_control_preset + 1) % 6;
        apply_preset(current_control_preset);
        drawLabels();
    } 
    else if (changed & BUTTON_LEFT) {
        current_control_preset = (current_control_preset + 5) % 6;
        apply_preset(current_control_preset);
        drawLabels();
    } 
    else if (changed & (BUTTON_START | BUTTON_A | BUTTON_B | BUTTON_C)) {
        STATE_set(&State_Title);
        return;
    }

    sd->idle_timer = joy ? 0 : sd->idle_timer + 1;
    if (sd->idle_timer >= 480) {
        STATE_set(&State_Title);
        return;
    }

    sd->last_joy_state = joy;
    SYS_doVBlankProcess();
}

static void exit() {
    // BRANDNEST-FIX 2: Interrupts sichern, bevor der Wechsel passiert
    VDP_setHInterrupt(FALSE);
    SYS_setHIntCallback(hint_dummy);

    FADE_out(15, false);  

    SPR_reset();
    VDP_resetSprites();
    
    ind = TILE_USER_INDEX; 

    VDP_setTextPlane(BG_A); 
    VDP_setWindowOff();
    VDP_setHorizontalScroll(BG_A, 0);
    VDP_setVerticalScroll(BG_A, 0);
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    
    SPR_update();
    SYS_doVBlankProcess();
}

const GameState State_Controls = { enter, update, exit };