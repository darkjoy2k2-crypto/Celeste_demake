#include <genesis.h>
#include "states.h"

static const GameState* currentState = NULL;

void STATE_set(const GameState* nextState) {
    if (currentState && currentState->exit) {
        currentState->exit();
    }

    currentState = nextState;

    if (currentState && currentState->enter) {
        currentState->enter();
    }
}

void STATE_update() {
    if (currentState && currentState->update) {
        currentState->update();
    }
}