#include "area.h"

static const Area* active_areas = NULL;
static u16 active_count = 0;
static int last_id = 0; 

void load_areas(const Area* data, u16 count) {
    active_areas = data;
    active_count = count;
    last_id = 0;
}

const Area* get_area(u16 id) {
    if (active_areas != NULL && id < active_count) {
        return &active_areas[id];
    }
    return NULL;
}

int get_current_area_id(s16 px, s16 py) {
    if (active_areas == NULL) return -1;

    // 1. PRIORITÄT: Können wir weiter vorwärts gehen?
    // Wir prüfen alle Areas ab dem aktuellen Fortschritt + 1
    for (u16 i = last_id + 1; i < active_count; i++) {
        const Area* a = &active_areas[i];
        if (px >= a->min.x && px < a->max.x &&
            py >= a->min.y && py < a->max.y) {
            
            last_id = i; 
            return i;
        }
    }

    // 2. BLEIBEN: Sind wir noch in der aktuellen Area?
    // Wir erlauben hier einen Puffer von 4 Pixeln nach außen, 
    // damit kurzes Rausglitchen nicht sofort den Rücksprung triggert.
    if (last_id > 0) {
        const Area* current = &active_areas[last_id];
        if (px >= (current->min.x - 4) && px < (current->max.x + 4) &&
            py >= (current->min.y - 4) && py < (current->max.y + 4)) {
            return last_id;
        }
    }

    // 3. FALLBACK: Nur auf Area 0 zurückfallen, wenn wir ECHT weit weg sind
    const Area* master = &active_areas[0];
    if (px >= master->min.x && px < master->max.x &&
        py >= master->min.y && py < master->max.y) {
        
        last_id = 0; 
        return 0;
    }

    return -1;
}