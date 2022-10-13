#include <raylib.h>
#include <raymath.h>
#include "Platformer2D.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    Tile tileMap[ROW][COL];

    InitWindow(screenWidth, screenHeight, "Platformer 2D");

    Texture2D txTiles = LoadTexture("resources/img/tilesets/platformTilesheet.png");
    Texture2D txGameBoy = LoadTexture("resources/img/characters/gameBoySheet.png");

    Player player(screenWidth / 2, 10);
    Animation idleAnimation = {5, {0, 0}};
    Animation walkAnimation = {3, {2, 3}};
    Animation jumpAnimation = {5, {1, 1}};
    player.loadSpriteSheet(txGameBoy, 2, 4, idleAnimation);

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    generateTileMap(tileMap);
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        player.update(dt);
        player.moveAndCollideY(tileMap, dt);
        updateCameraBoundsPush(&camera, &player, screenWidth, screenHeight, dt);

        if (player.jumping)
            player.currentAnimation = jumpAnimation;
        else if (player.moving)
            player.currentAnimation = walkAnimation;
        else
            player.currentAnimation = idleAnimation;

        BeginDrawing();
        ClearBackground(SKYBLUE);
        BeginMode2D(camera);

        drawTileMap(tileMap, txTiles);
        DrawRectangleRec(player.getCollisionRect(), GREEN);
        player.draw(txGameBoy);
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
