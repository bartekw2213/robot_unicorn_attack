#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MyPlatform.h"

MyPlatform::MyPlatform() {
	mPlatformTypeNumber = 0;
	mPlatformWidth = 0; 
	mPlatformHeight = 0;
	mMinPlatformY = 0; 
	mMaxPlatformY = 0;
	mPlatfromCollidersNum = 0;
	mPosX = 0;
	mPosY = 0;
	mRenderStar = false;
}

MyPlatform::~MyPlatform() {
	mMyPlatformTexture.free();
}

void MyPlatform::initPlatformProperties(int platformTypeNumber) {
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
	case 4:
		mPlatformWidth = PLATFORM4_WIDTH; mPlatformHeight = PLATFORM4_HEIGHT;
		mMinPlatformY = MIN_PLATFORM4_Y; mMaxPlatformY = MAX_PLATFORM4_Y;
		mPlatfromCollidersNum = PLATFORM4_COLLIDERS_NUM;
		mPosX = PLATFORM1_WIDTH + DISTANCE_BETWEEN_PLATFORMS;
		break;
	}

	mPosY = generatePlatformY();
}

bool MyPlatform::loadTexture(SDL_Renderer* renderer) {
	char platformTexturePath[MAX_PATH_LENGTH] = "";

	switch (mPlatformTypeNumber) {
		case 1: strncpy(platformTexturePath, "../images/platforms/platform1.png", strlen("../images/platforms/platform1.png")); break;
		case 2: strncpy(platformTexturePath, "../images/platforms/platform2.png", strlen("../images/platforms/platform2.png")); break;
		case 3: strncpy(platformTexturePath, "../images/platforms/platform3.png", strlen("../images/platforms/platform3.png")); break;
		case 4: strncpy(platformTexturePath, "../images/platforms/platform4.png", strlen("../images/platforms/platform4.png")); break;
	}

	if (!(mMyPlatformTexture).loadFromFile(platformTexturePath, renderer)) {
		printf("Failed to load platform texture image!\n");
		return false;
	};

	if (!mStarRenderedOnPlatform.loadTexture(renderer)) {
		printf("Failed to load star texture image!\n");
		return false;
	};


	createPlatformColliders(mPlatformColliders);

	return true;
}

void MyPlatform::createStarIfNeeded(SDL_Renderer* renderer) {
	if (rand() % 100 > CHANCE_TO_RENDER_STAR) {
		mRenderStar = false;
		printf("Nie renderuje\n");
	}
	else {
		printf("Renderuje\n");
		mRenderStar = true;
		mStarRenderedOnPlatform.initStarPosition(mPlatformColliders, mPlatfromCollidersNum);
	}
}

void MyPlatform::render(SDL_Renderer* renderer, int XscrollingVelocity, int YscrollingVelocity) {
	mPosX += XscrollingVelocity;
	mPosY += YscrollingVelocity;

	if (mRenderStar)
		mStarRenderedOnPlatform.render(renderer, mPlatformColliders);

	mMyPlatformTexture.resizeAndRender(mPosX, mPosY, mPlatformWidth, mPlatformHeight, renderer);
	shiftColliders();

	if (mPosX < -mPlatformWidth) {
		mPosX = generatePlatformX();
		mPosY = generatePlatformY();
		createStarIfNeeded(renderer);
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
		case 4: x = PLATFORM1_WIDTH + 2 * DISTANCE_BETWEEN_PLATFORMS; break;
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
			mPlatformColliders[2].y = mPosY;
			mPlatformColliders[2].x = mPosX + mPlatformWidth / 1.3;
			break;
		case 3: 
			mPlatformColliders[0].y = mPosY;
			mPlatformColliders[0].x = mPosX;
			break;
		case 4:
			mPlatformColliders[0].y = mPosY + mPlatformHeight / 7;
			mPlatformColliders[0].x = mPosX;
			mPlatformColliders[1].y = mPosY;
			mPlatformColliders[1].x = mPosX + mPlatformWidth / 2;
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
			platformColliders[2].w = mPlatformWidth / 6;
			platformColliders[2].h = mPlatformHeight;
			break;
		case 3:
			platformColliders[0].w = mPlatformWidth;
			platformColliders[0].h = mPlatformHeight / 2;
			break;
		case 4:
			platformColliders[0].w = mPlatformWidth;
			platformColliders[0].h = mPlatformHeight / 2;
			platformColliders[1].w = mPlatformWidth / 2;
			platformColliders[1].h = mPlatformHeight / 5;
			break;
	}
}

SDL_Rect* MyPlatform::getCollider(int colliderNum) {
	return &mPlatformColliders[colliderNum];
};

bool MyPlatform::checkIfUnicornLandedOnCollider(SDL_Rect* platformCollider, SDL_Rect* unicornCollider) {
	int unicornLeftX = unicornCollider->x;
	int unicornRightX = unicornCollider->x + unicornCollider->w;
	int unicornBottomY = unicornCollider->y + unicornCollider->h;

	if (unicornBottomY < platformCollider->y - PLATFORMS_Y_COLLISION_OFFSET)
		return false;
	else if (unicornBottomY > platformCollider->y + PLATFORMS_Y_COLLISION_OFFSET)
		return false;
	else if (unicornRightX < platformCollider->x)
		return false;
	else if (unicornLeftX > platformCollider->x + platformCollider->w)
		return false;

	return true;
}

bool MyPlatform::checkIfUnicornCrashedIntoColliderOrStar(SDL_Rect* platformCollider, SDL_Rect* unicornCollider, bool isUnicornDashing) {
	int unicornTopY = unicornCollider->y;
	int unicornBottomY = unicornCollider->y + unicornCollider->h;
	int unicornRightX = unicornCollider->x + unicornCollider->w;
	int unicornLeftX = unicornCollider->x;

	if (mStarRenderedOnPlatform.checkIfUnicornCrashedIntoStar(unicornCollider)) {
		if (isUnicornDashing) {
			mStarRenderedOnPlatform.explode();
		}
		else {
			mStarRenderedOnPlatform.restartStar();
			return true;
		}
	}

	if (checkIfUnicornLandedOnCollider(platformCollider, unicornCollider))
		return false;

	if (unicornRightX < platformCollider->x)
		return false;
	else if (unicornLeftX > platformCollider->x + platformCollider->w)
		return false;
	else if (unicornTopY > platformCollider->y + platformCollider->h)
		return false;
	else if (unicornBottomY < platformCollider->y)
		return false;

	return true;
}

bool MyPlatform::checkIfUnicornLandedOnPlatform(SDL_Rect* unicornCollider) {
	for (int i = 0; i < mPlatfromCollidersNum; i++)
		if (checkIfUnicornLandedOnCollider(&mPlatformColliders[i], unicornCollider))
			return true;

	return false;
};

bool MyPlatform::checkIfUnicornCrashedIntoPlatformOrStar(SDL_Rect* unicornCollider, bool isUnicornDashing) {
	for (int i = 0; i < mPlatfromCollidersNum; i++)
		if (checkIfUnicornCrashedIntoColliderOrStar(&mPlatformColliders[i], unicornCollider, isUnicornDashing))
			return true;

	return false;
};

void MyPlatform::restartPlatform() {
	switch (mPlatformTypeNumber) {
	case 1:
		mPosX = 0;
		break;
	case 2:
		mPosX = PLATFORM1_WIDTH + DISTANCE_BETWEEN_PLATFORMS;
		break;
	case 3:
		mPosX = PLATFORM1_WIDTH + PLATFORM2_WIDTH + 2 * DISTANCE_BETWEEN_PLATFORMS;
		break;
	case 4:
		mPosX = PLATFORM1_WIDTH + DISTANCE_BETWEEN_PLATFORMS;
		break;
	}

	mPosY = generatePlatformY();
	mRenderStar = false;
	shiftColliders();
}

int MyPlatform::getPosY() {
	return mPosY;
}