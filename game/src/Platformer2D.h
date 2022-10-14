#pragma once
#include "raylib.h"

typedef struct Tile
{
    int id;
    bool isBlock;
    float x;
    float y;
} Tile;

typedef struct Animation
{
    float fps;
    int frames[2];
} Animation;

const int totalRow = 5;
const int totalColumn = 20;

extern const float tileSize;
extern const float playerSpeed;
extern const float jumpForce;
extern const float gravity;
extern Tile tileMap[totalRow][totalColumn];
extern Texture2D txTiles;
extern Texture2D txGameBoy;

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
    Player(float x, float y);

    Vector2 pos = {0};
    Vector2 speed = {0};
    bool jumping = false;
    bool moving = false;
    float size = 0.0f;
    int direction = 1;  //R(1):L(-1)

    void moveAndCollideY(float delta);
    void moveAndCollideX(float delta);
    Rectangle getHitbox();
    int getCol();
    int getRow();
    bool isOnFloor(Tile tile);
    bool isWallCollision(Tile tile);
    void handleControl(float delta);
    void draw();
    void update(float delta);
};

void generateTileMap();
void drawTileMap();
void updateCameraSmooth(Camera2D *camera, Player *player, int width, int height, float delta);
