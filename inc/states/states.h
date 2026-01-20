#pragma once

typedef struct {
    void (*enter)();  
    void (*update)(); 
    void (*exit)();  
} GameState;

extern const GameState State_Title;
extern const GameState State_InGame;