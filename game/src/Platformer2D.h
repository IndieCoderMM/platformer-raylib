#pragma once
#include "raylib.h"

#define ROW 5
#define COL 20
#define TILESIZE 128.0f
#define P_VEL 200.0f
#define JUMP_FORCE 350.0f
#define G 400

typedef struct Tile
{
    int id;
    bool isBlock;
    Rectangle rect;
} Tile;

typedef struct Animation
{
    float fps;
    int frames[2];
} Animation;

class AnimatedSprite
{
public:
    Animation currentAnimation = {0};
    float frameWidth = 0.0f;
    float frameHeight = 0.0f;
    int frameCounter = 0;
    float timer = 0.0f;
    int startFrame = 0;
    int endFrame = 0;
    int currentFrame = 0;
    Rectangle frameRect = {0};

    void loadSpriteSheet(Texture2D sheet, int row, int col, Animation animation);
    void updateFrame(float delta);
};

class Player : public AnimatedSprite
{
public:
    Vector2 position = {0.0f, 0.0f};
    Vector2 speed = {P_VEL, 0};
    bool jumping = false;
    bool moving = false;
    float size = 64;

    Player(float x, float y);
    void moveAndCollideY(Tile tileMap[ROW][COL], float dekta);
    void moveAndCollideX(int tileData[ROW][COL], float dx);
    void changeState();
    Rectangle getCollisionRect();
    int getCol();
    int getRow();
    bool isOnFloor(Tile tile, float dy);
    void movement(float delta);
    void draw(Texture2D sheet);
    void update(float delta);
};

void generateTileMap(Tile tileMap[ROW][COL]);
void drawTileMap(Tile tilemap[ROW][COL], Texture2D tileSet);
void updateCamera(Camera2D *camera, Player *player, int width, int height, float delta);
void updateCameraBoundsPush(Camera2D *camera, Player *player, int width, int height, float delta);
