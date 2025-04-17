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
    Vector2 position;   
    Vector2 direction;  // Current movement direction
    float speed;        // Movement speed (pixels per frame)
    float radius;       // For drawing Pacman
} Pacman;

typedef struct {
    Vector2 position;
    Vector2 direction;
    float speed;
    float radius;
    Color color;
} Ghost;

// Initialize Pacman at a given position
Pacman InitPacman(float x, float y)
{
    Pacman p;
    p.position = (Vector2){ x, y };
    p.direction = (Vector2){ 0, 0 };
    p.speed = 4.0f;
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

#define GHOST_COUNT 4
Ghost ghosts[GHOST_COUNT];

void InitGhosts() {
    Color ghostColors[GHOST_COUNT] = { RED, GREEN, BLUE, MAGENTA };
    for (int i = 0; i < GHOST_COUNT; i++) {
        ghosts[i].position = (Vector2){ 14 * TILE_SIZE + TILE_SIZE/2, 14 * TILE_SIZE + TILE_SIZE/2 + i*10 };
        ghosts[i].direction = (Vector2){ 0, -1 };
        ghosts[i].speed = 3.0f;
        ghosts[i].radius = 16.0f;
        ghosts[i].color = ghostColors[i];
    }
}

// Local Functions Declaration
static void UpdateDrawFrame(void);          // Update and draw one frame
void UpdateGhosts();

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
    
    InitGhosts();

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
                pacman.position = cellCenter;
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

    UpdateGhosts();


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
                DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, DARKGRAY);
            else if (maze[row][col] == 2)
                DrawCircle(x + TILE_SIZE/2, y + TILE_SIZE/2, 4, WHITE);
        }
    }

    // Draw Pacman as a yellow circle
    DrawCircleV(pacman.position, pacman.radius, YELLOW);

    // For debugging Pacman's position in the grid
    if (distToCenter <= 2.0)
    {
        DrawRectangle((int)pacmanGridPos.x * TILE_SIZE, (int)pacmanGridPos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
    }
    else
    {
        DrawRectangle((int)pacmanGridPos.x * TILE_SIZE, (int)pacmanGridPos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, YELLOW);
    }
    
    // For debugging:
    // DrawText(TextFormat("Dist: %.2f", distToCenter), 10, 10, 20, WHITE);
     
    // Draw Ghosts
    for (int i = 0; i < GHOST_COUNT; i++) {
        DrawCircleV(ghosts[i].position, ghosts[i].radius, ghosts[i].color);
    }

    //DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}

void UpdateGhosts()
{
    const float CHASE_DISTANCE = 200.0f; // Only chase if Pacman is within 200 pixels

    // Update Ghosts
    for (int i = 0; i < GHOST_COUNT; i++) {
        Vector2 diff = {
            pacman.position.x - ghosts[i].position.x,
            pacman.position.y - ghosts[i].position.y
        };
        float distance = sqrtf(diff.x * diff.x + diff.y * diff.y);

        if (distance <= CHASE_DISTANCE) {
            // Choose direction with greatest distance (simple AI)
            if (fabs(diff.x) > fabs(diff.y)) {
                ghosts[i].direction = (Vector2){ (diff.x > 0) ? 1 : -1, 0 };
            }
            else {
                ghosts[i].direction = (Vector2){ 0, (diff.y > 0) ? 1 : -1 };
            }
        } else {
            ghosts[i].direction = (Vector2){ 0, 0 }; // Stop if Pacman is too far
        }

        // Check for walls
        int nextRow = (int)((ghosts[i].position.y + ghosts[i].direction.y * ghosts[i].speed) / TILE_SIZE);
        int nextCol = (int)((ghosts[i].position.x + ghosts[i].direction.x * ghosts[i].speed) / TILE_SIZE);
        if (maze[nextRow][nextCol] == 1) {
            ghosts[i].direction = (Vector2){ 0, 0 }; // Stop if wall
        }
        // Move ghost
        ghosts[i].position.x += ghosts[i].direction.x * ghosts[i].speed;
        ghosts[i].position.y += ghosts[i].direction.y * ghosts[i].speed;
    }
}