#ifndef _GLOBALS_H
#define _GLOBALS_H
#include "raylib.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

#define OBSTACLES_LIM 32

static enum { GAME_NEW, GAME_ONGOING, GAME_OVER, GAME_AGAIN } game_state = GAME_NEW;
static int framesCounter = 0;
static int finishScreen = 0;
static const float tick = 1.0f/30;
static unsigned long long tick_count = 0;
static long long score = 0;
static long long high_score = 0;
static long long highest_score = 0;
static unsigned int multiplier = 1;
static float speed = 16.0f;
static float sec_per_inc = 5.0f;
static float sec_accum = 0.0f;
static int should_draw_thatwasclose = 0.0f;
static float close_float = 0.0f;
static int should_shake_screen = 0;
static float shake_float = 0.0f;
static Camera3D camera = { 0 };
static float camera_distance = 0.0f;
static float camera_target = -1000.0f;
static Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
static Vector2 delta = { 0 };
static Vector2 delta_accum = { 0 };
static Vector2 slew = { 0 };
static float camera_speed = 8.2f;
static struct game_bounds {
    Rectangle shape;
    float depth;
    float padding;
} game_bounds;
float world_units;
static Color tunnelColor = { 240, 240, 240, 255 };

// Load shader and set up some uniforms
Shader shader;
float fogDensity = 0.15f;
Vector4 fogAmbient = { 0.2f, 0.2f, 0.2f, 1.0f };
int fogColorLoc;
Vector4 fogColor = { 1.0f, 1.0f, 1.0f, 1.0f };
int fogDensityLoc;

static struct tunnel {
    Model mo;
} tunnel;

static struct obstacles {
    int id;
    int active;
    Vector3 pos;
    Vector3 size;
    BoundingBox body;
    BoundingBox close;
    float speed;
    Color colr;
} g_obstacles[OBSTACLES_LIM];
Color obs_state_color[2] = { GREEN, RED };
float ob_pad = 0.25f;

static int g_obstacle_max_width,
           g_obstacle_max_height,
           g_obstacle_max_depth;

static struct player {
    Vector3 pos;
    Vector3 scale;
    BoundingBox collision;
    float speed;
    enum { PLAYER_OUT, PLAYER_CLOSE, PLAYER_IN } side;
} player;

#endif // _GLOBALS_H
