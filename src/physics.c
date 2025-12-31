#include "physics.h"
#include "entity_list.h"
#include "checkCollisions.h"
#include "player_update.h"
#include "area.h" // Wichtig für update_area

void handle_all_entities(){
    for (int i = 0; i < MAX_ENTITIES; i++){
        if (entity_used[i] == 1 && entities[i] != NULL){
            handle_physics(entities[i]);
        }
    }
}

void handle_physics(Entity* e){
    if (e->type == ENTITY_PLAYER){
        update_area(e);
        update_player_state_and_physics(e); 
        check_collision(e);
    }
    
    // Die Kollision muss für alle aktiven Entities (auch Player) laufen
}