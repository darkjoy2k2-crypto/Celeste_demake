#pragma once

#include "entity_list.h" // Stellt die Entity-Struktur (Daten) bereit

// Deklariert die Hauptfunktion der State Machine.
// Diese Funktion wird in player_update.c definiert und von physics.c aufgerufen.
void update_player_state_and_physics(Entity* e);

