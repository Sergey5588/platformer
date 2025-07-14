
#include "raylib.h"
#include "raymath.h"
#include "box2d/box2d.h"
#include "stdbool.h"
#include "stdio.h"
#include "math.h"
#define BODY_COUNT 3

#define M_PI 3.14159265358979323846
typedef struct Actor
{
    bool isStatic;
    Rectangle hitbox;
    Texture2D sprite;
    b2BodyId bodyId;
    Color color; // if presented
} Actor;



void initActor(Actor* actor, bool isStatic, Rectangle hitbox,Texture2D sprite, Color color, b2WorldId* worldId) {
    actor->isStatic = isStatic;
    actor->color = color;
    actor->sprite = sprite;
    actor->hitbox = hitbox;
    
    b2BodyDef BodyDef = b2DefaultBodyDef();
    if(isStatic == false) BodyDef.type = b2_dynamicBody;
    BodyDef.fixedRotation = true;
    BodyDef.position = (b2Vec2){hitbox.x, hitbox.y};
    b2BodyId bodyId = b2CreateBody(*worldId, &BodyDef);
    actor->bodyId = bodyId;
    b2Polygon bodyBox = b2MakeBox(hitbox.width/2 , hitbox.height/2); 
    b2ShapeDef ShapeDef = b2DefaultShapeDef();
    ShapeDef.density = 1.0f;
    ShapeDef.material.friction = 0.0f;
    b2CreatePolygonShape(bodyId, &ShapeDef, &bodyBox);

}


void def_init(Actor scene[BODY_COUNT], b2WorldId worldId) {
    for(int i = 0; i < BODY_COUNT; i++) {
        initActor(&scene[i], true, (Rectangle){1,1,1,1}, (Texture2D){0}, RED, &worldId);
    }
}

void get_axis(Vector2* input_dir) {
    
    if(IsKeyDown(KEY_W)) {
        input_dir->y = -1;
    }else if(IsKeyDown(KEY_S)) {
        input_dir->y = 1;
    }

    if(IsKeyDown(KEY_D)) {
        input_dir->x = 1;
    }else if(IsKeyDown(KEY_A)) {
        input_dir->x = -1;
    }

    input_dir->x = Vector2Normalize(*input_dir).x;
    input_dir->y = Vector2Normalize(*input_dir).y;

    
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    const int virtualScreenWidth = 160;
    const int virtualScreenHeight = 90;

    const float virtualRatio = (float)screenWidth/(float)virtualScreenWidth;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - smooth pixel-perfect camera");

    Camera2D worldSpaceCamera = { 0 };  // Game world camera
    worldSpaceCamera.zoom = 1.0f;

    Camera2D screenSpaceCamera = { 0 }; // Smoothing camera
    screenSpaceCamera.zoom = 1.0f;

    RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight); // This is where we'll draw all our objects.


    // The target's height is flipped (in the source Rectangle), due to OpenGL reasons
    Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
    Rectangle destRec = { -virtualRatio, -virtualRatio, screenWidth + (virtualRatio*2), screenHeight + (virtualRatio*2) };

    Vector2 origin = { 0.0f, 0.0f };

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 10.0f};

    b2WorldId worldId = b2CreateWorld(&worldDef);
    
    Image placeholder_img = LoadImage("assets/placeholder.png"); // bob the placeholder
    Texture2D placholder = LoadTextureFromImage(placeholder_img);

    Actor scene[BODY_COUNT];
    def_init(scene, worldId);
    initActor(&scene[0], false, (Rectangle){80,0, 16,16}, placholder, (Color){0,0,0,0}, &worldId);
    initActor(&scene[1], true, (Rectangle){80,45, 100,10}, (Texture2D){0}, RED, &worldId);

    float timeStep = 1.0f / 120.0f;
    int subStepCount = 4;
    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        b2World_Step(worldId, timeStep, subStepCount);

        Vector2 input_dir = (Vector2){0.0, 0.0};
        b2Vec2 velocity = b2Body_GetLinearVelocity(scene[0].bodyId);
        get_axis(&input_dir);

        b2Body_SetLinearVelocity(scene[0].bodyId, ((b2Vec2){input_dir.x * 2 + velocity.x, input_dir.y*2+velocity.y}));
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);

            BeginMode2D(worldSpaceCamera);
                for(int i = 0; i < BODY_COUNT; i++) {
                    b2Vec2 pos = b2Body_GetPosition(scene[i].bodyId);
                    Rectangle rect = (Rectangle){pos.x,pos.y, scene[i].hitbox.width, scene[i].hitbox.height};
                    DrawRectanglePro(rect,(Vector2){scene[i].hitbox.width/2,scene[i].hitbox.height/2}, asin(b2Body_GetRotation(scene[i].bodyId).s)*(180.0f/M_PI), scene[i].color);
                    DrawTextureRec(scene[i].sprite, (Rectangle){0,0, rect.width, rect.height},(Vector2){rect.x - scene[i].hitbox.width/2, rect.y-scene[i].hitbox.height/2}, WHITE);
                }
            
            EndMode2D();
        EndTextureMode();

        BeginDrawing();
            ClearBackground(RED);

            BeginMode2D(screenSpaceCamera);
                DrawTexturePro(target.texture, sourceRec, destRec, origin, 0.0f, WHITE);
            EndMode2D();

            DrawText(TextFormat("Screen resolution: %ix%i", screenWidth, screenHeight), 10, 10, 20, DARKBLUE);
            DrawText(TextFormat("World resolution: %ix%i", virtualScreenWidth, virtualScreenHeight), 10, 40, 20, DARKGREEN);
            DrawFPS(GetScreenWidth() - 95, 10);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);    // Unload render texture

    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}