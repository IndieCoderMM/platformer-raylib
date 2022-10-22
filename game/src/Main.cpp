#include "Platformer2D.h"

// Global variables
const float tileSize = 128.0f;
const float objSize = 64.0f;
const float playerSpeed = 150.0f;
const float jumpForce = 450.0f;
const float gravity = 600.0f;
const float playerSize = 64.0f;
const int playerMaxHp = 3;
Tile tileMap[totalRow][totalColumn] = { 0 };
Texture2D txTiles = { 0 };
Texture2D txObjs = { 0 };
Texture2D txGameBoy = { 0 };
Animation walkAnimation = { 5, {2, 3} };
Animation idleAnimation = { 5, {0, 0} };     // (fpa, [frame index,...])
Animation jumpAnimation = { 5, {1, 1} };

Sprite objects[totalObjects] = { 0 };
Sprite platforms[totalPlatforms] = { 0 };

// Local variables
static const int screenWidth = 800;
static const int screenHeight = 450;



int main(void)
{
	InitWindow(screenWidth, screenHeight, "Platformer 2D");
	SetTargetFPS(60);

	txTiles = LoadTexture("resources/img/tilesets/platformTilesheetx2.png");
	txObjs = LoadTexture("resources/img/tilesets/platformTilesheet.png");
	txGameBoy = LoadTexture("resources/img/characters/gameBoySheet.png");

	Player player(Vector2(10, -10));
	player.loadSpriteSheet(txGameBoy, 2, 4, idleAnimation);

	Camera2D camera = { 0 };
	camera.target = player.pos;
	camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 0.9f;

	Game game(screenWidth, screenHeight);
	game.newStage(&player);

	while (!WindowShouldClose())
	{
		// Debug updates
		if (IsKeyPressed(KEY_T)) game.setNewTheme();
		if (IsKeyPressed(KEY_R)) game.newStage(&player);
		
		float dt = GetFrameTime();

		game.update(&player, &camera, dt);

		BeginDrawing();
		ClearBackground(SKYBLUE);
		BeginMode2D(camera);

		game.drawLevel();
		player.draw();
		
		/* DrawRectangleLines(player.pos.x, player.pos.y, player.size, player.size, YELLOW);
		 for (int x = 0; x < totalColumn; x++)
			 for (int y = 0; y < totalRow; y++)
				 DrawRectangleLines(tileMap[y][x].x, tileMap[y][x].y, tileSize, tileSize, RAYWHITE);

		 DrawRectangleLines(tileMap[player.getRow()][player.getCol()].x, tileMap[player.getRow()][player.getCol()].y, tileSize, tileSize, RED);*/

		EndMode2D();
		// Draw hearts
		game.drawGUI(player);
		EndDrawing();
	}

	CloseWindow();
	UnloadTexture(txTiles);
	UnloadTexture(txGameBoy);
	UnloadTexture(txObjs);
	return 0;
}