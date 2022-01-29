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

    struct obstacles *obj = &o[i];
    obj->active = 1;
    {
        obj->pos.x =
            (float)GetRandomValue(game_bounds.shape.x, game_bounds.shape.x +game_bounds.shape.width)
            * 1 +world_units;
        obj->pos.y =
            (float)GetRandomValue(game_bounds.shape.y, game_bounds.shape.y +game_bounds.shape.height)
            * 1 +world_units;
        obj->pos.z = -game_bounds.depth;
        obj->size.x = (float)GetRandomValue(1, g_obstacle_max_width) * 1 +world_units;
        obj->size.y = (float)GetRandomValue(1, g_obstacle_max_height) * 1 +world_units;
        obj->size.z = (float)GetRandomValue(1, g_obstacle_max_depth) * 1 +world_units;

        obj->body.min.x = obj->pos.x -(obj->size.x/2);
        obj->body.min.y = obj->pos.y -(obj->size.y/2);
        obj->body.min.z = obj->pos.z -(obj->size.z/2);

        obj->body.max.x = obj->pos.x +(obj->size.x/2);
        obj->body.max.y = obj->pos.y +(obj->size.y/2);
        obj->body.max.z = obj->pos.z +(obj->size.z/2);

/*
        float min_x, min_y, min_z,
              max_x, max_y, max_z;
        min_x = -game_bounds.shape.x/2;                      max_x = game_bounds.shape.x/2;
        min_y = -game_bounds.shape.y/2;                      max_y = game_bounds.shape.y/2;
        min_z = -game_bounds.depth -(game_bounds.depth/2);   max_z = -game_bounds.depth;

        obj->body.min.x = ((float)GetRandomValue(0, 10)/10.0f) * min_x;
        obj->body.min.y = ((float)GetRandomValue(0, 20)/20.0f) * min_y;
        obj->body.min.z = ((float)GetRandomValue(1, 20)/20.0f) * min_z;

        obj->body.max.x = ((float)GetRandomValue(1, 10)/10.0f) * max_x;
        obj->body.max.y = ((float)GetRandomValue(1, 20)/20.0f) * max_y;
        obj->body.max.z = ((float)GetRandomValue(0, 20)/20.0f) * max_z;
*/

        // bounding box
        o[i].close.min.x = o[i].pos.x -(o[i].size.x / 2) -ob_pad;
        o[i].close.min.y = o[i].pos.y -(o[i].size.y / 2) -ob_pad;
        o[i].close.min.z = o[i].pos.z -(o[i].size.z / 2) -ob_pad;
        o[i].close.max.x = o[i].pos.x +(o[i].size.x / 2) +ob_pad;
        o[i].close.max.y = o[i].pos.y +(o[i].size.y / 2) +ob_pad;
        o[i].close.max.z = o[i].pos.z +(o[i].size.z / 2) +ob_pad;
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
        o[i].body.min.z += frame_time * speed;
        o[i].body.max.z += frame_time * speed;
        o[i].close.min.z += frame_time * speed;
        o[i].close.max.z += frame_time * speed;

        if (player.collision.min.z <= o[i].close.max.z) { // player can collide
            o[i].colr = GREEN;
            if (CheckCollisionBoxes(o[i].body, player.collision)) {
                player.side = PLAYER_IN;
                o[i].colr = BLUE;
                /* TODO: go to post-game screen */
            } else if (CheckCollisionBoxes(o[i].close, player.collision)) {
                player.side = PLAYER_CLOSE;
                o[i].colr = PURPLE;
                /* ACTIVE: increment score and say "That was Close!" */
            } else player.side = PLAYER_OUT;
        } else o[i].colr = RED;
        if (0 <= o[i].body.min.z)
            despawn_obstacle(&o[i]);
    }
}

void
draw_obstacles(struct obstacles *o)
{
    for (int i = 0; i < OBSTACLES_LIM; i++)
        if (o[i].active)
//            DrawCubeV(o[i].pos , o[i].size, o[i].colr),
            DrawBoundingBoxVolume(o[i].body, o[i].colr),
            DrawBoundingBoxVolumeWires(o[i].close, BLUE),
            DrawCubeV(o[i].close.min, (Vector3){ 0.3f, 0.3f, 0.3f }, DARKGREEN),
            DrawCubeV(o[i].close.max, (Vector3){ 0.3f, 0.3f, 0.3f }, DARKBLUE);
}


#endif // _OBSTACLES_H
