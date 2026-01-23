#include <genesis.h>
#include "physics/handle_tile_collision.h"
#include "physics/tile_properties.h"
#include "entities/handle_entities.h" 
#include "level.h"
#include "globals.h"
#include "debug.h"

#define MAP_WIDTH_TILES MAP_W
#define MAP_HEIGHT_TILES MAP_H


static inline bool isTileSolid(Entity* entity, const u16* _colMap, u16 _mapW, u16 _mapH, s16 world_x, s16 world_y, CollisionSide side)
{

    s16 tile_x = world_x >> 3;
    s16 tile_y = world_y >> 3;

if (tile_x < 0 || tile_y < 0 || tile_x >= _mapW || tile_y >= _mapH)
        return true;

    u16 tile_index = _colMap[tile_y * _mapW + tile_x];

    return is_tile_solid_only(tile_index, entity, world_x, world_y, side);
}

static inline void apply_step_up(Entity *entity, const u16* _currentCollisionMap, u16 _mapW, u16 _mapH, fix16 saved_vx, s16 current_x, s16 current_y)
{
    if (saved_vx == F16_0) return;

    Player* p = (Player*)entity;
    s16 dir = (saved_vx > 0) ? 1 : -1;
    s16 half_w = entity->width >> 1;
    s16 half_h = entity->height >> 1;

    s16 check_x = current_x + (dir * half_w) + dir;
    s16 foot_y  = current_y + half_h - 1;   // Auf Fußhöhe
    s16 head_y  = current_y - half_h - 4;   // Direkt über der Stufe
    s16 sky_y   = current_y - half_h - 12;  // Platz für den Kopf

    bool foot_blocked = isTileSolid(entity, _currentCollisionMap,  _mapW,  _mapH, check_x, foot_y, SIDE_PEEK);
    bool head_free    = !isTileSolid(entity, _currentCollisionMap,  _mapW,  _mapH, check_x, head_y, SIDE_PEEK);
    bool sky_free     = !isTileSolid(entity, _currentCollisionMap,  _mapW,  _mapH, check_x, sky_y, SIDE_PEEK);

    if (foot_blocked && head_free && sky_free)
    {
        entity->vx = saved_vx;
        entity->vy = HELPING_HOP;
        SET_P_FLAG(p->physics_state, P_FLAG_STEPPING);
        p->state = P_FALLING; // State auf Falling, damit Ground-Physics nicht bremst
    }
}


void check_tile_collision(Entity *entity)
{
    Player* p = (Player*)entity;
    const LevelDefinition* lv = &levels[current_level_index];
    const u16* colMap = lv->collision_data;
    u16 mapW = lv->width_tiles;
    u16 mapH = lv->height_tiles;

    fix16 saved_vx = entity->vx;
    s16 current_x = F32_toInt(entity->x_f32);
    s16 current_y = F32_toInt(entity->y_f32);
    s16 half_w = entity->width >> 1;
    s16 half_h = entity->height >> 1;

    // Horizontale Kollision nur prüfen, wenn wir NICHT gerade steppen
    if (!CHECK_P_FLAG(p->physics_state, P_FLAG_STEPPING))
    {
        if (entity->vx != F16_0)
        {
            s16 vel_x = entity->vx;
            s16 dir = (vel_x > 0) ? 1 : -1;

            if (dir > 0) CLEAR_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT);
            else SET_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT);

            s16 check_x = (dir > 0) ? (current_x + half_w - 1) : (current_x - half_w);
            CollisionSide side = (dir > 0) ? SIDE_RIGHT : SIDE_LEFT;

            if (isTileSolid(entity, colMap, mapW, mapH, check_x, current_y - half_h + 1, side) ||
                isTileSolid(entity, colMap, mapW, mapH, check_x, current_y, side) ||
                isTileSolid(entity, colMap, mapW, mapH, check_x, current_y + half_h - 1, side))
            {
                s16 tile_col_x = (check_x >> 3);
                current_x = (dir > 0) ? (tile_col_x << 3) - half_w
                                      : (tile_col_x << 3) + 8 + half_w;
                
                entity->vx = F16_0;
                entity->x_f32 = FIX32(current_x);
            }
        }
    }

    // Step-up Check: Wenn wir am Boden gestoppt wurden, aber VX eigentlich laufen will
    if (p->state == P_GROUNDED && entity->vx == F16_0 && saved_vx != F16_0)
    {
        apply_step_up(entity, colMap, mapW, mapH, saved_vx, current_x, current_y);
    }

    // Vertikale Kollision
    if (entity->vy != F16_0 || p->solid_vy != F16_0)
    {
        s16 dir = (entity->vy + p->solid_vy > 0) ? 1 : -1;
        s16 check_y = (dir > 0) ? (current_y + half_h) : (current_y - half_h);
        CollisionSide side = (dir > 0) ? SIDE_BOTTOM : SIDE_TOP;

        bool hitLeft  = isTileSolid(entity, colMap, mapW, mapH, current_x - half_w + 5, check_y, side);
        bool hitRight = isTileSolid(entity, colMap, mapW, mapH, current_x + half_w - 5, check_y, side);
        bool hitMid   = isTileSolid(entity, colMap, mapW, mapH, current_x, check_y, side);

        if (hitLeft || hitMid || hitRight)
        {
            s16 tile_col_y = (check_y >> 3);
            if (dir > 0) {
                current_y = (tile_col_y << 3) - half_h;
                SET_P_FLAG(p->physics_state, P_FLAG_ON_GROUND);
            } else {
                current_y = (tile_col_y << 3) + 8 + half_h;
            }

            entity->vy = F16_0;
            entity->y_f32 = FIX32(current_y);
            // Sobald wir Boden berühren, beenden wir das Stepping
            CLEAR_P_FLAG(p->physics_state, P_FLAG_STEPPING);
        }
    }

    // Wand-Flags setzen (Peek)
    s16 peek_dist = half_w;
    if (isTileSolid(entity, colMap, mapW, mapH, current_x + peek_dist, current_y, SIDE_PEEK) ||
        isTileSolid(entity, colMap, mapW, mapH, current_x - peek_dist - 1, current_y, SIDE_PEEK))
    {
        SET_P_FLAG(p->physics_state, P_FLAG_ON_WALL);
    }

    // State finalisieren
    if (CHECK_P_FLAG(p->physics_state, P_FLAG_ON_GROUND)) {
        p->state = P_GROUNDED;
    } else if (!CHECK_P_FLAG(p->physics_state, P_FLAG_STEPPING)) {
        if (p->state != P_JUMPING && p->state != P_ON_WALL && p->state != P_SHOT_JUMP) {
            p->state = P_FALLING;
        }
    }

    // Wenn der Step-up Hop den Scheitelpunkt erreicht, Flag löschen
    if (CHECK_P_FLAG(p->physics_state, P_FLAG_STEPPING) && entity->vy >= 0) {
        CLEAR_P_FLAG(p->physics_state, P_FLAG_STEPPING);
    }

    entity->x = current_x;
    entity->y = current_y;
}