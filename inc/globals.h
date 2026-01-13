#pragma once

#include <genesis.h>

#define F16_0 FIX16(0)
#define F16_01 FIX16(0.1)
#define abs16(x) ((x) < 0 ? -(x) : (x))

// Nur die Bekanntmachung für andere Dateien
extern u16 ind; 
extern u16 hud_tile_base;
extern s16 player_id;

typedef struct Rect_s16 {
    Vect2D_s16 pos;
    Vect2D_s16 size;
} Rect_s16;

typedef struct Rect_f32 {
    Vect2D_f32 pos;
    Vect2D_f32 size;
} Rect_f32;

bool PointVsRect_s16(Vect2D_s16 p, Rect_s16 r);
bool PointVsRect_f32(const Vect2D_f32 *p, const Rect_f32 *r);
bool RectVsRect_f32(const Rect_f32 *r1, const Rect_f32 *r2);
fix32 getSinusValueF32(u16 time, u16 speed, u16 amplitude) ;
