#include "Platformer2D.h"

// Global variables
const float tileSize = 128.0f;
const float playerSpeed = 200.0f;
const float jumpForce = 450.0f;
const float gravity = 600.0f;
Tile tileMap[totalRow][totalColumn] = {0};
Texture2D txTiles = { 0 };
Texture2D txGameBoy = { 0 };

// Local variables
static const int screenWidth = 800;
static const int screenHeight = 450;
static Animation idleAnimation = { 5, {0, 0} };     // (fpa, [frame index,...])
static Animation walkAnimation = { 5, {2, 3} };
static Animation jumpAnimation = { 5, {1, 1} };

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Platformer 2D");

    txTiles = LoadTexture("resources/img/tilesets/platformTilesheet.png");
    txGameBoy = LoadTexture("resources/img/characters/gameBoySheet.png");

    Player player(10, -10);
    player.loadSpriteSheet(txGameBoy, 2, 4, idleAnimation);

    Camera2D camera = {0};
    camera.target = player.pos;
    camera.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    generateTileMap();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        player.update(dt);
 
        updateCameraSmooth(&camera, &player, screenWidth, screenHeight, dt);

        if (player.jumping)
            player.currentAnimation = jumpAnimation;
        else if (player.moving)
            player.currentAnimation = walkAnimation;
        else
            player.currentAnimation = idleAnimation;

        if (player.pos.y >= screenHeight)
        {
            player.pos.y = -100;
            generateTileMap();
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);
        BeginMode2D(camera);

        drawTileMap();
        DrawRectangleLines(player.pos.x, player.pos.y, player.size, player.size, YELLOW);
        for (int x = 0; x < totalColumn; x++)
            for (int y = 0; y < totalRow; y++)
                DrawRectangleLines(tileMap[y][x].x, tileMap[y][x].y, tileSize, tileSize, RAYWHITE);
        DrawRectangleLines(tileMap[player.getRow()][player.getCol()].x, tileMap[player.getRow()][player.getCol()].y, tileSize, tileSize, RED);
        player.draw();
        
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
