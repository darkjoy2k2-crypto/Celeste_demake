#include "states/states.h"
#include "globals.h"

// Leere Funktionen für den Sicherheits-State
static void doNothing() { }

static const GameState EmptyState = {
    .enter  = doNothing,
    .update = doNothing,
    .exit   = doNothing
};

// Interne Zustandsvariablen
static const GameState* currentState = &EmptyState;
static const GameState* currentSubState = NULL;

// NEU: Diese Variablen merken sich den Wechselwunsch
static const GameState* pendingState = NULL;
static bool isTransitioning = false;

void STATE_clean_up(){
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    VDP_setHorizontalScroll(BG_A, 0);
    VDP_setVerticalScroll(BG_A, 0);
    VDP_setHorizontalScroll(BG_B, 0);
    VDP_setVerticalScroll(BG_B, 0);
    
    PAL_setColors(0, palette_black, 64, CPU);

    // Sprite-Flush
    SPR_reset();
    VDP_resetSprites();
    VDP_updateSprites(1, DMA); 

    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearPlane(WINDOW, TRUE);

    ind = TILE_USER_INDEX; 
    memset(&state_ctx, 0, sizeof(state_ctx));
    
    SYS_doVBlankProcess();
}

/**
 * STATE_set merkt sich jetzt nur noch das Ziel.
 * Der eigentliche Wechsel passiert sicher am Ende von STATE_update.
 */
void STATE_set(const GameState* nextState) {
    pendingState = nextState ? nextState : &EmptyState;
    isTransitioning = true;
}

void STATE_setSubState(const GameState* nextSub) {
    currentSubState = nextSub;
    if (currentSubState && currentSubState->enter) {
        currentSubState->enter();
    }
}

void STATE_exitSubState() {
    if (currentSubState && currentSubState->exit) {
        currentSubState->exit();
    }
    currentSubState = NULL;
}

/**
 * Zentraler Herzschlag der State Machine
 */
void STATE_update() {
    GameSync = true;
    
    // 1. Normales Update ausführen
    if (currentSubState) {
        if (currentSubState->update) currentSubState->update();
    } else {
        if (currentState->update) currentState->update();
    }

    // 2. CHECK: Wurde während des Updates ein Wechsel angefordert?
    if (isTransitioning) {
        
        // --- EXIT ALT ---
        if (currentState->exit) {
            GameSync = false;
            currentState->exit();
        }

        // --- CLEAN UP ---
        STATE_clean_up();
        
        // --- WECHSEL ---
        currentState = pendingState;
        pendingState = NULL;
        currentSubState = NULL; 
        isTransitioning = false;

        // --- ENTER NEU ---
        if (currentState->enter) {
            currentState->enter();
        }
    }
}