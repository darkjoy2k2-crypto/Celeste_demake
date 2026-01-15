// main.c (oder globals.c)
#include "genesis.h"
#include "globals.h"

const u16 COL_BALL_WHITE[]  = { 0x0EEE, 0x0AAA, 0x0666 };
const u16 COL_BALL_YELLOW[] = { 0x00AA, 0x0066, 0x00EE };
const u16 COL_BALL_ORANGE[] = { 0x004A, 0x0026,0x026E };
const u16 COL_BALL_RED[]    = { 0x000E, 0x000A, 0x0006 };
const u16 COL_BALL_BLUE[]   = { 0x0E00, 0x0A00, 0x0600 };

Map* level_1_map = NULL;

u16 ind = TILE_USER_INDEX;
u16 hud_tile_base = 0;
s16 player_id = -1;

u16 BTN_UP      = BUTTON_UP;
u16 BTN_DOWN    = BUTTON_DOWN;
u16 BTN_LEFT    = BUTTON_LEFT;
u16 BTN_RIGHT   = BUTTON_RIGHT;
u16 BTN_JUMP    = BUTTON_A;
u16 BTN_DASH    = BUTTON_B;
u16 BTN_ACTION  = BUTTON_C;
u16 BTN_START   = BUTTON_START;
u16 BTN_MODE    = BUTTON_MODE;

bool PointVsRect_f32(const Vect2D_f32 *p, const Rect_f32 *r)
{
    fix32 px = p->x;
    fix32 rx = r->pos.x;

    if (px < rx) return false;
    if (px >= rx + r->size.x) return false;

    fix32 py = p->y;
    fix32 ry = r->pos.y;

    if (py < ry) return false;
    if (py >= ry + r->size.y) return false;

    return true;
}

bool RectVsRect_f32(const Rect_f32 *r1, const Rect_f32 *r2)
{
    fix32 r1x = r1->pos.x;
    fix32 r1y = r1->pos.y;
    fix32 r2x = r2->pos.x;
    fix32 r2y = r2->pos.y;

    if (r1x >= r2x + r2->size.x) return false;
    if (r1y >= r2y + r2->size.y) return false;
    if (r2x >= r1x + r1->size.x) return false;
    if (r2y >= r1y + r1->size.y) return false;

    return true;
}

fix32 getSinusValueF32(u16 time, u16 speed, u16 amplitude) 
{
    u16 angle = (time * speed) % 1024;
    
    // sinFix32 liefert -1.0 bis 1.0 als fix32
    fix32 sin_val = sinFix32(angle);
    
    // Multiplikation mit der Amplitude (als fix32)
    return F32_mul(sin_val, FIX32(amplitude));
}
