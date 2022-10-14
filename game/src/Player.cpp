#include "Platformer2D.h"

Player::Player(float x, float y)
{
	pos.x = x;
	pos.y = y;
	speed.x = playerSpeed;
	speed.y = 0;
	jumping = false;
	moving = false;
	size = 64;
}

Rectangle Player::getHitbox()
{
	// Rectangle rect = { position.x, position.y, size, size };
	return Rectangle(pos.x, pos.y, size, size);
}

void Player::draw()
{
	frameRect.width = (direction != 0) ? frameWidth * direction : frameWidth;
	DrawTextureRec(txGameBoy, frameRect, Vector2(pos.x - size / 2, pos.y - size / 2), WHITE);
}

void Player::handleControl(float delta)
{
	moving = false;
	direction = 0;
	if (IsKeyDown(KEY_LEFT))
	{
		moving = true;
		direction = -1;
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		moving = true;
		direction = 1;
	}

	if (IsKeyDown(KEY_SPACE) && !jumping)
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
}

bool Player::isOnFloor(Tile tile)
{
	if (!tile.isBlock) return false;
	if (CheckCollisionRecs(Rectangle(tile.x, tile.y, tileSize, tileSize), getHitbox()))
		return (pos.x + size / 2 >= tile.x) && (pos.x + size / 2 <= tile.x + tileSize);
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

	if (isOnFloor(tile1) || isOnFloor(tile2)) {
		jumping = false;
		speed.y = 0.0f;
		pos.y = tile1.y - size;
	}
	else {
		speed.y += gravity * delta;
	}
}