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
// TODO: Get tunnel/movement
// TODO: Player movement
// TODO: Obstacles
// TODO: music

#include <stdio.h>
#include "screens.h"

static int framesCounter = 0;
static int finishScreen = 0;
static Camera3D camera = { 0 };
static float camera_distance = 10.0f;
static float camera_target = -1000.0f;
static Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
static Vector2 delta = { 0 };
static Vector2 delta_accum = { 0 };
static Vector2 slew = { 0 };
static float camera_speed = 0.06f;
static Rectangle game_bounds = { 0 };

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
    Image checked = GenImageChecked(2, 2, 1, 1, RED, GREEN);
    Texture2D texture = LoadTextureFromImage(checked);
    UnloadImage(checked);
    tunnel->mo.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
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
    game_bounds.width = 17.5f;
    game_bounds.height = 10.0f;
    game_bounds.x = -(width/2);
    game_bounds.y = -(height/2);

}

void
InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;

    set_game_bounds(17.5f, 10.0f);

    camera.position = (Vector3){ 0.0f, 10.0f, camera_distance };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, camera_target };    // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type
#ifdef _DEBUG
    SetCameraMode(camera, CAMERA_FIRST_PERSON); // Set a first person camera mode
#endif // _DEBUG

    slew = (Vector2){ 1.4f, 1.4f }; // FIXME: Use or lose

    cubePosition.x = 0.0f;
    cubePosition.y = 0.0f;
    cubePosition.z = 0.0f;

    init_tunnel(&tunnel);
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
}

void
DrawGameplayScreen(void)
{
    ClearBackground(RAYWHITE);
    BeginMode3D(camera); {
//        DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
        DrawCubeWires(cubePosition, 3.0f, 3.0f, 3.0f, MAROON);
        DrawModelWires(tunnel.mo, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);

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
#endif // _DEBUG
}

// Gameplay Screen Unload logic
void
UnloadGameplayScreen(void)
{
}

// Gameplay Screen should finish?
int
FinishGameplayScreen(void)
{
    return finishScreen;
}
