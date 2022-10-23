#include "Platformer2D.h"

Player::Player(Vector2 startPos)
{
	pos = startPos;
	checkPoint.x = pos.x;
	checkPoint.y = pos.y;
	speed.x = playerSpeed;
	speed.y = 0;
	jumping = false;
	moving = false;
	size = playerSize;
	hp = playerMaxHp;
}

Rectangle Player::getHitbox()
{
	// Rectangle rect = { position.x, position.y, size, size };
	return Rectangle(pos.x, pos.y, size, size);
}

void Player::reset(void) {
	pos = checkPoint;
}

void Player::draw(void)
{
	Color color = (invincible) ? RED : WHITE;
	DrawTextureRec(txGameBoy, frameRect, Vector2(pos.x - size / 2, pos.y - size / 2), color);
}

void Player::handleControl(float delta)
{
	moving = false;
	direction = 0;
	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
	{
		moving = true;
		direction = -1;
		frameRect.width = frameWidth * direction;
	}
	else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
	{
		moving = true;
		direction = 1;
		frameRect.width = frameWidth * direction;
	}

	if ((IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_UP)) && !jumping)
	{
		speed.y = -jumpForce;
		jumping = true;
	}
}

int Player::getRow()
{
	return (int)pos.y / tileSize;
}

int Player::getCol()
{
	return (int)pos.x / tileSize;
}

void Player::update(float delta)
{
	handleControl(delta);
	moveAndCollideY(delta);
	moveAndCollideX(delta);
	updateFrame(delta);
	handleCollision(delta);

	if (jumping)
		currentAnimation = jumpAnimation;
	else if (moving)
		currentAnimation = walkAnimation;
	else
		currentAnimation = idleAnimation;
}

bool Player::isOnFloor(Tile tile)
{
	if (!tile.isBlock) return false;
	if (CheckCollisionRecs(Rectangle(tile.x, tile.y, tileSize, tileSize), getHitbox()))
		return (pos.x + size / 2 >= tile.x) && (pos.x + size / 2 <= tile.x + tileSize);
	return false;
}

bool Player::handleHeadBump() {
	for (int i = 0; i < totalPlatforms; i++)
	{
		if (!platforms[i].visible) continue;
		Rectangle pRect = { platforms[i].x, platforms[i].y, platforms[i].width * 3, platforms[i].height };
		if (CheckCollisionRecs(pRect, getHitbox())) {
			if ((pos.x > platforms[i].x) && (pos.x < platforms[i].x + pRect.width) && (pos.y >= platforms[i].y))
			{
				platforms[i].visible = false;
				pos.y = platforms[i].y + platforms[i].height;
				return true;
			}
		}
	}
	return false;
}

bool Player::isWallCollision(Tile tile) {
	if (!tile.isBlock) return false;
	if (CheckCollisionRecs(Rectangle(tile.x, tile.y, tileSize, tileSize), getHitbox())) {
		return true;
	}
	return false;
}

void Player::moveAndCollideX(float delta) {
	pos.x += speed.x * direction * delta;
	if (pos.x <= 0) pos.x = 0;
	if (getCol() < 0 || getCol() >= totalColumn || getRow() < 0) return;
	Tile tile = tileMap[getRow()][getCol() + direction];
	if (isWallCollision(tile)) {
		if (direction > 0) pos.x = tile.x - size;
		//TODO: Fix left wall collision
		else  pos.x = tile.x + tileSize + 10;
	}
}

void Player::moveAndCollideY(float delta)
{
	pos.y += speed.y * delta;

	Tile tile1 = tileMap[getRow() + 1][getCol()];
	Tile tile2 = tileMap[getRow() + 1][getCol() + 1];

	if (jumping && handleHeadBump()) {
		speed.y = 20.0f;
	}

	if (isOnFloor(tile1) || isOnFloor(tile2)) {
		jumping = false;
		speed.y = 0.0f;
		pos.y = tile1.y - size;
	}
	else {
		speed.y += gravity * delta;
	}
}

void Player::handleCollision(float delta) {
	if (invincible) {
		invincibleTimer += delta;
		if (invincibleTimer >= 3) {
			invincibleTimer = 0;
			invincible = false;
		}
		return;
	}
	for (int i = 0; i < totalObjects; i++) {
		if (!objects[i].visible) continue;

		if (CheckCollisionRecs(getHitbox(), Rectangle(objects[i].x, objects[i].y, objSize, objSize))) {
			if (objects[i].checkPoint) {
				checkPoint.x = objects[i].x;
				checkPoint.y = objects[i].y;
				objects[i].frame.x = objSize * 13;
				return;
			}
			if (objects[i].collidable) {
				invincible = true;
				hp--;
				return;
			}
			if (objects[i].collectable) {
				collected++;
				objects[i].visible = false;
				return;
			}
		}
	}
}