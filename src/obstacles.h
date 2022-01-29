#ifndef _OBSTACLES_H
#define _OBSTACLES_H
#include "raylib.h"
#include "globals.h"

void
spawn_obstacle(struct obstacles *o)
{
    // just randomly create obstacles here
    int i = 0;
    for (; i < OBSTACLES_LIM; i++)
        if (!o[i].active) break;
    if (OBSTACLES_LIM == i) return;

    o[i].active = 1;
    {
        o[i].pos.x =
            (float)GetRandomValue(game_bounds.shape.x, game_bounds.shape.x +game_bounds.shape.width)
            * 1 +world_units;
        o[i].pos.y =
            (float)GetRandomValue(game_bounds.shape.y, game_bounds.shape.y +game_bounds.shape.height)
            * 1 +world_units;
        o[i].pos.z = -game_bounds.depth;
        o[i].size.x = (float)GetRandomValue(1, g_obstacle_max_width) * 1 +world_units;
        o[i].size.y = (float)GetRandomValue(1, g_obstacle_max_height) * 1 +world_units;
        o[i].size.z = (float)GetRandomValue(1, g_obstacle_max_depth) * 1 +world_units;

        // bounding box
        o[i].close.min.x = o[i].pos.x +ob_pad;
        o[i].close.min.y = o[i].pos.y +ob_pad;
        o[i].close.min.z = o[i].pos.z +ob_pad;
        o[i].close.max.x = o[i].size.x +ob_pad * 2;
        o[i].close.max.y = o[i].size.y +ob_pad * 2;
        o[i].close.max.z = o[i].size.z +ob_pad * 2;
    }
}

void
despawn_obstacle(struct obstacles *o)
{
    o->active = 0;
}

void
update_obstacle(struct obstacles *o, float frame_time)
{
    for (int i = 0; i < OBSTACLES_LIM; i++) {
        if (!o[i].active) continue;
        o[i].pos.z += frame_time*o[i].speed;
        if (CheckCollisionBoxes(o[i].close, player.collision))
            /* TODO: increment score and say "That was Close!" */;
        if (collide(o[i].pos, o[i].size, player.pos, player.scale))
            /* TODO: go to post-game screen */;
        if (0 <= o[i].pos.z -o[i].size.z)
            despawn_obstacle(&o[i]);
    }
}

void
draw_obstacles(struct obstacles *o)
{
    for (int i = 0; i < OBSTACLES_LIM; i++)
        if (o[i].active) DrawCubeV(o[i].pos, o[i].size, o[i].colr);
}


#endif // _OBSTACLES_H
