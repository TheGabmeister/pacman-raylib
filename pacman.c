#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Shared Variables Definition (global)
Font font = { 0 };
Music music = { 0 };
Sound fxCoin = { 0 };


// Local Variables Definition (local to this module)
static const int screenWidth = 600;
static const int screenHeight = 800;

typedef struct {
    Vector2 position;   // Pacman's position in pixels
    Vector2 direction;  // Current movement direction
    float speed;        // Movement speed (pixels per frame)
    float radius;       // For drawing Pacman
} Pacman;

// Initialize Pacman at a given position
Pacman InitPacman(float x, float y)
{
    Pacman p;
    p.position = (Vector2){ x, y };
    p.direction = (Vector2){ 0, 0 };
    p.speed = 2.0f;
    p.radius = 20.0f;
    return p;
}

Pacman pacman;

// Local Functions Declaration
static void UpdateDrawFrame(void);          // Update and draw one frame

int main(void)
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Pacman");

    InitAudioDevice();      // Initialize audio device

    pacman = InitPacman(screenWidth / 2.0f, screenHeight / 2.0f);
    
    // Load global data (assets that must be available in all screens, i.e. font)
    font = LoadFont("resources/mecha.png");
    //music = LoadMusicStream("resources/ambient.ogg"); // TODO: Load music
    fxCoin = LoadSound("resources/coin.wav");

    SetMusicVolume(music, 1.0f);
    PlayMusicStream(music);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // Unload global data loaded
    UnloadFont(font);
    UnloadMusicStream(music);
    UnloadSound(fxCoin);

    CloseAudioDevice();     // Close audio context

    CloseWindow();          // Close window and OpenGL context

    return 0;
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    //UpdateMusicStream(music);       // NOTE: Music keeps playing between screens
    
    // Keyboard input for Pacman movement
    if (IsKeyDown(KEY_RIGHT)) pacman.direction = (Vector2){ 1, 0 };
    else if (IsKeyDown(KEY_LEFT)) pacman.direction = (Vector2){ -1, 0 };
    else if (IsKeyDown(KEY_UP)) pacman.direction = (Vector2) { 0, -1 };
    else if (IsKeyDown(KEY_DOWN)) pacman.direction = (Vector2) { 0, 1 };

    // Update Pacman's position
    pacman.position.x += pacman.direction.x * pacman.speed;
    pacman.position.y += pacman.direction.y * pacman.speed;


    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw Pacman as a yellow circle
    DrawCircleV(pacman.position, pacman.radius, YELLOW);


    //DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}

