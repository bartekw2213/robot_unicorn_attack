#include "Obstacle.h"


Obstacle::Obstacle() {
	srand(time(0));									//	przy pomocy rand() bede generowac losowo gdzie ma pojawic sie przeszkoda
	mPosX = SCREEN_WIDTH + (rand() % SCREEN_WIDTH);
	mPosY = PLATFORM_Y - obstacleTexture.getHeight();
};

Obstacle::~Obstacle() {
	obstacleTexture.free();
};

bool Obstacle::loadTexture(SDL_Renderer* renderer) {
	char obstacleTexturePath[MAX_PATH_LENGTH] = "../images/obstacle2.png";
	if (!obstacleTexture.loadFromFile(obstacleTexturePath, renderer)) {
		printf("Failed to load obstacle texture image!\n");
		return false;
	}
	else
		mPosY = PLATFORM_Y - obstacleTexture.getHeight();

	return true;
};

void Obstacle::render(SDL_Renderer* renderer, int scrollingOffsetVel) {
	mPosX += scrollingOffsetVel;
	obstacleTexture.render(mPosX, mPosY, renderer);

	if (mPosX < -obstacleTexture.getWidth())
		mPosX = (SCREEN_WIDTH * (rand() % 2 + 1)) + (rand() % SCREEN_WIDTH);
};

bool Obstacle::checkIfHorseRanIntoObstacle(SDL_Rect* horseCollider) {
	if ((horseCollider->x + horseCollider->w) >= mPosX &&
		mPosX >= horseCollider->x &&
		(horseCollider->y + horseCollider->h) > mPosY + OBSTACLE_COLLISION_Y_OFFSET) // zalicza granice bledu tego ze kon moze byc lekko nizej
		return true;
	return false;
};

bool Obstacle::checkIfHorseLandedOnObstacle(SDL_Rect* horseCollider) {
	if ((horseCollider->x + horseCollider->w) >= mPosX &&
		mPosX + obstacleTexture.getWidth() >= horseCollider->x &&
		(horseCollider->y + horseCollider->h) <= mPosY + OBSTACLE_COLLISION_Y_OFFSET && (horseCollider->y + horseCollider->h) >= mPosY - OBSTACLE_COLLISION_Y_OFFSET) {
		return true;
	}

	return false;
};