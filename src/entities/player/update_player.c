#include "entities/player/player_physics.h"
#include "entities/player/player_controls.h"
#include "area.h"
#include "physics/handle_platform_collision.h"
#include "physics/handle_tile_collision.h"
#include "globals.h"

static void reset_physics_internal(Player* p) {
    CLEAR_P_FLAG(p->physics_state, P_FLAG_ON_GROUND); 
    CLEAR_P_FLAG(p->physics_state, P_FLAG_ON_WALL);
}

void ENTITY_UPDATE_player(Entity* self) {
    Player* p = (Player*) self;

    update_area(self);
    reset_physics_internal(p);

    handle_platform_collision(self); 
    check_tile_collision(self);

    self->x_f32 = FIX32(self->x);
    self->y_f32 = FIX32(self->y);

    handle_player_input(p);
    update_player_state_and_physics(self);

    self->x = F32_toInt(self->x_f32);
    self->y = F32_toInt(self->y_f32);
}