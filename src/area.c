#include "area.h"
#include "entities/handle_entities.h"
#include "globals.h"

void update_area(Entity* e) {

    Player* p = (Player*) e;
    int area_id = get_current_area_id(e->x, e->y);
    if (area_id == -1) return;

    const Area* new_area = get_area(area_id);
    
    if (p->current_area != NULL) {
        if (p->current_area->die_on_bottom && e->y >= p->current_area->max.y) {
            return; 
        }
    }

    p->current_area = (Area*)new_area;
}


void load_areas(const Area* data, u16 count) {
    state_ctx.ingame.active_areas = data;
    state_ctx.ingame.active_count = count;
}



const Area* get_area(u16 id) {
    if (state_ctx.ingame.active_areas != NULL && id < state_ctx.ingame.active_count) {
        return &state_ctx.ingame.active_areas[id];
    }
    return NULL;
}

int get_current_area_id(s16 px, s16 py) {
    if (state_ctx.ingame.active_areas == NULL) return -1;

    int found_id = -1;

    for (int i = 0; i < (int)state_ctx.ingame.active_count; i++) {
        const Area* a = &state_ctx.ingame.active_areas[i];
        
        if (px >= a->min.x && px < a->max.x &&
            py >= a->min.y && py < a->max.y) {
            found_id = i; 
        }
    }
    return found_id;
}