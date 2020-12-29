#include <stdio.h>
#include "Platform.h"

Platform::Platform() {
	mPosX = 0;
	mPosY = PLATFORM_Y;
};

Platform::~Platform() {
	mPlatformTexture.free();
};

bool Platform::loadTexture(SDL_Renderer* renderer) {
	char platformTexturePath[MAX_PATH_LENGTH] = "../images/platform.png";
	if (!(mPlatformTexture).loadFromFile(platformTexturePath, renderer)) {
		printf("Failed to load platform texture image!\n");
		return false;
	};

	return true;
};

void Platform::render(SDL_Renderer* renderer, int scrollingOffsetVel) {
	mPosX += scrollingOffsetVel;
	mPlatformTexture.render(mPosX, mPosY, renderer);
	mPlatformTexture.render(mPosX + mPlatformTexture.getWidth(), mPosY, renderer);
	if (mPosX < -mPlatformTexture.getWidth())
		mPosX = 0;
};

bool Platform::checkIfHorseLandedOnPlatform(SDL_Rect* horseCollider) {
	int bottomHorseCollider;
	bottomHorseCollider = (*horseCollider).y + (*horseCollider).h;

	if (bottomHorseCollider >= mPosY)
		return true;

	return false;
};