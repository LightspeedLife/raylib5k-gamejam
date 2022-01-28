#ifndef _PLAYER_H
#define _PLAYER_H
#include "raylib.h"
#include "globals.h"

void
player_update(struct player *p, Vector2 target, float frame_time)
{
    p->pos.x += frame_time * p->speed * target.x;
    p->pos.y -= frame_time * p->speed * target.y;

    // clamp position
    if (game_bounds.shape.x > p->pos.x) p->pos.x = game_bounds.shape.x;
    if (game_bounds.shape.y > p->pos.y) p->pos.y = game_bounds.shape.y;
    if (game_bounds.shape.x +game_bounds.shape.width < p->pos.x)
        p->pos.x = game_bounds.shape.x +game_bounds.shape.width;
    if (game_bounds.shape.y +game_bounds.shape.height < p->pos.y)
        p->pos.y = game_bounds.shape.y +game_bounds.shape.height;
}

void
player_draw(struct player *p)
{
    DrawModelWiresEx(p->body, p->pos, (Vector3){ 1.0f, 0.0f, 0.0f }, -90.0f, (Vector3){ 0.3f, 0.3f, 0.3f }, WHITE);
}

void
camera_update(Camera *cam, Vector3 target, float frame_time)
{

    cam->position.x += camera_speed*(target.x -cam->position.x)*frame_time;
    cam->position.y += camera_speed*(target.y -cam->position.y)*frame_time;

    // cap camera position
    if (game_bounds.shape.x                     > camera.position.x) camera.position.x = game_bounds.shape.x;
    if (game_bounds.shape.x +game_bounds.shape.width  < camera.position.x) camera.position.x = game_bounds.shape.x +game_bounds.shape.width;
    if (game_bounds.shape.y                     > camera.position.y) camera.position.y = game_bounds.shape.y;
    if (game_bounds.shape.y +game_bounds.shape.height < camera.position.y) camera.position.y = game_bounds.shape.y +game_bounds.shape.height;
}

#endif // _PLAYER_H
