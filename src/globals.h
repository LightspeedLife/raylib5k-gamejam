#ifndef _GLOBALS_H
#define _GLOBALS_H
#include "raylib.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

static int framesCounter = 0;
static int finishScreen = 0;
static Camera3D camera = { 0 };
static float camera_distance = 1.0f;
static float camera_target = -1000.0f;
static Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
static Vector2 delta = { 0 };
static Vector2 delta_accum = { 0 };
static Vector2 slew = { 0 };
static float camera_speed = 0.06f;
static struct game_bounds {
    Rectangle shape;
    float depth;
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

#define OBSTACLES_LIM 16
#define OBSTACLES_IS_OOB(ptr) ((g_obstacles >= (ptr)) || (g_obstacles +OBSTACLES_LIM <= (ptr)))

static struct obstacles {
    int active;
    Vector3 pos;
    Vector3 size;
    float speed;
} g_obstacles[OBSTACLES_LIM];

static int g_obstacle_max_width,
           g_obstacle_max_height,
           g_obstacle_max_depth;

#endif // _GLOBALS_H
