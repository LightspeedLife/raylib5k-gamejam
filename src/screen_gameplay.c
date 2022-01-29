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
// DONE: scoring
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
    sprintf(score_str, "%lld", score);
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
    static const unsigned int thatwasclose_size = 48;
    static const float distance = 200.0f;
    static const unsigned int min = 150;
    DrawText(thatwasclose, (GetScreenWidth()/2) -(MeasureText(thatwasclose, thatwasclose_size)/2),
             *level * distance +(GetScreenHeight()/2 -distance -min), thatwasclose_size, GOLD);
//    DrawText("test", (GetScreenWidth()/2), (GetScreenHeight()/2), thatwasclose_size, GOLD);
    *level -= this_frame;
}

void
shake_screen(float *brrr, float this_frame)
{
    static const float amp = 0.1f;
    camera.position.x += (float)GetRandomValue(0, 100)/100 * amp -(amp/2);
    camera.position.y += (float)GetRandomValue(0, 100)/100 * amp -(amp/2);
    *brrr -= this_frame;
}

void
update_score(float this_frame)
{
    // NOTE: maybe add score/lose sounds here?
    static float this_tick = 0.0f;
    static int was_close = 0;
    this_tick += this_frame;
    sec_accum += this_frame;

    switch (player.side) {
        case PLAYER_CLOSE: {
            if (!was_close) {
                was_close = 1;
                close_float = 1.0f;
            }
            if (tick < this_tick) score += multiplier;
        } break;
        case PLAYER_IN: {
            was_close = 0;
            if (1 < multiplier) {
                score -= multiplier * 10;
                sec_accum = 0.0f;
                multiplier = 1; // :'C  I weep for thee
            } else score -= 10;
            if (0 < score) {
                should_shake_screen = 1;
                shake_float = 0.5f;
            }
        } break;
        default: {
            if (was_close) should_draw_thatwasclose = 1;
            was_close = 0;
        }
    }
    if (tick < this_tick) this_tick = 0.0f;
    if (sec_per_inc < sec_accum) {
        multiplier++;
        score += 100;
        sec_accum = 0.0f;
    }
    if (score > high_score) high_score = score;
    if (high_score > highest_score) highest_score = high_score;
    if (0.0f > score) game_state = GAME_OVER;
}

void
reset(void)
{
    framesCounter = 0;
    finishScreen = 0;
    should_shake_screen = 0;
    score = high_score = 0;
    camera.position = (Vector3){ 0.0f, 0.0f, camera_distance };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, camera_target };    // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type
    init_obstacles(g_obstacles);
    init_player();
}

void UpdateGameplayScreen(void);

void
update_game_state(void)
{
    switch (game_state) {
        case GAME_NEW: {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                game_state = GAME_ONGOING;
        } break;
        case GAME_ONGOING: {
            UpdateGameplayScreen();
        } break;
        case GAME_OVER: {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                game_state = GAME_AGAIN;
        } break;
        case GAME_AGAIN: {
            reset();
            game_state = GAME_ONGOING;
        }
        default:;
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
        player_update(&player, delta, this_frame);
    }
    camera_update(&camera, player.pos, this_frame);

#if _DEBUG
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
#endif // 0

    SetShaderValue(shader, fogColorLoc, &fogColor, SHADER_UNIFORM_VEC4);
    // Update the light shader with the camera view position
    // SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position.x, SHADER_UNIFORM_VEC3);
}

void DrawGameplayScreen(void);
void draw_endgame(void);
void draw_newgame(void);
static float endgame_prompt_counter;

void
draw_game(void)
{
    switch (game_state) {
    case GAME_NEW: {
        draw_newgame();
    } break;
    case GAME_ONGOING: {
        DisableCursor();
        endgame_prompt_counter = 2.0f;
        DrawGameplayScreen();
    } break;
    case GAME_OVER: {
        draw_endgame();
    } break;
    default:;
    }
}

void
draw_newgame(void)
{
    ClearBackground(WHITE);
    BeginMode3D(camera); {
        DrawModel(tunnel.mo, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, tunnelColor);
        draw_obstacles(g_obstacles);
        player_draw(&player);
    }; EndMode3D();
    {
        static char prompt[32] = "Click or tap to begin.";
        static const unsigned int font_size = 36;
        unsigned int text_len = MeasureText(prompt, font_size);
        DrawText(prompt, GetScreenWidth()/2 -text_len/2, GetScreenHeight()/2, font_size, GREEN);
    }
}

void
draw_endgame(void)
{
    ClearBackground(WHITE);
    BeginMode3D(camera); {
        DrawModel(tunnel.mo, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, tunnelColor);
        draw_obstacles(g_obstacles);
        player_draw(&player);
    }; EndMode3D();
    { // "game over"
        static const char game_over[] = "GAME OVER";
        static const unsigned int font_size = 64;
        unsigned int text_len = MeasureText(game_over, font_size);
        DrawText("GAME OVER", GetScreenWidth()/2 -text_len/2,
                GetScreenHeight()/4, font_size, GOLD);
    } {
        static char your_score[128] = "Your Score Was: ";
        static const unsigned int font_size = 48;
        sprintf(your_score +16, "%lld", high_score);
        unsigned int text_len = MeasureText(your_score, font_size);
        DrawText(your_score, GetScreenWidth()/2 -text_len/2, GetScreenHeight()/2, font_size, GOLD);
    } {
        static char your_score[128] = "Highest: ";
        static const unsigned int font_size = 48;
        sprintf(your_score +9, "%lld", highest_score);
        unsigned int text_len = MeasureText(your_score, font_size);
        DrawText(your_score, GetScreenWidth()/2 -text_len/2, GetScreenHeight()/2 +font_size +10, font_size, GOLD);
    }
    if (0.0f > (endgame_prompt_counter -= GetFrameTime())) {
        static char prompt[32] = "Click or tap to try again.";
        static const unsigned int font_size = 36;
        unsigned int text_len = MeasureText(prompt, font_size);
        DrawText(prompt, GetScreenWidth()/2 -text_len/2, 3 * (GetScreenHeight()/4), font_size, GREEN);
    }
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
    if (should_shake_screen) shake_screen(&shake_float, this_frame);
    if (should_draw_thatwasclose) draw_thatwasclose(&close_float, this_frame);
    if (0.0f >= close_float) should_draw_thatwasclose = 0;
    if (0.0f >= shake_float) should_shake_screen = 0;
    draw_multiplier();

#ifdef _DEBUG
    DrawDebugText(1, "should shake? %i", should_shake_screen);
    DrawDebugText(2, "shake float? %f", shake_float);
    DrawDebugText(3, "player state? %s", ((PLAYER_OUT == player.side)? "OUT"
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
