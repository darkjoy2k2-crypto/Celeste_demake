#include "area.h"
#include "entity_list.h"

static const Area* active_areas = NULL;
static u16 active_count = 0;

void update_area(Entity* e) {
    if (e->type != ENTITY_PLAYER) return;

    Player* p = (Player*) e;
    s16 px = p->ent.x;
    s16 py = p->ent.y;

    s16 area_id = get_current_area_id(px, py);
    if (area_id == -1) return;

    const Area* a = get_area(area_id);
    p->current_area = (Area*)a; 
}

void load_areas(const Area* data, u16 count) {
    active_areas = data;
    active_count = count;
}

const Area* get_area(u16 id) {
    if (active_areas != NULL && id < active_count) {
        return &active_areas[id];
    }
    return NULL;
}

int get_current_area_id(s16 px, s16 py) {
    if (active_areas == NULL) return -1;

    for (int i = (int)active_count - 1; i >= 0; i--) {
        const Area* a = &active_areas[i];
        
        if (px >= a->min.x && px < a->max.x &&
            py >= a->min.y && py < a->max.y) {
            return i;
        }
    }

    return -1;
}