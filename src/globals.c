// main.c (oder globals.c)
#include "genesis.h"
#include "globals.h"

u16 ind = TILE_USER_INDEX;
u16 hud_tile_base = 0;
s16 player_id = -1;

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


