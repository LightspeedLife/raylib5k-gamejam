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
// TODO: Pickups
// TODO: scoring
// DONE: Player movement
// TODO: music

#include <stdio.h>
#include "screens.h"
#include "globals.h"
#include "utils.h"
#include "init.h"
#include "obstacles.h"
#include "player.h"

char debug_frame_time[64] = "";
char debug_time_since_spawn[64] = "";
static float time_since_spawn = 0.0f;

void
UpdateGameplayScreen(void)
{
    delta = GetMouseDelta();
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
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        DisableCursor();
        player_update(&player, delta, this_frame);
    } else EnableCursor();
    camera_update(&camera, player.pos, this_frame);

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
        player_draw(&player);
    }; EndMode3D();
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
