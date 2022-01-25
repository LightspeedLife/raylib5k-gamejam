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
// TODO: Get tunnel/movement
// DONE: Fog shader
// TODO: Player movement
// TODO: Obstacles
// TODO: Čollisions
// TODO: music

#include <stdio.h>
#include "screens.h"
#include "globals.h"
#include "utils.h"
#include "init.h"

void
UpdateGameplayScreen(void)
{
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
    if (game_bounds.x                     > camera.position.x) camera.position.x = game_bounds.x;
    if (game_bounds.x +game_bounds.width  < camera.position.x) camera.position.x = game_bounds.x +game_bounds.width;
    if (game_bounds.y                     > camera.position.y) camera.position.y = game_bounds.y;
    if (game_bounds.y +game_bounds.height < camera.position.y) camera.position.y = game_bounds.y +game_bounds.height;

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
    ClearBackground(BLACK);
    BeginMode3D(camera); {
//        DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
        DrawCubeWires(cubePosition, 3.0f, 3.0f, 3.0f, tunnelColor);
        DrawModel(tunnel.mo, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, tunnelColor);

#ifdef _DEBUG
        DrawGrid(100, 1.0f);
#endif // _DEBUG
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
        game_bounds.x, game_bounds.y, game_bounds.width, game_bounds.height);
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
