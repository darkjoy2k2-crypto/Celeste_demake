#include <genesis.h>
#include "states/states.h"

static const GameState* currentState = NULL;
static const GameState* currentSubState = NULL;

void STATE_set(const GameState* nextState) {
    // Standard state swap: Exit old, Enter new
    if (currentState && currentState->exit) currentState->exit();
    
    currentState = nextState;
    currentSubState = NULL; // Clear substate when changing main states

    if (currentState && currentState->enter) currentState->enter();
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
    // If a substate exists, only update that. 
    // Otherwise, update the main state.
    if (currentSubState && currentSubState->update) {
        currentSubState->update();
    } else if (currentState && currentState->update) {
        currentState->update();
    }
}