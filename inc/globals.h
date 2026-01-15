#pragma once

#include <genesis.h>

extern const u16 COL_BALL_WHITE[3];
extern const u16 COL_BALL_YELLOW[3];
extern const u16 COL_BALL_ORANGE[3];
extern const u16 COL_BALL_RED[3];
extern const u16 COL_BALL_BLUE[3];

#define F16_0 FIX16(0)
#define F16_01 FIX16(0.1)
#define abs16(x) ((x) < 0 ? -(x) : (x))

#define HELPING_HOP FIX16(-3)
#define JUMP_FORCE          FIX16(-4)
#define JUMP_FORCE_WALL_V   FIX16(-4.0)
#define JUMP_FORCE_WALL_H   FIX16(3.0)
#define JUMP_FORCE_EDGE     FIX16(-3.5)
#define JUMP_RELEASE_LIMIT  FIX16(-1.5)
#define MOVE_SPEED          FIX16(0.5)
#define DASH_STAMINA_COST   10
#define CLIMB_SPEED FIX16(0.5)
#define GRAVITY_JUMP            FIX16(0.2)
#define GRAVITY_FALL            FIX16(0.4)
#define GRAVITY_GROUNDED        FIX16(0.5)
#define FRICTION                FIX16(0.9)
#define FRICTION_AIR            FIX16(0.98)

#define SHOT_JUMP_SPEED        FIX16(4)
#define SHOT_JUMP_SPEED_DIAG   FIX16(2.8) // (4.5 * 0.707)
#define TIMER_SHOT_JUMP_MAX    15

#define TIMER_GRACE_MAX    6
#define TIMER_BUFFER_MAX   6
#define MOVE_SPEED_MAX             FIX16(2.0)

typedef struct Rect_s16 {
    Vect2D_s16 pos;
    Vect2D_s16 size;
} Rect_s16;

typedef struct Rect_f32 {
    Vect2D_f32 pos;
    Vect2D_f32 size;
} Rect_f32;

fix32 getSinusValueF32(u16 time, u16 speed, u16 amplitude) ;

// Nur die Bekanntmachung für andere Dateien

extern Map* level_1_map;

extern s16 player_id;

extern u16 ind; 
extern u16 hud_tile_base;

extern u16 BTN_UP;
extern u16 BTN_DOWN;
extern u16 BTN_LEFT;
extern u16 BTN_RIGHT;
extern u16 BTN_JUMP;
extern u16 BTN_DASH;
extern u16 BTN_ACTION;
extern u16 BTN_START;
extern u16 BTN_MODE;

bool PointVsRect_s16(Vect2D_s16 p, Rect_s16 r);
bool PointVsRect_f32(const Vect2D_f32 *p, const Rect_f32 *r);
bool RectVsRect_f32(const Rect_f32 *r1, const Rect_f32 *r2);
