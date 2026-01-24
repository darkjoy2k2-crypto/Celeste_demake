#include "states/states.h"
#include "globals.h"

// Leere Funktionen für den Sicherheits-State
static void doNothing() { }

static const GameState EmptyState = {
    .enter  = doNothing,
    .update = doNothing,
    .exit   = doNothing
};

// 'static' sorgt dafür, dass nur dieses Skript die Zeiger ändern kann
static const GameState* currentState = &EmptyState;
static const GameState* currentSubState = NULL;

void STATE_set(const GameState* nextState) {
    // Exit alten State
    if (currentState->exit) {
        GameSync = false;
        currentState->exit();
    }
    
    // Wechsel (Sicherheits-Check auf NULL)
    currentState = nextState ? nextState : &EmptyState;
    currentSubState = NULL; 

    // Enter neuen State
    if (currentState->enter) {
        currentState->enter();
    }
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

void STATE_update() {
    GameSync = true;
    
    // Substate hat Vorrang (z.B. Pause-Menü)
    if (currentSubState) {
        if (currentSubState->update) currentSubState->update();
    } else {
        // Dank EmptyState ist currentState niemals NULL
        if (currentState->update) currentState->update();
    }
}