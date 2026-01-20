#include "genesis.h"
#include "globals.h"

const u16 COL_BALL_WHITE[]  = { 0x0EEE, 0x0AAA, 0x0666 };
const u16 COL_BALL_YELLOW[] = { 0x00AA, 0x0066, 0x00EE };
const u16 COL_BALL_ORANGE[] = { 0x004A, 0x0026, 0x026E };
const u16 COL_BALL_RED[]    = { 0x000E, 0x000A, 0x0006 };
const u16 COL_BALL_BLUE[]   = { 0x0E00, 0x0A00, 0x0600 };

u16 button_map[ACTION_MAX] = {
    [ACTION_UP]    = BUTTON_UP,
    [ACTION_DOWN]  = BUTTON_DOWN,
    [ACTION_LEFT]  = BUTTON_LEFT,
    [ACTION_RIGHT] = BUTTON_RIGHT,
    [ACTION_JUMP]   = BUTTON_A,
    [ACTION_DASH]   = BUTTON_B,
    [ACTION_SHOT]   = BUTTON_C,
    [ACTION_START]  = BUTTON_START
};

StateContext state_ctx;
Map* level_1_map = NULL;
u16 ind = TILE_USER_INDEX;
u16 hud_tile_base = 0;
s16 player_id = -1;

bool PointVsRect_s16(Vect2D_s16 p, Rect_s16 r) {
    if (p.x < r.pos.x || p.x >= r.pos.x + r.size.x) return false;
    if (p.y < r.pos.y || p.y >= r.pos.y + r.size.y) return false;
    return true;
}

bool PointVsRect_f32(const Vect2D_f32 *p, const Rect_f32 *r) {
    if (p->x < r->pos.x || p->x >= r->pos.x + r->size.x) return false;
    if (p->y < r->pos.y || p->y >= r->pos.y + r->size.y) return false;
    return true;
}

bool RectVsRect_f32(const Rect_f32 *r1, const Rect_f32 *r2) {
    if (r1->pos.x >= r2->pos.x + r2->size.x) return false;
    if (r1->pos.y >= r2->pos.y + r2->size.y) return false;
    if (r2->pos.x >= r1->pos.x + r1->size.x) return false;
    if (r2->pos.y >= r1->pos.y + r1->size.y) return false;
    return true;
}

fix32 getSinusValueF32(u16 time, u16 speed, u16 amplitude) {
    u16 angle = (time * speed) % 1024;
    fix32 sin_val = sinFix32(angle);
    return F32_mul(sin_val, FIX32(amplitude));
}