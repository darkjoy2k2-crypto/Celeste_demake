#include <genesis.h>
#include "checkCollisions.h"
#include "tiles.h"
#include "entity_list.h" 
#include "level.h"
#include "debug.h"

#define TILE_SIZE_PX 8
#define MAP_WIDTH_TILES MAP_W   
#define MAP_HEIGHT_TILES MAP_H  
#define HELPING_HOP FIX16(-3)
#define MAX_COLLISION_STEP 4
#define FIX32_TO_S16(f) ((s16)F32_toRoundedInt(f))

static bool isTileSolid(Entity* entity, s16 world_x, s16 world_y, CollisionSide side) {

    s16 tile_x = world_x >> 3;
    s16 tile_y = world_y >> 3;

    if (tile_x < 0 || tile_y < 0 || tile_x >= MAP_WIDTH_TILES || tile_y >= MAP_HEIGHT_TILES) 
        return true;

    u16 tile_index = map_collision_data[tile_y * MAP_WIDTH_TILES + tile_x];

    return is_tile_solid_only(tile_index, entity, world_x, world_y, side);
}

static void apply_step_up(Entity *entity, fix16 saved_vx, s16 current_x, s16 current_y)
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
        entity->state = P_FALLING;
    }
}

void check_collision(Entity *entity)
{
    entity->is_on_wall = false;
    bool isOnGround = false;
    bool blocked_horizontally = false;
    entity->trampolin = false;

    fix16 saved_vx = entity->vx;

    s16 current_x = FIX32_TO_S16(entity->x_f32);
    s16 current_y = FIX32_TO_S16(entity->y_f32);
    s16 half_w = (entity->width >> 1);
    s16 half_h = (entity->height >> 1);

    s16 desired_x = FIX32_TO_S16(entity->x_f32 + F16_toFix32(entity->vx));
    s16 desired_y = FIX32_TO_S16(entity->y_f32 + F16_toFix32(entity->vy));

    s16 dx = desired_x - current_x;
    if (dx != 0)
    {
        entity->facing = (dx > 0) ? 1 : -1;
        s16 steps = (abs(dx) + MAX_COLLISION_STEP - 1) / MAX_COLLISION_STEP;
        s16 step_x = dx / steps;
        for (int i = 0; i < steps; i++) {
            current_x += step_x;

            s16 check_x;
            CollisionSide side;
            if (dx > 0){
                check_x = current_x + half_w - 1;
                side = SIDE_RIGHT;
            } else {
                check_x = current_x - half_w;
                side = SIDE_LEFT;
            }

            
            if (isTileSolid(entity, check_x, current_y - half_h + 1, side) ||
                isTileSolid(entity, check_x, current_y, side) ||
                isTileSolid(entity, check_x, current_y + half_h - 1, side)) 
            {
                current_x -= step_x;
                entity->vx = F16_0;
                blocked_horizontally = true;
                s16 tile_col_x = (check_x >> 3);
                current_x = (dx > 0) ? (tile_col_x << 3) - half_w : (tile_col_x << 3) + 8 + half_w;
                break;
            }
        }
    }

    if (entity->state == P_GROUNDED && blocked_horizontally && saved_vx != F16_0)
        apply_step_up(entity, saved_vx, current_x, current_y);

    s16 dy = desired_y - current_y;
    if (dy != 0)
    {
        s16 steps = (abs(dy) + MAX_COLLISION_STEP - 1) / MAX_COLLISION_STEP;
        s16 step_y = dy / steps;
        for (int i = 0; i < steps; i++) {
            current_y += step_y;

            s16 check_y = (dy > 0) ? (current_y + half_h) : (current_y - half_h);

            CollisionSide side = (dy > 0) ? SIDE_BOTTOM : SIDE_TOP;

            bool hitLeft  = isTileSolid(entity, current_x - half_w + 1, check_y, side);
            bool hitRight = isTileSolid(entity, current_x + half_w - 1, check_y, side);
            bool hitMid   = isTileSolid(entity, current_x, check_y, side);


            bool collision = (dy > 0) ? (isTileSolid(entity, current_x - half_w + 3, check_y, SIDE_BOTTOM) 
            || hitMid 
            || isTileSolid(entity, current_x + half_w - 3, check_y, SIDE_BOTTOM)) 
            : (hitLeft || hitMid || hitRight);

            if (collision) {
                if (dy < 0 && !hitMid) {
                    if (hitLeft && !hitRight) { current_x += 2; entity->x_f32 = FIX32(current_x); continue; }
                    else if (hitRight && !hitLeft) { current_x -= 2; entity->x_f32 = FIX32(current_x); continue; }
                }
                current_y -= step_y;
                entity->vy = F16_0;
                s16 tile_col_y = (check_y >> 3);
                if (dy > 0) { current_y = (tile_col_y << 3) - half_h; isOnGround = true; }
                else { current_y = (tile_col_y << 3) + 8 + half_h; }
                break;
            }
        }
    }

    entity->x = current_x;
    entity->y = current_y;
    entity->x_f32 = FIX32(current_x);
    entity->y_f32 = FIX32(current_y);

    s16 right_x = current_x + half_w;
    if (isTileSolid(entity, right_x, current_y - half_h + 1, SIDE_PEEK) ||
        isTileSolid(entity, right_x, current_y, SIDE_PEEK) ||
        isTileSolid(entity, right_x, current_y + half_h - 1, SIDE_PEEK))
    {
        entity->is_on_wall = true;
        entity->facing = 1;
    }
    else 
    {
        s16 left_x = current_x - half_w - 1;
        if (isTileSolid(entity, left_x, current_y - half_h + 1, SIDE_PEEK) ||
            isTileSolid(entity, left_x, current_y, SIDE_PEEK) ||
            isTileSolid(entity, left_x, current_y + half_h - 1, SIDE_PEEK))
        {
            entity->is_on_wall = true;
            entity->facing = -1;
        }
    }

    if (isOnGround && entity->vy >= F16_0) {
        entity->state = P_GROUNDED;
        entity->vy = F16_0;
    } else {
        if (entity->state != P_SHOT_JUMP && entity->state != P_ON_WALL && entity->state != P_JUMPING && entity->state != P_EDGE_GRAB && entity->state != P_FLYING) {
            if (entity->vy < F16_0) {
                entity->state = P_FALLING;
            } else {
                s16 check_y_ground = entity->y + half_h + 1;
                bool has_ground = isTileSolid(entity, current_x - half_w + 3, check_y_ground, SIDE_BOTTOM) ||
                                  isTileSolid(entity, current_x, check_y_ground, SIDE_BOTTOM) ||
                                  isTileSolid(entity, current_x + half_w - 3, check_y_ground, SIDE_BOTTOM);
                

                if (!has_ground) entity->state = P_FALLING;
                else { entity->state = P_GROUNDED; entity->vy = F16_0; }
            }
        }
    }
}