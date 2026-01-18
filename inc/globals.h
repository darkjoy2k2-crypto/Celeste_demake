#pragma once

#include <genesis.h>

/* =============================================================================
PLAYER PHYSIK-FLAGS (Bitmaske u16)
=============================================================================
*/
#define P_FLAG_ON_GROUND      (1 << 0)
#define P_FLAG_ON_WALL        (1 << 1)
#define P_FLAG_TRAMPOLINE     (1 << 2)
#define P_FLAG_DYING          (1 << 3)
#define P_FLAG_DONT_BREAK     (1 << 4)
#define P_FLAG_FACING_LEFT    (1 << 5)
#define P_FLAG_EDGE_GRAB      (1 << 6)

/* Makros zur schnellen Bit-Manipulation am Player-Zustand */
#define SET_P_FLAG(mask, flag)   ((mask) |= (flag))
#define CLEAR_P_FLAG(mask, flag) ((mask) &= ~(flag))
#define CHECK_P_FLAG(mask, flag) ((mask) & (flag))

/* =============================================================================
INPUT / BUTTON MAPPING
=============================================================================
*/
typedef enum {
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_JUMP,
    ACTION_DASH,
    ACTION_SHOT,
    ACTION_START,
    ACTION_MAX // Hilfswert für Array-Größe
} PlayerAction;

/* Globales Mapping-Array (In globals.c definiert) */
extern u16 button_map[ACTION_MAX];

/* =============================================================================
PHYSIK & GAMEPLAY KONSTANTEN
=============================================================================
*/
#define F16_0               FIX16(0)
#define F16_01              FIX16(0.1)
#define abs16(x)            ((x) < 0 ? -(x) : (x))

/* Gravitation & Reibung */
#define GRAVITY_JUMP        FIX16(0.2)
#define GRAVITY_FALL        FIX16(0.4)
#define GRAVITY_GROUNDED    FIX16(0.5)
#define FRICTION            FIX16(0.9)
#define FRICTION_AIR        FIX16(0.98)

/* Geschwindigkeiten & Kräfte */
#define MOVE_SPEED          FIX16(0.5)
#define MOVE_SPEED_MAX      FIX16(2.0)
#define CLIMB_SPEED         FIX16(0.5)
#define JUMP_FORCE          FIX16(-4)
#define JUMP_FORCE_WALL_V   FIX16(-4.0)
#define JUMP_FORCE_WALL_H   FIX16(3.0)
#define JUMP_FORCE_EDGE     FIX16(-3.5)
#define JUMP_RELEASE_LIMIT  FIX16(-1.5)
#define HELPING_HOP         FIX16(-3)

/* Shot-Jump (Der "Rückstoß-Sprung") */
#define SHOT_JUMP_SPEED      FIX16(4)
#define SHOT_JUMP_SPEED_DIAG FIX16(2.8)
#define TIMER_SHOT_JUMP_MAX  15

/* Timer & Kosten */
#define TIMER_GRACE_MAX     6
#define TIMER_BUFFER_MAX    6
#define DASH_STAMINA_COST   10

/* =============================================================================
FARB-KONSTANTEN (Globaler Zugriff für PAL-Funktionen)
=============================================================================
*/
extern const u16 COL_BALL_WHITE[3];
extern const u16 COL_BALL_YELLOW[3];
extern const u16 COL_BALL_ORANGE[3];
extern const u16 COL_BALL_RED[3];
extern const u16 COL_BALL_BLUE[3];

/* =============================================================================
STRUKTUREN & MATHEMATIK
=============================================================================
*/
typedef struct Rect_s16 { Vect2D_s16 pos; Vect2D_s16 size; } Rect_s16;
typedef struct Rect_f32 { Vect2D_f32 pos; Vect2D_f32 size; } Rect_f32;

/* Globaler VDP & Map Zugriff */
extern Map* level_1_map;
extern s16 player_id;
extern u16 ind; 
extern u16 hud_tile_base;

/* Funktions-Deklarationen */
fix32 getSinusValueF32(u16 time, u16 speed, u16 amplitude);
bool PointVsRect_s16(Vect2D_s16 p, Rect_s16 r);
bool PointVsRect_f32(const Vect2D_f32 *p, const Rect_f32 *r);
bool RectVsRect_f32(const Rect_f32 *r1, const Rect_f32 *r2);