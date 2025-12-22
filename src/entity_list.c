#include "entity_list.h"
#include "genesis.h"
#include "title.h"


#define MAX_ENTITIES 10

Entity entities[MAX_ENTITIES];
u8 entity_used[MAX_ENTITIES];

void init_entities() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        entity_used[i] = 0;
    }
}

int create_entity(s16 x, s16 y, u16 width, u16 height, u16 type) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!entity_used[i]) {
            entity_used[i] = 1;
            Entity* e = &entities[i];

            e->height = height;
            e->width = width;
            e->x = e->x_old = x;
            e->x_f32 = e->x_old_f32 = FIX32(x);
            e->y = e->y_old = y;
            e->y_f32 = e->y_old_f32 = FIX32(y);

            e->vx = F16_0;
            e->vy = F16_0;

            e->dontbreakjump = false;
            e->facing = 0;
            e->is_on_wall = false;
            e->state = e->state_old = P_FLYING;
            e->state_old_joy = 0;
            e->trampolin = false;
            e->type = type;
            e->is_dying = false;

            e->timer_buffer = 0;
            e->timer_edgegrab = 0;
            e->timer_grace = 0;
            e->timer_shot_jump = 0;
             e->timer_stamina = 100;

            e->timer_wall = 0;
            e->timer_wall_exec = 0;

            e->anim_index = 0;
            e->sprite = SPR_addSprite(&player_sprite, x, y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
            if (e->sprite) {
                SPR_setAnimAndFrame(e->sprite, 0, 0);
            }

            return i;
        }
    }
    return -1;
}

void delete_entity(int id) {
    if (id < 0 || id >= MAX_ENTITIES) return;
    entity_used[id] = 0;
}