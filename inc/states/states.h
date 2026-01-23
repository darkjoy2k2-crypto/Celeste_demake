#pragma once

typedef struct {
    void (*enter)();  
    void (*update)(); 
    void (*exit)();  
} GameState;

void STATE_set(const GameState* nextState);
void STATE_setSubState(const GameState* nextSub);
void STATE_exitSubState();
void STATE_update();

extern const GameState State_Title;
extern const GameState State_InGame;
extern const GameState State_GameOver;
extern const GameState SubState_Death;
