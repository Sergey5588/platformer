
#include "raylib.h"
#include "box2d/box2d.h"
#include "stdbool.h"
#include "stdio.h"
#include "math.h"
#define BODY_COUNT 2

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
    BodyDef.position = (b2Vec2){hitbox.x, hitbox.y};
    b2BodyId bodyId = b2CreateBody(*worldId, &BodyDef);
    actor->bodyId = bodyId;
    b2Polygon bodyBox = b2MakeBox(hitbox.height/2 , hitbox.width/2); 
    b2ShapeDef ShapeDef = b2DefaultShapeDef();
    ShapeDef.density = 1.0f;
    ShapeDef.material.friction = 0.0f;
    b2CreatePolygonShape(bodyId, &ShapeDef, &bodyBox);

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


    Actor scene[BODY_COUNT];
    initActor(&scene[0], false, (Rectangle){75,0, 10,10}, (Texture2D){0}, BLACK, &worldId);
    initActor(&scene[1], true, (Rectangle){80,45, 10,10}, (Texture2D){0}, RED, &worldId);

    float timeStep = 1.0f / 60.0f;
    int subStepCount = 4;
    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        b2World_Step(worldId, timeStep, subStepCount);


        

        BeginTextureMode(target);
            ClearBackground(RAYWHITE);

            BeginMode2D(worldSpaceCamera);
                for(int i = 0; i < BODY_COUNT; i++) {
                    b2Vec2 pos = b2Body_GetPosition(scene[i].bodyId);
                    DrawRectanglePro((Rectangle){pos.x,pos.y, scene[i].hitbox.width, scene[i].hitbox.height},(Vector2){scene[i].hitbox.width/2,scene[i].hitbox.height/2}, asin(b2Body_GetRotation(scene[i].bodyId).s)*(180.0f/M_PI), scene[i].color);
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