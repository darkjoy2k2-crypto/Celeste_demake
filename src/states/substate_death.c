#include <genesis.h>
#include "area.h"
#include "fade.h"
#include "globals.h"
#include "entities/handle_entities.h"
#include "entities/sprites.h"
#include "entities/update_camera.h"
#include "states/states.h"

extern Map* level_1_map;

static Player* p;

static void enter() {
    p = (Player*) entities[player_id];

    // Initial "Hit" logic
    PAL_set_colors(PAL1, 1, COL_BALL_RED, 3);
    p->timer_death = 60;
    p->ent.vy = FIX16(-6);
    
    // Start fade immediately upon entering the death state
    FADE_out(30, true);
}

static void update() {
    // Physics and visual frame-steps
    p->ent.vy += GRAVITY_FALL;
    p->ent.y_f32 += F16_toFix32(p->ent.vy); 
    p->ent.y = F32_toInt(p->ent.y_f32);

    handle_all_animations();
    update_camera(entities[player_id], level_1_map, true);

    p->timer_death--;

    // When timer hits 0, the state manager will call exit()
    if (p->timer_death <= 0) {
        STATE_exitSubState();
    }
    SPR_update(); 
    SYS_doVBlankProcess();
}

static void exit() {
    const Area* death_area = p->current_area;

    /* Teleport player to Spawn */
    if (death_area != NULL) {
        p->ent.x = death_area->spawn.x << 3;
        p->ent.y = death_area->spawn.y << 3;
        p->ent.x_f32 = FIX32(p->ent.x);
        p->ent.y_f32 = FIX32(p->ent.y);
    } 

    p->ent.vx = F16_0;
    p->ent.vy = F16_0;
    p->state = P_FALLING;
    p->count_shot_jump = 2; 
    p->timer_shot_jump = 0;
    LIVES--;
    p->physics_state = 0; 

    update_camera(entities[player_id], level_1_map, true);
    
    // Fade back into the game after teleporting
    FADE_in(30, true);
}

const GameState SubState_Death = {enter, update, exit};