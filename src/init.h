#ifndef _INIT_H
#define _INIT_H
#define  RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "raylib.h"
#include "globals.h"

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


#endif // _INIT_H
