#include "raylib.h"
#include "math.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Shared Variables Definition (global)
Font font = { 0 };
Music music = { 0 };
Sound fxCoin = { 0 };


// Local Variables Definition (local to this module)
static const int screenWidth = 800;
static const int screenHeight = 1000;

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
    p.radius = 16.0f;
    return p;
}
Pacman pacman;
Vector2 desiredDirection = {0,0};

#define MAZE_ROWS 31
#define MAZE_COLS 28
#define TILE_SIZE 28
// 0 = empty, 1 = wall, 2 = pellet
int maze[MAZE_ROWS][MAZE_COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,0,0,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,0,0,0,0,0,0,1,0,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,0,2,0,0,0,1,0,0,0,0,0,0,1,0,0,0,2,0,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,0,0,0,0,0,0,1,0,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

// Local Functions Declaration
static void UpdateDrawFrame(void);          // Update and draw one frame

int main(void)
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Pacman");

    InitAudioDevice();      // Initialize audio device

    pacman = InitPacman(
        20 * TILE_SIZE + TILE_SIZE /2, 
        20 * TILE_SIZE + TILE_SIZE /2
    );

    // code 
    
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
    //UpdateMusicStream(music);       
    
    // Keyboard input for Pacman movement
    if (IsKeyDown(KEY_RIGHT)) desiredDirection = (Vector2){ 1, 0 };
    else if (IsKeyDown(KEY_LEFT)) desiredDirection = (Vector2){ -1, 0 };
    else if (IsKeyDown(KEY_UP)) desiredDirection = (Vector2) { 0, -1 };
    else if (IsKeyDown(KEY_DOWN)) desiredDirection = (Vector2) { 0, 1 };

    Vector2 pacmanGridPos = {
        (int)(pacman.position.x / TILE_SIZE),
        (int)(pacman.position.y / TILE_SIZE)
    };

    // Calculate center of current grid cell
    Vector2 cellCenter = {
        pacmanGridPos.x * TILE_SIZE + TILE_SIZE / 2.0f,
        pacmanGridPos.y * TILE_SIZE + TILE_SIZE / 2.0f
    };

    // Calculate distance from Pacman to center of grid cell
    float distToCenter = sqrtf(
        (pacman.position.x - cellCenter.x) * (pacman.position.x - cellCenter.x) +
        (pacman.position.y - cellCenter.y) * (pacman.position.y - cellCenter.y)
    );

    

    if(distToCenter <= 2.0)
    {
        // Check if the next cell in the desired direction is a wall
        int nextRow = (int)pacmanGridPos.y + (int)desiredDirection.y;
        int nextCol = (int)pacmanGridPos.x + (int)desiredDirection.x;
        if (nextRow >= 0 && nextRow < MAZE_ROWS && nextCol >= 0 && nextCol < MAZE_COLS)
        {
            if (maze[nextRow][nextCol] != 1)
            {
                pacman.direction = desiredDirection;
            }
            else
            {
                // Only stop if the wall is directly in front of Pacman's current direction
                if ((int)desiredDirection.x == (int)pacman.direction.x && (int)desiredDirection.y == (int)pacman.direction.y)
                {
                    pacman.direction = (Vector2){0, 0};
                }
                // Otherwise, keep moving in the current direction
            }
        }

        else
        {
            pacman.direction = (Vector2){0, 0}; // Stop if out of bounds
        }
    }

    // Update Pacman's position
    pacman.position.x += pacman.direction.x * pacman.speed;
    pacman.position.y += pacman.direction.y * pacman.speed;

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(BLACK);

    

    // Draw Maze
    for (int row = 0; row < MAZE_ROWS; row++)
    {
        for (int col = 0; col < MAZE_COLS; col++)
        {
            int x = col * TILE_SIZE;
            int y = row * TILE_SIZE;
            if (maze[row][col] == 1)
                DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, BLUE);
            else if (maze[row][col] == 2)
                DrawCircle(x + TILE_SIZE/2, y + TILE_SIZE/2, 4, WHITE);
        }
    }

    // Draw Pacman as a yellow circle
    DrawCircleV(pacman.position, pacman.radius, YELLOW);

    // For debugging Pacman's position in the grid
    if (distToCenter <= 5.0)
    {
        DrawRectangle((int)pacmanGridPos.x * TILE_SIZE, (int)pacmanGridPos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
    }
    else
    {
        DrawRectangle((int)pacmanGridPos.x * TILE_SIZE, (int)pacmanGridPos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, YELLOW);
    }
    
    

    // Optionally, for debugging:
     DrawText(TextFormat("Dist: %.2f", distToCenter), 10, 10, 20, WHITE);
     
    //DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}

