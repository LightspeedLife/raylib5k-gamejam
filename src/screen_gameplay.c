/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
// DONE: Get tunnel/movement
// DONE: Fog shader
// ACTIVE: Obstacles
    // DONE: debug obstacle pool fill
    // DONE: obstacles use fog shader
    // TODO: multi-obstacle spawn
    // TODO: obstacle speed rate adjust
    // TODO: obstacle spawn rate adjust
// TODO: Player movement
// TODO: Collisions
// TODO: invest or something
// TODO: walks
// TODO: open up an HVCU
// TODO: start getting therapy
// TODO: learn to type
// TODO: music

#include <stdio.h>
#include "screens.h"
#include "globals.h"
#include "utils.h"
#include "init.h"

void
debug_obstacles(struct obstacles *o)
{
    char dbg_os[64] = "dbg_obstacles: ";
    for (int i = 0; i < OBSTACLES_LIM; i++)
        dbg_os[i +15] = o[i].active +0x30;
    DrawDebugText(dbg_os, 9);
}

void
spawn_obstacle(struct obstacles *o)
{
    // just randomly create obstacles here
    int i = 0;
    for (; i < OBSTACLES_LIM; i++)
        if (!o[i].active) break;
    if (OBSTACLES_LIM == i) return;

    TraceLog(LOG_DEBUG, "spawning %d", o->id);
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
    }
    TraceLog(LOG_DEBUG, "obstacle %d pos: %f,%f size: %f,%f", i, o[i].pos.x, o[i].pos.y, o[i].size.x, o[i].size.y);
}

void
despawn_obstacle(struct obstacles *o)
{
    TraceLog(LOG_DEBUG, "despawning %d", o->id);
    o->active = 0;
}

void
update_obstacle(struct obstacles *o, float frame_time)
{
    for (int i = 0; i < OBSTACLES_LIM; i++) {
        if (!o[i].active) continue;
        o[i].pos.z += frame_time*o[i].speed;
        if (0 <= o[i].pos.z -o[i].size.z)
            despawn_obstacle(&o[i]);
    }
}

void
draw_obstacles(struct obstacles *o)
{
    BeginShaderMode(shader);
    for (int i = 0; i < OBSTACLES_LIM; i++)
        if (o[i].active) DrawCubeV(o[i].pos, o[i].size, o[i].colr);
    EndShaderMode();
}

char debug_frame_time[64] = "";
char debug_time_since_spawn[64] = "";
static float time_since_spawn = 0.0f;

void
UpdateGameplayScreen(void)
{
    static float spawn_interval = 2.0f;
    float this_frame = GetFrameTime();
    time_since_spawn += this_frame;
    // 7
    sprintf(debug_frame_time, "this_frame: %f", this_frame);
    sprintf(debug_time_since_spawn, "time_since_spawn: %f", time_since_spawn);
    update_obstacle(g_obstacles, this_frame);
    if (spawn_interval <= time_since_spawn) {
        spawn_obstacle(g_obstacles);
        time_since_spawn = 0.0f;
    }
    delta = GetMouseDelta();
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        DisableCursor();
//        static const float delta_bounds = 0.5f;
//        if ( delta_bounds < delta_accum.x) delta_accum.x =  delta_bounds;
//        if (-delta_bounds > delta_accum.x) delta_accum.x = -delta_bounds;
//        if ( delta_bounds < delta_accum.y) delta_accum.y =  delta_bounds;
//        if (-delta_bounds > delta_accum.y) delta_accum.y = -delta_bounds;
        delta_accum = AddVector2(delta_accum, delta);
    } else EnableCursor();
    camera.position.x += camera_speed*(float)delta_accum.x;
    camera.position.y -= camera_speed*(float)delta_accum.y;
    delta_accum.x -= 4*camera_speed*delta_accum.x;
    delta_accum.y -= 4*camera_speed*delta_accum.y;

    // cap camera position
    if (game_bounds.shape.x                     > camera.position.x) camera.position.x = game_bounds.shape.x;
    if (game_bounds.shape.x +game_bounds.shape.width  < camera.position.x) camera.position.x = game_bounds.shape.x +game_bounds.shape.width;
    if (game_bounds.shape.y                     > camera.position.y) camera.position.y = game_bounds.shape.y;
    if (game_bounds.shape.y +game_bounds.shape.height < camera.position.y) camera.position.y = game_bounds.shape.y +game_bounds.shape.height;

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        camera.position.z += delta.y;
    } if ((0.0f == delta.x && 0.0f == delta.y)
          && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        camera.position.z = camera_distance;
    }

    fogDensity -= 0.01f*GetMouseWheelMove();
    SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    if (IsKeyPressed(KEY_UP)) {
        static const unsigned char x = 16;
        tunnelColor.r += x;
        tunnelColor.g += x;
        tunnelColor.b += x;
    } else if (IsKeyPressed(KEY_DOWN)) {
        static const unsigned char x = 16;
        tunnelColor.r -= x;
        tunnelColor.g -= x;
        tunnelColor.b -= x;
    }

    if (IsKeyPressed(KEY_LEFT)) {
        static const float x = 0.1f;
        fogColor.x -= x;
        fogColor.y -= x;
        fogColor.z -= x;
    } else if (IsKeyPressed(KEY_RIGHT)) {
        static const float x = 0.2f;
        fogColor.x += x;
        fogColor.y += x;
        fogColor.z += x;
    }
    SetShaderValue(shader, fogColorLoc, &fogColor, SHADER_UNIFORM_VEC4);
    // Update the light shader with the camera view position
    // SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position.x, SHADER_UNIFORM_VEC3);
}

void
DrawGameplayScreen(void)
{
    ClearBackground(WHITE);
    BeginMode3D(camera); {
//        DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
        DrawCubeWires(cubePosition, 3.0f, 3.0f, 3.0f, tunnelColor);
        DrawModel(tunnel.mo, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, tunnelColor);
        draw_obstacles(g_obstacles);
    }; EndMode3D();

#ifdef _DEBUG
    // 0
    char debug_delta[64] = "";
    sprintf(debug_delta, "delta x:%f, y:%f", delta.x, delta.y);
    DrawDebugText(debug_delta, 0);

    // 1
    char debug_delta_accum[64] = "";
    sprintf(debug_delta_accum, "accum: <%f,%f>", delta_accum.x, delta_accum.y);
    DrawDebugText(debug_delta_accum, 1);

    // 2
    char debug_bounds[64] = "";
    sprintf(debug_bounds, "bounds: <%f,%f> %fx%f",
        game_bounds.shape.x, game_bounds.shape.y, game_bounds.shape.width, game_bounds.shape.height);
    DrawDebugText(debug_bounds, 2);

    // 3
    char debug_camera_position[64] = "";
    float x = camera.position.x,
        y = camera.position.y,
        z = camera.position.z;
    sprintf(debug_camera_position, "cam_pos: x:%f, y:%f, z:%f", x, y, z);
    DrawDebugText(debug_camera_position, 3);

    // 4
    char debug_fog_density[64] = "";
    sprintf(debug_fog_density, "fog_density: %f", fogDensity);
    DrawDebugText(debug_fog_density, 4);

    // 5
    char debug_fog_color[64] = "";
    sprintf(debug_fog_color, "fog_color: x:%f y:%f z:%f",
        fogColor.x, fogColor.y, fogColor.z);
    DrawDebugText(debug_fog_color, 5);

    // 6
    char debug_tunnel_color[64] = "";
    sprintf(debug_tunnel_color, "tunnel_color: r:%u r:%u b:%u",
            tunnelColor.r, tunnelColor.g, tunnelColor.b);
    DrawDebugText(debug_tunnel_color, 6);

    // 7
    DrawDebugText(debug_frame_time, 7);

    // 8
    DrawDebugText(debug_time_since_spawn, 8);

    debug_obstacles(g_obstacles);
#endif // _DEBUG
}

// Gameplay Screen Unload logic
void
UnloadGameplayScreen(void)
{
    UnloadModel(tunnel.mo);
    UnloadShader(shader);
}

// Gameplay Screen should finish?
int
FinishGameplayScreen(void)
{
    return finishScreen;
}
