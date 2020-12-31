#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MyPlatform.h"

MyPlatform::MyPlatform(int platformTypeNumber) {
	mPlatformTypeNumber = platformTypeNumber;

	switch (mPlatformTypeNumber) {
	case 1: 
		mPlatformWidth = PLATFORM1_WIDTH; mPlatformHeight = PLATFORM1_HEIGHT; 
		mMinPlatformY = MIN_PLATFORM1_Y; mMaxPlatformY = MAX_PLATFORM1_Y;
		mPlatfromCollidersNum = PLATFORM1_COLLIDERS_NUM;
		mPosX = 0;
		break;
	case 2:
		mPlatformWidth = PLATFORM2_WIDTH; mPlatformHeight = PLATFORM2_HEIGHT;
		mMinPlatformY = MIN_PLATFORM2_Y; mMaxPlatformY = MAX_PLATFORM2_Y;
		mPlatfromCollidersNum = PLATFORM2_COLLIDERS_NUM;
		mPosX = PLATFORM1_WIDTH + DISTANCE_BETWEEN_PLATFORMS;
		break;
	case 3:
		mPlatformWidth = PLATFORM3_WIDTH; mPlatformHeight = PLATFORM3_HEIGHT;
		mMinPlatformY = MIN_PLATFORM3_Y; mMaxPlatformY = MAX_PLATFORM3_Y;
		mPlatfromCollidersNum = PLATFORM3_COLLIDERS_NUM;
		mPosX = PLATFORM1_WIDTH + PLATFORM2_WIDTH + 2 * DISTANCE_BETWEEN_PLATFORMS;
		break;
	}

	mPosY = generatePlatformY(); //odkomentowac
}

MyPlatform::~MyPlatform() {
	mMyPlatformTexture.free();
}

bool MyPlatform::loadTexture(SDL_Renderer* renderer) {
	char platformTexturePath[MAX_PATH_LENGTH] = "";

	switch (mPlatformTypeNumber) {
		case 1: strncpy(platformTexturePath, "../images/platforms/platform1.png", strlen("../images/platforms/platform1.png")); break;
		case 2: strncpy(platformTexturePath, "../images/platforms/platform2.png", strlen("../images/platforms/platform2.png")); break;
		case 3: strncpy(platformTexturePath, "../images/platforms/platform3.png", strlen("../images/platforms/platform3.png")); break;
	}

	if (!(mMyPlatformTexture).loadFromFile(platformTexturePath, renderer)) {
		printf("Failed to load platform texture image!\n");
		return false;
	};

	createPlatformColliders(mPlatformColliders);

	return true;
}

void MyPlatform::render(SDL_Renderer* renderer, int scrollingOffsetVel, int scrollingYOffsetVel) {
	mPosX += scrollingOffsetVel;
	mPosY += scrollingYOffsetVel;

	mMyPlatformTexture.resizeAndRender(mPosX, mPosY, mPlatformWidth, mPlatformHeight, renderer);
	shiftColliders();

	if (mPosX < -mPlatformWidth) {
		mPosX = generatePlatformX();
		mPosY = generatePlatformY();
	}

}

int MyPlatform::generatePlatformY() {
	int y = 0;

	int numberToAdd = (rand() % (mMaxPlatformY - mMinPlatformY));

	while (y < mMinPlatformY)
		y = mMinPlatformY + numberToAdd + 1;

	return y;
}

int MyPlatform::generatePlatformX() {
	int x = 0;

	switch (mPlatformTypeNumber) {
		case 1: x = PLATFORM2_WIDTH + 2 * DISTANCE_BETWEEN_PLATFORMS; break;
		case 2: x = PLATFORM1_WIDTH + 2 * DISTANCE_BETWEEN_PLATFORMS; break;
		case 3: x = PLATFORM2_WIDTH + 2 * DISTANCE_BETWEEN_PLATFORMS; break;
	}

	return x;
}

void MyPlatform::shiftColliders() {
	switch (mPlatformTypeNumber) {
		case 1:
			mPlatformColliders[0].y = mPosY + mPlatformHeight / 5;
			mPlatformColliders[0].x = mPosX;
			mPlatformColliders[1].y = mPosY + mPlatformHeight / 15;
			mPlatformColliders[1].x = mPosX + mPlatformWidth / 4.2;
			mPlatformColliders[2].y = mPosY;
			mPlatformColliders[2].x = mPosX + mPlatformWidth / 1.6;
			break;
		case 2:
			mPlatformColliders[0].y = mPosY + mPlatformHeight / 5;
			mPlatformColliders[0].x = mPosX;
			mPlatformColliders[1].y = mPosY;
			mPlatformColliders[1].x = mPosX + mPlatformWidth / 1.7;
			break;
		case 3: 
			mPlatformColliders[0].y = mPosY;
			mPlatformColliders[0].x = mPosX;
			break;
	}
		
};


void MyPlatform::createPlatformColliders(SDL_Rect platformColliders[MAX_COLLIDERS]) {
	switch (mPlatformTypeNumber) {
		case 1:
			platformColliders[0].w = mPlatformWidth;
			platformColliders[0].h = mPlatformHeight / 3;
			platformColliders[1].w = mPlatformWidth / 11;
			platformColliders[1].h = mPlatformHeight / 7;
			platformColliders[2].w = mPlatformWidth / 6;
			platformColliders[2].h = mPlatformHeight / 5;
			break;
		case 2:
			platformColliders[0].w = mPlatformWidth / 2.5;
			platformColliders[0].h = mPlatformHeight / 4;
			platformColliders[1].w = mPlatformWidth / 2.5;
			platformColliders[1].h = mPlatformHeight / 4;
			break;
		case 3:
			platformColliders[0].w = mPlatformWidth;
			platformColliders[0].h = mPlatformHeight / 2;
			break;
	}
}

SDL_Rect* MyPlatform::getCollider(int colliderNum) {
	return &mPlatformColliders[colliderNum];
};

bool MyPlatform::checkIfUnicornLandedOnPlatform(SDL_Rect* unicornCollider) {
	int bottomUnicornCollider, unicornStartX, unicornEndX;
	bottomUnicornCollider = (*unicornCollider).y + (*unicornCollider).h;
	unicornStartX = (*unicornCollider).x;
	unicornEndX = (*unicornCollider).x + (*unicornCollider).w;

	for(int i = 0; i < mPlatfromCollidersNum; i++)
		if (bottomUnicornCollider >= mPlatformColliders[i].y - PLATFORMS_Y_COLLISION_OFFSET &&
			bottomUnicornCollider <= mPlatformColliders[i].y + mPlatformColliders[i].h &&
			unicornEndX >= mPlatformColliders[i].x &&
			unicornStartX <= mPlatformColliders[i].x + mPlatformColliders[i].w) 
			return true;

	return false;
};

bool MyPlatform::checkIfUnicornCrashedIntoPlatform(SDL_Rect* unicornCollider) {
	int unicornBottomY, unicornTopY, unicornRightX, unicornLeftX;
	unicornTopY = (*unicornCollider).y;
	unicornBottomY = (*unicornCollider).y + (*unicornCollider).h;
	unicornRightX = (*unicornCollider).x + (*unicornCollider).w;
	unicornLeftX = (*unicornCollider).x;

	for (int i = 0; i < mPlatfromCollidersNum; i++)
		if (unicornRightX > mPlatformColliders[i].x &&
			unicornTopY < mPlatformColliders[i].y + mPlatformColliders[i].h &&
			unicornBottomY > mPlatformColliders[i].y + PLATFORMS_Y_COLLISION_OFFSET &&
			unicornLeftX < mPlatformColliders[i].x + mPlatformColliders[i].w) 
			return true;

	return false;
};
