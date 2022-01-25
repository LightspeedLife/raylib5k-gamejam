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
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
// TODO: Get tunnel/movement
// TODO: Fog shader
// TODO: Player movement
// TODO: Obstacles
// TODO: Čollisions
// TODO: music

#include <stdio.h>
#include "screens.h"

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
static Rectangle game_bounds = { 0 };
static Color tunnelColor = { 240, 240, 240, 255 };

// Load shader and set up some uniforms
Shader shader;
float fogDensity = 0.15f;
Vector4 fogAmbient = { 0.2f, 0.2f, 0.2f, 1.0f };
int fogColorLoc;
Vector4 fogColor = { 0.0f, 0.0f, 0.0f, 1.0f };
int fogDensityLoc;

static void
init_shader(Shader *shader)
{
    *shader = LoadShader(TextFormat("resources/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                                TextFormat("resources/shaders/glsl%i/fog.fs", GLSL_VERSION));
    shader->locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(*shader, "matModel");
    shader->locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(*shader, "viewPos");
    shader->locs[SHADER_LOC_COLOR_AMBIENT] = GetShaderLocation(*shader, "ambient");

    fogColorLoc = GetShaderLocation(*shader, "fogColor");
    SetShaderValue(*shader, fogColorLoc, &fogColor, SHADER_UNIFORM_VEC4);
    // Ambient light level
    // int ambientLoc = GetShaderLocation(*shader, "ambient");
    // SetShaderValue(*shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);
    SetShaderValue(*shader, shader->locs[SHADER_LOC_COLOR_AMBIENT], &fogAmbient, SHADER_UNIFORM_VEC4);

    fogDensityLoc = GetShaderLocation(*shader, "fogDensity");
    SetShaderValue(*shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);
}

static struct tunnel {
    Model mo;
} tunnel;

static Mesh
tunnel_gen_mesh(const Rectangle size, float depth)
{
    Mesh mesh = { 0 };
    mesh.triangleCount = 8; //8; // 2*4 walls
    mesh.vertexCount = mesh.triangleCount*3; //16; // 4 walls, 4 verts each
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));    // 3 vertices, 3 coordinates each (x, y, z)
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount*2*sizeof(float));   // 3 vertices, 2 coordinates each (x, y)
    mesh.normals = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));     // 3 vertices, 3 coordinates each (x, y, z)

    static float padding = 2.0f;
    #include "tunnel_verts.txt"

    // Upload mesh data from CPU (RAM) to GPU (VRAM) memory
    UploadMesh(&mesh, false);

    return mesh;
}

static void
init_tunnel(struct tunnel *tunnel)
{
    tunnel->mo = LoadModelFromMesh(tunnel_gen_mesh(game_bounds, 40));
    Image i = GenImageColor(1, 1, WHITE);
    Texture texture = LoadTextureFromImage(i);
    tunnel->mo.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    UnloadImage(i);
    UnloadTexture(texture);
}

Vector2
AddVector2(const Vector2 a, const Vector2 b)
{
    Vector2 re = { 0 };
    re.x = a.x +b.x;
    re.y = a.y +b.y;
    return re;
}

Vector2
SubVector2(const Vector2 a, const Vector2 b)
{
    Vector2 re = { 0 };
    re.x = a.x -b.x;
    re.y = a.y -b.y;
    return re;
}

static void
DrawDebugText(const char *text, unsigned int index)
{
    static const unsigned int debug_height_min = 28;
    static const unsigned int font_size = 16;
    DrawText(text, 10, index*font_size +debug_height_min, font_size, DARKGREEN);
}

static void
set_game_bounds(float width, float height)
{
    game_bounds.width = width;
    game_bounds.height = height;
    game_bounds.x = -(width/2);
    game_bounds.y = -(height/2);

}

void
InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;

    set_game_bounds(6.0f, 3.0f);

    camera.position = (Vector3){ 0.0f, 10.0f, camera_distance };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, camera_target };    // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

    slew = (Vector2){ 1.4f, 1.4f }; // FIXME: Use or lose

    cubePosition.x = 0.0f;
    cubePosition.y = 0.0f;
    cubePosition.z = 0.0f;

    init_tunnel(&tunnel);
    init_shader(&shader);
    tunnel.mo.materials[0].shader = shader;
    CreateLight(LIGHT_POINT, (Vector3){ 0, 2, 6 }, Vector3Zero(), WHITE, shader);
}

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
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position.x, SHADER_UNIFORM_VEC3);
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
