#include <genesis.h>
#include "checkTileCollisions.h"
#include "tiles.h"
#include "entity_list.h" 
#include "level.h"
#include "globals.h"
#include "debug.h"

#define MAP_WIDTH_TILES MAP_W
#define MAP_HEIGHT_TILES MAP_H

static inline bool isTileSolid(Entity* entity, s16 world_x, s16 world_y, CollisionSide side)
{
    s16 tile_x = world_x >> 3;
    s16 tile_y = world_y >> 3;

    if (tile_x < 0 || tile_y < 0 || tile_x >= MAP_WIDTH_TILES || tile_y >= MAP_HEIGHT_TILES)
        return true;

    u16 tile_index = map_collision_data[tile_y * MAP_WIDTH_TILES + tile_x];

    return is_tile_solid_only(tile_index, entity, world_x, world_y, side);
}

static inline void apply_step_up(Entity *entity, fix16 saved_vx, s16 current_x, s16 current_y)
{
    if (saved_vx == F16_0) return;

    s16 dir = (saved_vx > 0) ? 1 : -1;
    s16 half_w = entity->width >> 1;
    s16 half_h = entity->height >> 1;

    s16 check_x = current_x + (dir * half_w) + dir;
    s16 foot_y = current_y + half_h - 1;
    s16 head_y = current_y - half_h;

    bool foot_blocked = isTileSolid(entity, check_x, foot_y, SIDE_PEEK);
    bool head_free    = !isTileSolid(entity, check_x, head_y, SIDE_PEEK);

    if (foot_blocked && head_free)
    {
        entity->vx = saved_vx;
        entity->vy = HELPING_HOP;
        if (entity->type == ENTITY_PLAYER)
            ((Player*)entity)->state = P_FALLING;
    }
}

void check_tile_collision(Entity *entity)
{
    Player* p = (entity->type == ENTITY_PLAYER) ? (Player*)entity : NULL;

    if (p) {
        CLEAR_P_FLAG(p->physics_state, P_FLAG_ON_WALL);
        CLEAR_P_FLAG(p->physics_state, P_FLAG_TRAMPOLINE);
    }

    fix16 saved_vx = entity->vx;

    s16 current_x = F32_toRoundedInt(entity->x_f32);
    s16 current_y = F32_toRoundedInt(entity->y_f32);
    s16 half_w = entity->width >> 1;
    s16 half_h = entity->height >> 1;

    if (entity->vx != F16_0 || (p && p->solid_vx != F16_0))
    {
        s16 vel_x = entity->vx + (p ? p->solid_vx : 0);
        s16 dir = (vel_x > 0) ? 1 : -1;

        if (p) {
            if (dir > 0) CLEAR_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT);
            else SET_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT);
        }

        s16 check_x = (dir > 0) ? (current_x + half_w - 1) : (current_x - half_w);
        CollisionSide side = (dir > 0) ? SIDE_RIGHT : SIDE_LEFT;

        if (isTileSolid(entity, check_x, current_y - half_h + 1, side) ||
            isTileSolid(entity, check_x, current_y, side) ||
            isTileSolid(entity, check_x, current_y + half_h - 1, side))
        {
            s16 tile_col_x = (check_x >> 3);
            current_x = (dir > 0) ? (tile_col_x << 3) - half_w
                                  : (tile_col_x << 3) + 8 + half_w;
            entity->vx = F16_0;
            entity->x_f32 = FIX32(current_x);
        }
    }

    if (p && p->state == P_GROUNDED && entity->vx == F16_0 && saved_vx != F16_0)
        apply_step_up(entity, saved_vx, current_x, current_y);

    if (entity->vy != F16_0 || (p && p->solid_vy != F16_0))
    {
        s16 dir = (entity->vy + (p ? p->solid_vy : 0) > 0) ? 1 : -1;
        s16 check_y = (dir > 0) ? (current_y + half_h) : (current_y - half_h);
        CollisionSide side = (dir > 0) ? SIDE_BOTTOM : SIDE_TOP;

        bool hitLeft  = isTileSolid(entity, current_x - half_w + 1, check_y, side);
        bool hitRight = isTileSolid(entity, current_x + half_w - 1, check_y, side);
        bool hitMid   = isTileSolid(entity, current_x, check_y, side);

        bool collision = (dir > 0) ?
            (isTileSolid(entity, current_x - half_w + 3, check_y, SIDE_BOTTOM) ||
             hitMid ||
             isTileSolid(entity, current_x + half_w - 3, check_y, SIDE_BOTTOM))
            : (hitLeft || hitMid || hitRight);

        if (collision)
        {
            if (dir < 0 && !hitMid)
            {
                if (hitLeft && !hitRight) {
                    current_x += 2;
                    entity->x_f32 = FIX32(current_x);
                    hitMid = isTileSolid(entity, current_x, check_y, side);
                    if (!hitMid) goto no_y_collision;
                    current_x -= 2;
                }
                else if (hitRight && !hitLeft) {
                    current_x -= 2;
                    entity->x_f32 = FIX32(current_x);
                    hitMid = isTileSolid(entity, current_x, check_y, side);
                    if (!hitMid) goto no_y_collision;
                    current_x += 2;
                }
            }

            s16 tile_col_y = (check_y >> 3);
            if (dir > 0) {
                current_y = (tile_col_y << 3) - half_h;
                if (p) SET_P_FLAG(p->physics_state, P_FLAG_ON_GROUND);
            } else {
                current_y = (tile_col_y << 3) + 8 + half_h;
            }

            entity->vy = F16_0;
            entity->y_f32 = FIX32(current_y);
        }
    }

no_y_collision:

    entity->x = current_x;
    entity->y = current_y;

    s16 right_x = current_x + half_w;
    if (isTileSolid(entity, right_x, current_y - half_h + 1, SIDE_PEEK) ||
        isTileSolid(entity, right_x, current_y, SIDE_PEEK) ||
        isTileSolid(entity, right_x, current_y + half_h - 1, SIDE_PEEK))
    {
        if (p) {
            SET_P_FLAG(p->physics_state, P_FLAG_ON_WALL);
            CLEAR_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT);
        }
    }
    else
    {
        s16 left_x = current_x - half_w - 1;
        if (isTileSolid(entity, left_x, current_y - half_h + 1, SIDE_PEEK) ||
            isTileSolid(entity, left_x, current_y, SIDE_PEEK) ||
            isTileSolid(entity, left_x, current_y + half_h - 1, SIDE_PEEK))
        {
            if (p) {
                SET_P_FLAG(p->physics_state, P_FLAG_ON_WALL);
                SET_P_FLAG(p->physics_state, P_FLAG_FACING_LEFT);
            }
        }
    }

    if (p)
    {
        if (p->state == P_FLYING) return;

        if (CHECK_P_FLAG(p->physics_state, P_FLAG_ON_GROUND)) {
            p->state = P_GROUNDED;
            entity->vy = F16_0;
        }
        else {
            if (p->state != P_SHOT_JUMP && p->state != P_ON_WALL &&
                p->state != P_JUMPING && p->state != P_EDGE_GRAB &&
                p->state != P_FLYING && p->state != P_DASHING)
            {
                if (entity->vy < F16_0) {
                    p->state = P_FALLING;
                } else {
                    s16 check_y_ground = entity->y + half_h + 1;
                    bool has_tile_ground =
                        isTileSolid(entity, current_x - half_w + 3, check_y_ground, SIDE_BOTTOM) ||
                        isTileSolid(entity, current_x, check_y_ground, SIDE_BOTTOM) ||
                        isTileSolid(entity, current_x + half_w - 3, check_y_ground, SIDE_BOTTOM);

                    if (!has_tile_ground) p->state = P_FALLING;
                    else {
                        p->state = P_GROUNDED;
                        entity->vy = F16_0;
                        SET_P_FLAG(p->physics_state, P_FLAG_ON_GROUND);
                    }
                }
            }
        }
    }
}
