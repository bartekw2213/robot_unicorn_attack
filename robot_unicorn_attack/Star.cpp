#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Star.h"

Star::Star() {
	mColliderIndexWhereStarIsRendered = -1;
	mStarXOffset = -1;
	mPosX = -1000;
	mPosY = -1000;
	mExploded = false;
	mTimeStarExploded = 0;
}

Star::~Star() {
	mColliderIndexWhereStarIsRendered = -1;
	mStarXOffset = -1;
	mStarTexture.free();
}

bool Star::loadTexture(SDL_Renderer* renderer) {
	char pathToFile[MAX_PATH_LENGTH] = "../images/star.png";
	if (!mStarTexture.loadFromFile(pathToFile, renderer)) {
		printf("Failed to load %s texture image!\n", pathToFile);
		return false;
	};

	for (int i = 1; i <= STAR_EXPLOSION_TEXTURE_NUM; i++) {
		char path[MAX_PATH_LENGTH] = "../images/star_explosion/";

		// dodanie do sciezki nazwe wczytywanego pliku
		char a[MAX_TEXTURE_FILE_LENGTH];
		_itoa(i, a, 10);
		strncat(path, a, strlen(a));
		strncat(path, ".png", strlen(".png"));

		if (!mExplosionTextures[i - 1].loadFromFile(path, renderer)) {
			printf("Failed to load star explosion images!\n");
			return false;
		}
	}

	return true;
}

bool Star::checkIfUnicornCrashedIntoStar(SDL_Rect* unicornCollider) {
	int unicornTopY = unicornCollider->y;
	int unicornBottomY = unicornCollider->y + unicornCollider->h;
	int unicornRightX = unicornCollider->x + unicornCollider->w;
	int unicornLeftX = unicornCollider->x;

	if (unicornRightX < mPosX)
		return false;
	else if (unicornLeftX > mPosX + mStarTexture.getWidth())
		return false;
	else if (unicornTopY > mPosY + mStarTexture.getHeight())
		return false;
	else if (unicornBottomY < mPosY)
		return false;

	return true;
}

void Star::render(SDL_Renderer* renderer, SDL_Rect platformColliders[MAX_COLLIDERS]) {
	mPosX = platformColliders[mColliderIndexWhereStarIsRendered].x + mStarXOffset;
	mPosY = platformColliders[mColliderIndexWhereStarIsRendered].y - mStarTexture.getHeight();

	if (mExploded) {
		int frameToDraw = (SDL_GetTicks() - mTimeStarExploded) / (float)STAR_EXPLOSION_TIME * (STAR_EXPLOSION_TEXTURE_NUM - 1);
		
		if (frameToDraw < STAR_EXPLOSION_TEXTURE_NUM) {
			mPosY = platformColliders[mColliderIndexWhereStarIsRendered].y - mExplosionTextures[0].getHeight();
			mExplosionTextures[frameToDraw].render(mPosX, mPosY, renderer);
			return;
		}
	}

	mStarTexture.render(mPosX, mPosY, renderer);
}

void Star::initStarPosition(SDL_Rect platformColliders[MAX_COLLIDERS], int numOfPlatformColliders) {
	mColliderIndexWhereStarIsRendered = rand() % numOfPlatformColliders;
	mStarXOffset = rand() % (platformColliders[mColliderIndexWhereStarIsRendered].w - mStarTexture.getWidth());
	mExploded = false;
}

void Star::restartStar() {
	mColliderIndexWhereStarIsRendered = -1;
	mStarXOffset = -1;
	mPosX = -1000;
	mPosY = -1000;
	mExploded = false;
	mTimeStarExploded = 0;
}

void Star::explode() {
	mExploded = true;
	mTimeStarExploded = SDL_GetTicks();
}