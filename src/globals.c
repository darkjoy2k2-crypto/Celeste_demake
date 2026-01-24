#include "globals.h"

// Initialisierung der Variablen
u16 LIVES = MAX_LIVES;
bool GameSync = false;
s16 player_id = -1;
u16 ind = TILE_USER_INDEX;
u16 hud_tile_base = 0;
Map* level_1_map = NULL;
Player* player_one = NULL;
StateContext state_ctx;

// Arrays
u16 button_map[ACTION_MAX] = {
    [ACTION_UP]    = BUTTON_UP,
    [ACTION_DOWN]  = BUTTON_DOWN,
    [ACTION_LEFT]  = BUTTON_LEFT,
    [ACTION_RIGHT] = BUTTON_RIGHT,
    [ACTION_JUMP]  = BUTTON_A,
    [ACTION_DASH]  = BUTTON_B,
    [ACTION_SHOT]  = BUTTON_C,
    [ACTION_START] = BUTTON_START
};

const u16 COL_BALL_WHITE[]  = { 0x0EEE, 0x0AAA, 0x0666 };
const u16 COL_BALL_YELLOW[] = { 0x00AA, 0x0066, 0x00EE };
const u16 COL_BALL_ORANGE[] = { 0x004A, 0x0026, 0x026E };
const u16 COL_BALL_RED[]    = { 0x000E, 0x000A, 0x0006 };
const u16 COL_BALL_BLUE[]   = { 0x0E00, 0x0A00, 0x0600 };

// Logik-Funktionen
bool PointVsRect_s16(Vect2D_s16 p, Rect_s16 r) {
    return (p.x >= r.pos.x && p.x < r.pos.x + r.size.x &&
            p.y >= r.pos.y && p.y < r.pos.y + r.size.y);
}

bool PointVsRect_f32(const Vect2D_f32 *p, const Rect_f32 *r) {
    return (p->x >= r->pos.x && p->x < r->pos.x + r->size.x &&
            p->y >= r->pos.y && p->y < r->pos.y + r->size.y);
}

bool RectVsRect_f32(const Rect_f32 *r1, const Rect_f32 *r2) {
    return (r1->pos.x < r2->pos.x + r2->size.x &&
            r1->pos.x + r1->size.x > r2->pos.x &&
            r1->pos.y < r2->pos.y + r2->size.y &&
            r1->pos.y + r1->size.y > r2->pos.y);
}

fix32 getSinusValueF32(u16 time, s16 speed, u16 amplitude) {
    u16 angle = (time * speed) % 1024;
    return F32_mul( sinFix32(angle), FIX32(amplitude));
}