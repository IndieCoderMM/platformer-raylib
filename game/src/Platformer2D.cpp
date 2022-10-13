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

Player::Player(float x, float y)
{
    position.x = x;
    position.y = y;
}

Rectangle Player::getCollisionRect()
{
    Rectangle rect = { position.x, position.y, size, size };
        return rect;
}

void Player::draw(Texture2D sheet)
{
    Vector2 pos = { position.x - size / 2, position.y - size / 2 };
    DrawTextureRec(sheet, frameRect, pos, WHITE);
}

void Player::movement(float delta)
{
    moving = false;
    if (IsKeyDown(KEY_LEFT))
    {
        moving = true;
        frameRect.width = -frameWidth;
        position.x -= speed.x * delta;
    }

    else if (IsKeyDown(KEY_RIGHT))
    {
        moving = true;
        frameRect.width = frameWidth;
        position.x += speed.x * delta;
    }

    if (IsKeyDown(KEY_SPACE) && !jumping)
    {
        speed.y = -JUMP_FORCE;
        jumping = true;
    }
}

int Player::getRow()
{
    return (int)position.y / TILESIZE;
}

int Player::getCol()
{
    return (int)position.x / TILESIZE;
}

void Player::update(float delta)
{
    movement(delta);
    updateFrame(delta);
}

bool Player::isOnFloor(Tile tile, float dy)
{
    return (tile.rect.x <= position.x && tile.rect.x + tile.rect.width >= position.x && tile.rect.y >= position.y + size && tile.rect.y < position.y + size + dy);
}

void Player::moveAndCollideY(Tile tileMap[ROW][COL], float delta)
{
    float dy = speed.y * delta;
    Tile tile = tileMap[getRow() + 1][getCol()];
    Tile tile2 = tileMap[getRow() + 1][getCol() + 1];
    bool onFloor = false;
    if (isOnFloor(tile, dy) || isOnFloor(tile2, dy))
    {
        onFloor = true;
        speed.y = 0.0f;
        position.y = tile.rect.y - size;
    }

    if (!onFloor)
    {
        position.y += dy;
        speed.y += G * delta;
    }
    else
        jumping = false;
}

void updateCamera(Camera2D *camera, Player *player, int width, int height, float delta)
{
    static float minSpeed = 30;
    static float minEffectLength = 20;
    static float fractionSpeed = 0.8f;
    Vector2 vOffset = { width / 2.0f, height / 2.0f };
    camera->offset = vOffset;
    Vector2 diff = Vector2Subtract(player->position, camera->target);
    float length = Vector2Length(diff);

    if (length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed * length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed * delta / length));
    }
}

void updateCameraBoundsPush(Camera2D *camera, Player *player, int width, int height, float delta)
{
    static Vector2 bbox = {0.2f, 0.2f};
    Vector2 minPos = { (1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height };
    Vector2 bboxWorldMin = GetScreenToWorld2D(minPos, *camera);
    Vector2 maxPos = { (1 + bbox.x) * 0.5f * width, (1 + bbox.y) * 0.5f * height };
    Vector2 bboxWorldMax = GetScreenToWorld2D(maxPos, *camera);
    Vector2 vOffset = { (1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height };
    camera->offset = vOffset;

    if (player->position.x < bboxWorldMin.x)
        camera->target.x = player->position.x;
    else if (player->position.y < bboxWorldMin.y)
        camera->target.y = player->position.y;
    else if (player->position.x > bboxWorldMax.x)
        camera->target.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
    else if (player->position.y > bboxWorldMax.y)
        camera->target.y = bboxWorldMin.y + (player->position.y - bboxWorldMin.y);
}

void generateTileMap(Tile tilemap[ROW][COL])
{

    for (int c = 0; c < COL; c++)
    {
        int rand = GetRandomValue(0, 4);

        for (int r = 0; r < ROW; r++)
        {
            if (rand == 0 || r < 1)
            {
                tilemap[r][c].isBlock = false;
                tilemap[r][c].id = -1;
                continue;
            }
            tilemap[r][c].isBlock = true;
            if (tilemap[r - 1][c].id == -1)
                tilemap[r][c].id = 0;
            else
                tilemap[r][c].id = 3;
            Rectangle rect = { c * TILESIZE, r * TILESIZE, TILESIZE, TILESIZE };
            tilemap[r][c].rect = rect;
        }
    }
}

void drawTileMap(Tile tilemap[ROW][COL], Texture2D tileSet)
{
    Rectangle srcRect = {0, 0, TILESIZE, TILESIZE};
    for (int r = 0; r < ROW; r++)
    {
        for (int c = 0; c < COL; c++)
        {
            if (tilemap[r][c].id == -1)
                continue;
            srcRect.x = TILESIZE * tilemap[r][c].id;
            Vector2 pos = { TILESIZE * c, TILESIZE * r };
            DrawTextureRec(tileSet, srcRect, pos, WHITE);
        }
    }
}