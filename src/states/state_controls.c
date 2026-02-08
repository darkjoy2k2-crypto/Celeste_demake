#include <genesis.h>
#include "debug.h"
#include "fade.h"
#include "globals.h"
#include "states/states.h"
#include "title.h"

// Permutationen im ROM (static const spart RAM)
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
    button_map[ACTION_DASH] = control_presets[idx][1]; // In deinem Code Dash
    button_map[ACTION_SHOT] = control_presets[idx][2]; // In deinem Code Climb
}

static void drawLabels() {
    VDP_setTextPriority(1); 
    VDP_setTextPalette(PAL2);

    // 1. Alte Texte löschen
    VDP_drawText("      ", 15, 17);
    VDP_drawText("      ", 17, 19);
    VDP_drawText("      ", 20, 21);

    // 2. JUMP zuordnen (ACTION_JUMP)
    u16 btnJump = button_map[ACTION_JUMP];
    if (btnJump == BUTTON_A)      VDP_drawText("JUMP", 15, 17);
    else if (btnJump == BUTTON_B) VDP_drawText("JUMP", 17, 19);
    else if (btnJump == BUTTON_C) VDP_drawText("JUMP", 20, 21);

    // 3. CLIMB zuordnen (In deinem Array ACTION_DASH)
    u16 btnClimb = button_map[ACTION_DASH];
    if (btnClimb == BUTTON_A)      VDP_drawText("CLIMB", 15, 17);
    else if (btnClimb == BUTTON_B) VDP_drawText("CLIMB", 17, 19);
    else if (btnClimb == BUTTON_C) VDP_drawText("CLIMB", 20, 21);

    // 4. DASH zuordnen (In deinem Array ACTION_SHOT)
    u16 btnDash = button_map[ACTION_SHOT];
    if (btnDash == BUTTON_A)      VDP_drawText("DASH", 15, 17);
    else if (btnDash == BUTTON_B) VDP_drawText("DASH", 17, 19);
    else if (btnDash == BUTTON_C) VDP_drawText("DASH", 20, 21);
}

static void enter() {
    memset(&state_ctx, 0, sizeof(state_ctx));
    
    FADE_set_target(PAL0, pal_controls.data);
    PAL_setColors(0, palette_black, 64, CPU);

    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    // Bild laden
    VDP_drawImageEx(BG_B, &bg_controls, TILE_ATTR_FULL(PAL0, false, false, false, ind), 0, 0, false, true);

    // WICHTIG: Text-Eigenschaften setzen
    VDP_setTextPalette(PAL2); 
    VDP_setTextPriority(1); // Text auf höchste Priorität setzen, damit er ÜBER dem Bild liegt

    VDP_drawText("CONTROLS", 12, 0);
    VDP_drawText("MOVE", 2, 19);
    VDP_drawText("  Move left or right", 6, 24); 
    VDP_drawText("<< to remap buttons >>", 6, 26);

    // Die Belegung basierend auf dem globalen Index anwenden
    apply_preset(current_control_preset);
    
    // Kurz warten, bis der VDP bereit ist
    SYS_doVBlankProcess(); 
    
    drawLabels();

    FADE_in(15, false);
}

static void update() {
    u16 joy = JOY_readJoypad(JOY_1);
    ControlsStateData *sd = &state_ctx.controls;
    u16 changed = joy & ~sd->last_joy_state;

    // Menü-Steuerung
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
    // Zurück zum Titel bei Start oder Action-Buttons
    else if (changed & (BUTTON_START | BUTTON_A | BUTTON_B | BUTTON_C)) {
        STATE_set(&State_Title);
        return;
    }

    // Idle-Timer Logik
    sd->idle_timer = joy ? 0 : sd->idle_timer + 1;
    if (sd->idle_timer >= 480) {
        STATE_set(&State_Title);
    }

    sd->last_joy_state = joy;
    SYS_doVBlankProcess();
}

static void exit() {
    FADE_out(15, false);
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    ind = TILE_USER_INDEX;
    SPR_reset();
}

const GameState State_Controls = { enter, update, exit };