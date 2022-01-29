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

#include <stdio.h>
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
// DONE: collision -- now it's ACTUALLY DONE!
// DONE: Player movement
// TODO: music

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
draw_score()
{
    char score_str[64] = "";
    static const unsigned score_fontsize = 26;
    static const Color score_color = LIME;
    sprintf(score_str, "%lu", score);
    DrawText(score_str,
             GetScreenWidth() -MeasureText(score_str, score_fontsize) -10,
             10, score_fontsize, score_color);
}

void
draw_multiplier()
{
    char multiplier_str[64] = "x";
    static const unsigned int multiplier_fontsize = 26;
    static const Color multiplier_color = LIME;
    sprintf(multiplier_str +sizeof(char), "%u", multiplier);
    DrawText(multiplier_str,
             GetScreenWidth() -MeasureText(multiplier_str, multiplier_fontsize) -10,
             45, multiplier_fontsize, multiplier_color);
}

void
draw_thatwasclose(float *level, float this_frame)
{
    static const char thatwasclose[] = "that was close!";
    static const unsigned int thatwasclose_size = 56;
    static const float distance = 5.0f;
    static const unsigned int min = 80;
    DrawText(thatwasclose, (GetScreenWidth() / 2) -(MeasureText(thatwasclose, thatwasclose_size) / 2),
             (long)level * distance +min, thatwasclose_size, GOLD);
    *level -= this_frame;
}

void
update_score(float this_frame)
{
    static float this_tick = 0.0f;
    this_tick += this_frame;
    sec_accum += this_frame;

    if (sec_per_inc < sec_accum) {
        multiplier++;
        score += 10;
        sec_accum = 0.0f;
    }
    if (PLAYER_CLOSE == player.side) {
        if (tick < this_tick) {
            score += multiplier;
            this_tick = 0.0f;
        }
    }
}

void
UpdateGameplayScreen(void)
{
    delta = GetMouseDelta();
    static float spawn_interval = 0.3f;
    float this_frame = GetFrameTime();
    time_since_spawn += this_frame;

    update_score(this_frame);
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
    float this_frame = GetFrameTime();
    ClearBackground(WHITE);
    BeginMode3D(camera); {
        DrawModel(tunnel.mo, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, tunnelColor);
        draw_obstacles(g_obstacles);
        player_draw(&player);
    }; EndMode3D();
    draw_score();
    draw_multiplier();
    if (0.0f < should_draw_thatwasclose)
        draw_thatwasclose(&should_draw_thatwasclose, this_frame);

#ifdef _DEBUG
    DrawDebugText(1, "should draw? %f", should_draw_thatwasclose);
    DrawDebugText(2, "player state? %s", ((PLAYER_OUT == player.side)? "OUT"
                                          : (PLAYER_CLOSE == player.side)? "CLOSE"
                                          : "IN"));
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
