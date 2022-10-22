#pragma once
#include "raylib.h"

typedef struct Tile
{
	int id;
	bool isBlock;
	float x;
	float y;
} Tile;

typedef struct Object {
	Rectangle frame;

	bool collidable;
} Object;

typedef struct Sprite {
	Rectangle frame;
	bool checkPoint;
	bool collidable;
	bool collectable;
	bool visible;
	float width;
	float height;
	float x;
	float y;
} Sprite;

typedef struct Animation
{
	float fps;
	int frames[2];
} Animation;

typedef struct Theme {
	int material;
	int layer;
	int base;
} Theme;

const int totalRow = 7;
const int totalColumn = 20;
const int totalObjects = 50;
const int totalPlatforms = 30;

extern const float tileSize;
extern const float objSize;
extern const float playerSpeed;
extern const float playerSize;
extern const float jumpForce;
extern const float gravity;
extern const int playerMaxHp;
extern Tile tileMap[totalRow][totalColumn];
extern Sprite objects[totalObjects];
extern Sprite platforms[totalPlatforms];
extern Texture2D txTiles;
extern Texture2D txObjs;
extern Texture2D txGameBoy;
extern Animation idleAnimation;
extern Animation jumpAnimation;
extern Animation walkAnimation;

class AnimatedSprite
{
public:
	Animation currentAnimation = { 0 };
	float frameWidth = 0.0f;
	float frameHeight = 0.0f;
	int frameCounter = 0;
	float timer = 0.0f;
	int startFrame = 0;
	int endFrame = 0;
	int currentFrame = 0;
	Rectangle frameRect = { 0 };

	void loadSpriteSheet(Texture2D sheet, int row, int col, Animation animation);
	void updateFrame(float delta);
};

class Player : public AnimatedSprite
{
	Vector2 speed = { 0 };
	float size = 0.0f;
	int direction = 1;  //R(1):L(-1)
	bool moving = false;
	bool jumping = false;
	bool invincible = false;
	float invincibleTimer = 0.0f;

public:
	int hp = 0;
	int collected = 0;
	bool hasKey = false;
	Vector2 checkPoint = { 0 };
	Vector2 pos = { 0 };

	Player(Vector2 startPos);
	void moveAndCollideY(float delta);
	void moveAndCollideX(float delta);
	int getCol(void);
	int getRow(void);
	bool isOnFloor(Tile tile);
	bool handleHeadBump(void);
	bool isWallCollision(Tile tile);
	void handleControl(float delta);
	void handleCollision(float delta);
	void draw(void);
	void update(float delta);
	void reset(void);
	Rectangle getHitbox(void);
};

class Game {
	Sprite key = { 0 };
	Sprite door = { 0 };
	Theme theme = { 0 };
	int width = 0;
	int height = 0;
	int level = 1;

public:
	bool complete = false;

	Game(int width, int height);
	void nextLevel(Player* player);
	void newStage(Player* player);
	void setNewTheme(void);
	void makeLevel(void);
	void generateTileMap(void);
	void generateObjects(void);
	void placeObjects(void);
	void generatePlatforms(void);
	void drawGUI(Player player);
	void placePlatforms(void);
	void drawLevel(void);
	void drawPlatforms(void);
	void drawObjects(void);
	void drawTileMap(void);
	void restart(Player* player);
	void update(Player* player, Camera2D* camera, float delta);
	void updateCameraSmooth(Camera2D* camera, Player* player, float delta);
};

