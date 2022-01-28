#ifndef _INIT_H
#define _INIT_H
#include "raymath.h"
#define  RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "raylib.h"
#include "globals.h"

static void
init_obstacles(struct obstacles *obstacles)
{
	for (int i = 0; i < OBSTACLES_LIM; i++) {
        obstacles[i].id = i;
		obstacles[i].active = 0;
        obstacles[i].speed = 24.0;
        obstacles[i].colr = RED;
	}
}

static void
init_shader(Shader *shader)
{
    *shader = LoadShader(TextFormat("resources/shaders/glsl100/base_lighting.vs"),
                                TextFormat("resources/shaders/glsl100/fog.fs"));
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

static Mesh
tunnel_gen_mesh(const Rectangle size, float depth)
{
    Mesh mesh = { 0 };
    mesh.triangleCount = 8; //8; // 2*4 walls
    mesh.vertexCount = mesh.triangleCount*3; //16; // 4 walls, 4 verts each
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));    // 3 vertices, 3 coordinates each (x, y, z)
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount*2*sizeof(float));   // 3 vertices, 2 coordinates each (x, y)
    mesh.normals = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));     // 3 vertices, 3 coordinates each (x, y, z)

    float padding = game_bounds.padding;
    #include "tunnel_verts.txt" // uses depth

    // Upload mesh data from CPU (RAM) to GPU (VRAM) memory
    UploadMesh(&mesh, false);

    return mesh;
}

static void
init_tunnel(struct tunnel *tunnel)
{
    Mesh m = tunnel_gen_mesh(game_bounds.shape, game_bounds.depth);
    tunnel->mo = LoadModelFromMesh(m);
    Image i = GenImageColor(1, 1, WHITE);
    Texture texture = LoadTextureFromImage(i);
    tunnel->mo.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
//    UnloadImage(i);
//    UnloadTexture(texture);
//    UnloadMesh(m);
}

static void
set_game_bounds(float width, float height, float depth, float padding)
{
    game_bounds.shape.width = width;
    game_bounds.shape.height = height;
    game_bounds.shape.x = -(width/2);
    game_bounds.shape.y = -(height/2);
    game_bounds.depth = depth;
    game_bounds.padding = padding;
}

void
init_player()
{
    Mesh m = GenMeshCone(0.5f, 1.5f, 12);
    UploadMesh(&m, false);
    player.body = LoadModelFromMesh(m);
    player.transform = MatrixRotate((Vector3){ 1.0f, 0.0f, 0.0f }, -(2*PI)/4);
    player.pos = (Vector3){ 0.0f, 0.0f, -2.0f };
    Image img = GenImageColor(1, 1, GOLD);
    Texture txr = LoadTextureFromImage(img);
    player.body.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = txr;
 //   UnloadImage(img);
 //   UnloadTexture(txr);
    player.speed = 0.2f;
}

void
InitGameplayScreen(void)
{
    SetTraceLogLevel(LOG_DEBUG);
    framesCounter = 0;
    finishScreen = 0;

    SetRandomSeed((unsigned int)GetTime());
    set_game_bounds(6.0f, 3.0f, 40.0f, 0.2f);

    camera.position = (Vector3){ 0.0f, 0.0f, camera_distance };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, camera_target };    // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

    slew = (Vector2){ 4.0f, 4.0f }; // FIXME: Use or lose

    cubePosition = (Vector3){ 0.0f, 0.0f, 0.0f };

    init_tunnel(&tunnel);
    init_shader(&shader);
    tunnel.mo.materials[0].shader = shader;
    CreateLight(LIGHT_POINT, (Vector3){ 0, 2, 6 }, Vector3Zero(), WHITE, shader);

    g_obstacle_max_depth = 20;
    g_obstacle_max_width = 3;
    g_obstacle_max_height = 3;
    init_obstacles(g_obstacles);

    init_player();
}


#endif // _INIT_H
