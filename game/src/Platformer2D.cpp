#include "raylib.h"
#include "raymath.h"
#include "Platformer2D.h"

void AnimatedSprite::loadSpriteSheet(Texture2D sheet, int row, int col, Animation animation)
{
	frameWidth = sheet.width / col;
	frameHeight = sheet.height / row;
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

void updateCameraSmooth(Camera2D* camera, Player* player, int width, int height, float delta)
{
	static float minSpeed = 30;
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

void generateTileMap(void)
{
	int pGround = 0;
	int pHill = 0;
	for (int x = 0; x < totalColumn; x++) {
		pGround = GetRandomValue(0, 4);
		pHill = GetRandomValue(0, 2);
		for (int y = 0; y < totalRow; y++) {
			tileMap[y][x].x = tileSize * x;
			tileMap[y][x].y = tileSize * y;
			if ((pGround == 0) || (y < 2) || (y == 2 && pHill != 1)) {
				//No Ground
				tileMap[y][x].isBlock = false;
				continue;
			}

			tileMap[y][x].isBlock = true;
			tileMap[y][x].id = (y > 0 && tileMap[y - 1][x].isBlock) ? 3 : 0;
		}
	}
}

void drawTileMap(void)
{
	Rectangle srcRect = { 0, 0, tileSize, tileSize };
	for (int y = 0; y < totalRow; y++) {
		for (int x = 0; x < totalColumn; x++) {
			if (!tileMap[y][x].isBlock)
				continue;
			srcRect.x = tileSize * tileMap[y][x].id;
			DrawTextureRec(txTiles, srcRect, Vector2(tileMap[y][x].x, tileMap[y][x].y), WHITE);
		}
	}
}