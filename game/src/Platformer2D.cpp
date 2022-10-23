#include "raylib.h"
#include "raymath.h"
#include "Platformer2D.h"

Game::Game(int width, int height) {
	this->width = width;
	this->height = height;
	door.width = objSize;
	door.height = objSize * 2;
	door.frame = Rectangle(objSize * 8, objSize * 5, door.width, door.height);
	door.visible = true;
	key.frame = Rectangle(objSize * 9, objSize * 4, objSize, objSize);
	key.visible = true;
}

void Game::drawGUI(Player player) {
	Rectangle heartFrame = { 0 };
	Rectangle keyFrame = { tileSize * 6, tileSize * 4, tileSize, tileSize };
	keyFrame.x = (player.hasKey) ? tileSize * 9 : keyFrame.x;
	int heartIndex = 0;
	for (int i = 0; i < 3; i++) {
		heartIndex = (player.hp > i) ? 4 : 2;
		heartFrame = Rectangle(tileSize * 11, tileSize * heartIndex, tileSize, tileSize);
		DrawTextureRec(txTiles, heartFrame, Vector2(-10 + (tileSize / 2 * i), -20), WHITE);
	}
	DrawTextureRec(txTiles, keyFrame, Vector2(-10, height - tileSize), WHITE);
	DrawTextureRec(txTiles, Rectangle(tileSize * 6, tileSize * 3, tileSize, tileSize), Vector2(width - tileSize, -20), WHITE);
	DrawText(TextFormat("%3d", player.collected), width - tileSize - MeasureText("00", 60), 20, 60, WHITE);
	DrawText(TextFormat("Stage %2d", level), width / 2 - MeasureText("Stage 00", 40) / 2, 10, 40, WHITE);
}

void Game::newStage(Player* player) {
	//player->pos = Vector2(0, 0);
	//player->checkPoint = Vector2(0, 0);
	setNewTheme();
	makeLevel();
}

void Game::update(Player* player, Camera2D* camera, float delta) {
	updateCameraSmooth(camera, player, delta);
	player->update(delta);
	if (CheckCollisionRecs(player->getHitbox(), Rectangle(key.x, key.y, objSize, objSize))) {
		player->hasKey = true;
		key.visible = false;
	}
	if (CheckCollisionRecs(player->getHitbox(), Rectangle(door.x, door.y, door.width, door.height))) {
		if (IsKeyPressed(KEY_ENTER)) {

			if (player->hasKey) {
				player->hasKey = false;
				complete = true;
			}
			else if (complete) {
				nextLevel(player);
			}
		}
	}
	if (player->pos.y >= height) {
		player->hp--;
		player->reset();
	}

	if (player->hp <= 0)  restart(player);
}

void Game::nextLevel(Player* player) {
	complete = false;
	level++;
	newStage(player);
}

void Game::restart(Player* player) {
	level = 1;
	player->hp = playerMaxHp;
	player->collected = 0;
	player->hasKey = false;
	player->checkPoint = Vector2(0, 0);
	player->pos = Vector2(0, 0);
}

void Game::setNewTheme(void) {
	theme.material = GetRandomValue(0, 1);
	theme.layer = GetRandomValue(0, 2);
	theme.base = 3;
}

void Game::makeLevel(void) {
	generateTileMap();
	generateObjects();
	generatePlatforms();
	placeObjects();
	placePlatforms();
}

void Game::drawLevel(void) {
	drawTileMap();
	drawObjects();
	drawPlatforms();
}


void Game::generateTileMap(void)
{
	int pGround = 0;
	int pHill = 0;
	int groundLevel = 3;

	for (int x = 0; x < totalColumn; x++) {
		pGround = (x > 3 && x != totalColumn - 1) ? GetRandomValue(0, 3) : 1;
		pHill = GetRandomValue(0, 2);
		for (int y = 0; y < totalRow; y++) {
			tileMap[y][x].x = tileSize * x;
			tileMap[y][x].y = tileSize * y;
			if ((y < groundLevel) || (pGround == 0 && tileMap[y][x - 1].isBlock)) {
				tileMap[y][x].isBlock = false;
			}
			else if ((y == groundLevel) && (pHill == 0)) {
				tileMap[y][x].isBlock = false;
			}
			else {
				tileMap[y][x].isBlock = true;
				pGround = 1;
			}
		}
	}
}

void Game::placeObjects(void) {
	int objCount = 0;
	bool placeCheckPoint = false;
	bool placeKey = false;
	bool placeDoor = false;
	for (int x = 0; x < totalColumn; x++)
		for (int y = 0; y < totalRow; y++) {
			if ((y < 1) || !tileMap[y][x].isBlock || tileMap[y - 1][x].isBlock) continue;
			// Place objects
			objects[objCount].x = tileMap[y][x].x + objSize * GetRandomValue(0, 1);
			objects[objCount].y = tileMap[y][x].y - objSize;
			if (x == totalColumn - 1 && !placeDoor) {
				door.x = objects[objCount].x;
				door.y = objects[objCount].y - objSize;
				placeDoor = true;
				return;
			}
			objects[objCount].visible = true;
			if ((x >= totalColumn / 3) && !placeCheckPoint) {
				objects[objCount].frame = Rectangle(objSize * 12, objSize * 5, objSize, objSize);
				objects[objCount].collectable = false;
				objects[objCount].collidable = false;
				objects[objCount].checkPoint = true;
				placeCheckPoint = true;
			}
			else if (!placeKey && placeCheckPoint && (x >= totalColumn / 2)) {
				key.x = objects[objCount].x;
				key.y = objects[objCount].y;
				objects[objCount].visible = false;
				placeKey = true;
			}
			objCount++;
		}
}

void Game::placePlatforms(void) {
	int pCount = 0;
	for (int x = 0; x < totalColumn; x++)
		for (int y = 0; y < totalRow; y++) {
			if ((y < 1) || tileMap[y - 1][x].isBlock || !tileMap[y][x].isBlock) continue;
			if ((x < 1) || (x > totalColumn - 3) || tileMap[y - 1][x - 1].isBlock || tileMap[y - 1][x + 1].isBlock) continue;
			if (GetRandomValue(0, 1) == 1) {
				platforms[pCount].visible = true;
				platforms[pCount].x = tileMap[y][x].x;
				platforms[pCount].y = tileMap[y][x].y - tileSize - platforms[pCount].height;
				pCount++;
			}
		}
}

void Game::generatePlatforms(void) {
	for (int i = 0; i < totalPlatforms; i++) {
		platforms[i].frame = Rectangle(objSize * 3, objSize * (3 + GetRandomValue(0, 1)), objSize, objSize);
		platforms[i].width = objSize;
		platforms[i].height = objSize;
		platforms[i].collidable = true;
		platforms[i].visible = false;
	}
}

void Game::generateObjects(void) {
	// Spikes & grass
	int objType = 0;
	for (int i = 0; i < totalObjects; i++) {
		objType = GetRandomValue(0, 10);
		objects[i].collidable = false;
		objects[i].collectable = false;
		objects[i].checkPoint = false;
		if (objType == 0) {
			// Spike
			objects[i].frame = Rectangle(objSize * 0, objSize * 5, objSize, objSize);
			objects[i].collidable = true;
		}
		else if (objType < 3) {
			// Grass
			objects[i].frame = Rectangle(objSize * 2, objSize * 5, objSize, objSize);
		}
		else {
			// Gems
			objects[i].frame = Rectangle(objSize * (7 + GetRandomValue(0, 3)), objSize * 3, objSize, objSize);
			objects[i].collectable = true;
		}
		objects[i].width = objSize;
		objects[i].height = objSize;
		objects[i].visible = false;
	}
}

void Game::drawPlatforms(void) {
	for (int i = 0; i < totalPlatforms; i++)
	{
		if (!platforms[i].visible) continue;
		DrawTextureRec(txObjs, platforms[i].frame, Vector2(platforms[i].x + (platforms[i].width), platforms[i].y), WHITE);
		/*for (int j = 0; j < 3; j++)
			DrawTextureRec(txObjs, platforms[i].frame, Vector2(platforms[i].x + (platforms[i].width * j), platforms[i].y), WHITE);
		platforms[i + 1].visible = false;
		platforms[i + 2].visible = false;*/
		//i += 2;
	}
}

void Game::drawObjects(void) {
	for (int i = 0; i < totalObjects; i++) {
		if (!objects[i].visible) continue;
		DrawTextureRec(txObjs, objects[i].frame, Vector2(objects[i].x, objects[i].y), WHITE);
	}
	if (key.visible)
		DrawTextureRec(txObjs, key.frame, Vector2(key.x, key.y), WHITE);
	door.frame.x = (complete) ? objSize * 5 : objSize * 8;
	DrawTextureRec(txObjs, door.frame, Vector2(door.x, door.y), WHITE);
}

void Game::drawTileMap(void)
{
	Rectangle srcRect = { 0, theme.material * tileSize, tileSize, tileSize };

	for (int y = 0; y < totalRow; y++) {
		for (int x = 0; x < totalColumn; x++) {
			if (!tileMap[y][x].isBlock)
				continue;
			tileMap[y][x].id = (y > 0 && tileMap[y - 1][x].isBlock) ? theme.base : theme.layer;
			srcRect.x = tileSize * tileMap[y][x].id;
			DrawTextureRec(txTiles, srcRect, Vector2(tileMap[y][x].x, tileMap[y][x].y), WHITE);
		}
	}
}

void AnimatedSprite::loadSpriteSheet(Texture2D sheet, int row, int col, Animation animation)
{
	frameWidth = sheet.width / (float)col;
	frameHeight = sheet.height / (float)row;
	Rectangle rect = { 0, 0, frameWidth, frameHeight };
	frameRect = rect;
	currentAnimation = animation;
	currentFrame = 0;
}

void AnimatedSprite::updateFrame(float delta)
{
	timer += delta;

	if (timer >= 1 / currentAnimation.fps)
	{
		timer = 0;
		currentFrame++;
		if (currentFrame >= 2)
			currentFrame = 0;
		frameRect.x = (float)(currentAnimation.frames[currentFrame] * frameWidth);
	}
}

void Game::updateCameraSmooth(Camera2D* camera, Player* player, float delta)
{
	static float minSpeed = 50;
	static float minEffectLength = 20;
	static float fractionSpeed = 0.8f;
	Vector2 vOffset = { width / 2.0f, height / 2.0f };
	camera->offset = vOffset;
	Vector2 diff = Vector2Subtract(player->pos, camera->target);
	float length = Vector2Length(diff);

	if (length > minEffectLength)
	{
		float speed = fmaxf(fractionSpeed * length, minSpeed);
		camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed * delta / length));
	}
}

