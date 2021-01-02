#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Unicorn.h"

Unicorn::Unicorn() {
	mPosX = 0;
	mPosY = SCREEN_HEIGHT / 3;
	mHowManyTimesUnicornJumped = 0;
	mTimeWhenUnicornJumped = 0;
	mTimeWhenFreeFallStarted = 0;
	mTimeWhenUnicornExploded = 0;
	mTimeWhenUnicornFellOver = 0;
	mTimeWhenUnicornDashed = 0;
	mDoesUnicornExploded = false;
	mDoesUnicornFellOver = false;
	mIsUnicornDashing = false;
	mIsPlayerHoldingJumpKey = false;
	mIsUnicornFreeFallingAfterDash = false;
	mCollider.w = UNICORN_WIDTH / 2;
	mCollider.h = UNICORN_HEIGHT * 3 / 4;
	shiftCollider();
};

Unicorn::~Unicorn() {
	for (int i = 0; i < UNICORN_TEXTURES_NUM; i++)
		mUnicornRunningTextures[i].free();
};

void Unicorn::render(SDL_Renderer* renderer, int scrollingYOffsetVel) {
	if (mDoesUnicornExploded) {
		int frameToDraw = (SDL_GetTicks() - mTimeWhenUnicornExploded) / (float)UNICORN_EXPLOSION_TIME * (UNICORN_EXPLOSION_TEXTURES_NUM - 1);
		mUnicornExplosionTextures[frameToDraw].resizeAndRender(mPosX, mPosY, UNICORN_WIDTH, UNICORN_HEIGHT, renderer);
	}
	else if (mIsUnicornDashing) {
		int frameToDraw = (SDL_GetTicks() - mTimeWhenUnicornDashed) / (float)UNICORN_DASH_TIME * (UNICORN_DASHING_TEXTURES_NUM - 1);
		mUnicornDashingTextures[frameToDraw].resizeAndRender(mPosX, mPosY, UNICORN_WIDTH * UNICORN_TEXTURES_DASH_TO_RUN_SIZE_RATIO, UNICORN_HEIGHT * UNICORN_TEXTURES_DASH_TO_RUN_SIZE_RATIO, renderer);
	}
	else if (scrollingYOffsetVel < 0) {
		int frameToDraw = SDL_GetTicks() % UNICORN_FALLING_TEXTURES_NUM;

		if(mDoesUnicornFellOver)
			mPosY -= scrollingYOffsetVel;

		mUnicornFallingTextures[frameToDraw].resizeAndRender(mPosX, mPosY, UNICORN_WIDTH, UNICORN_HEIGHT, renderer);
	}
	else if (scrollingYOffsetVel > 0) {
		int frameToDraw = SDL_GetTicks() % UNICORN_JUMPING_TEXTURES_NUM;
		mUnicornJumpingTextures[frameToDraw].resizeAndRender(mPosX, mPosY, UNICORN_WIDTH, UNICORN_HEIGHT, renderer);
	}
	else {
		int frameToDraw = SDL_GetTicks() % UNICORN_TEXTURES_NUM;
		mUnicornRunningTextures[frameToDraw].resizeAndRender(mPosX, mPosY, UNICORN_WIDTH, UNICORN_HEIGHT, renderer);
	}
};

void Unicorn::shiftCollider() {
	mCollider.x = mPosX + UNICORN_WIDTH / 3;
	mCollider.y = mPosY + UNICORN_HEIGHT / 5;

	if (mIsUnicornDashing)
		mCollider.x = mPosX + UNICORN_WIDTH / 1.5;
};

bool Unicorn::loadTextures(SDL_Renderer* renderer) {
	if (!mLoadUnicornAnimationFrames(UNICORN_TEXTURES_NUM, "../images/unicorn_run/", mUnicornRunningTextures, renderer))
		return false;

	if (!mLoadUnicornAnimationFrames(UNICORN_DASHING_TEXTURES_NUM, "../images/unicorn_dash/", mUnicornDashingTextures, renderer))
		return false;

	if (!mLoadUnicornAnimationFrames(UNICORN_FALLING_TEXTURES_NUM, "../images/unicorn_falling/", mUnicornFallingTextures, renderer))
		return false;

	if (!mLoadUnicornAnimationFrames(UNICORN_JUMPING_TEXTURES_NUM, "../images/unicorn_jump/", mUnicornJumpingTextures, renderer))
		return false;

	if (!mLoadUnicornAnimationFrames(UNICORN_EXPLOSION_TEXTURES_NUM, "../images/unicorn_explosion/", mUnicornExplosionTextures, renderer))
		return false;

	return true;
};

void Unicorn::manipulateUnicornOnYAxis(bool unicornLandedOnPlatform, int* scrollingYOffsetVel) {
	// jednorozec pozostaje nieruchomy na osi Y podczas gdy wykonuje zryw
	if (mIsUnicornDashing) {
		*scrollingYOffsetVel = 0;

		// wylacza zryw jednorozca po okreslonym czasie
		if (SDL_GetTicks() - mTimeWhenUnicornDashed > UNICORN_DASH_TIME) {
			mIsUnicornDashing = false;
			mIsUnicornFreeFallingAfterDash = true;
			mTimeWhenFreeFallStarted = SDL_GetTicks();
		}
	}
	// obsluga skoku jednorozca
	else if (mHowManyTimesUnicornJumped != 0 && !mIsUnicornFreeFallingAfterDash) {
		Uint32 jumpTime = SDL_GetTicks() - mTimeWhenUnicornJumped;

		if (unicornLandedOnPlatform && jumpTime > JUMP_TIME_OFFSET)
			mHowManyTimesUnicornJumped = 0;
		else if (mIsPlayerHoldingJumpKey && jumpTime <= UNICORN_MAX_LONG_JUMP_TIME && jumpTime > JUMP_TIME_OFFSET)
			*scrollingYOffsetVel = (UNICORN_EXTENDED_JUMP_POWER - jumpTime) / GRAVITATION_FACTOR;
		else if ((UNICORN_NORMAL_JUMP_POWER - jumpTime / GRAVITATION_FACTOR) > 0)
			*scrollingYOffsetVel = (UNICORN_NORMAL_JUMP_POWER - jumpTime / GRAVITATION_FACTOR);
		else
			mTimeWhenFreeFallStarted = SDL_GetTicks();
	}
	// obsluga swobodnego spadania jednorozca - np. gdy spada z platformy lub przeszkody
	else if (!unicornLandedOnPlatform) {
		if (mTimeWhenFreeFallStarted == 0)
			mTimeWhenFreeFallStarted = SDL_GetTicks();

		Uint32 freeFallTime = SDL_GetTicks() - mTimeWhenFreeFallStarted;
		*scrollingYOffsetVel = 0 - (freeFallTime / GRAVITATION_FACTOR);
	}
	else {
		*scrollingYOffsetVel = 0;
		mTimeWhenFreeFallStarted = 0;
		mIsUnicornFreeFallingAfterDash = false;
	}

	shiftCollider();
};

void Unicorn::jump(bool isPlayerHoldingJumpKey) {
	if (mDoesUnicornExploded)
		return;

	mIsPlayerHoldingJumpKey = isPlayerHoldingJumpKey;
	if (mHowManyTimesUnicornJumped < 2 && isPlayerHoldingJumpKey && !mIsUnicornDashing) {
		++mHowManyTimesUnicornJumped;
		mTimeWhenUnicornJumped = SDL_GetTicks();
		mIsUnicornFreeFallingAfterDash = false;
	}
};

void Unicorn::dash(bool isPlayerHoldingDash) {
	// gdy gracz przestanie trzymacz przycisk X przestan wykonywac zryw
	if (!isPlayerHoldingDash) {
		mIsUnicornDashing = false;
		return;
	}

	// pozwol na wykonanie zrywu jesli poprzedni byl wystarczajaco czasu temu
	if (SDL_GetTicks() - mTimeWhenUnicornDashed > UNICORN_MIN_TIME_BETWEEN_DASHES && isPlayerHoldingDash) {
		mIsUnicornDashing = true;
		mTimeWhenUnicornDashed = SDL_GetTicks();
		if (mHowManyTimesUnicornJumped == 2)
			--mHowManyTimesUnicornJumped;
	}
};

void Unicorn::explode() {
	mTimeWhenUnicornExploded = SDL_GetTicks();
	mDoesUnicornExploded = true;
}

void Unicorn::fellOver() {
	if (mDoesUnicornFellOver)
		return;

	mTimeWhenUnicornFellOver = SDL_GetTicks();
	mDoesUnicornFellOver = true;
}

bool Unicorn::getIsUnicornDashing() {
	return mIsUnicornDashing;
};

bool Unicorn::getDoesUnicornExploded() {
	return mDoesUnicornExploded;
}

bool Unicorn::hasExplosionEnded() {
	return SDL_GetTicks() - mTimeWhenUnicornExploded >= UNICORN_EXPLOSION_TIME;
}

bool Unicorn::hasFallingOverEnded() {
	if (mTimeWhenUnicornFellOver == 0)
		return false;

	return SDL_GetTicks() - mTimeWhenUnicornFellOver >= UNICORN_FELLING_OVER_TIME;
}

SDL_Rect* Unicorn::getCollider() {
		return &mCollider;
};

bool Unicorn::mLoadUnicornAnimationFrames(int numOfFrames, char const* pathToFolderWithFrames, Texture* arrayOfFrames, SDL_Renderer* renderer) {

	for (int i = 1; i <= numOfFrames; i++) {
		char path[MAX_PATH_LENGTH] = "";
		strncat(path, pathToFolderWithFrames, strlen(pathToFolderWithFrames));

		// dodanie do sciezki nazwe wczytywanego pliku (nazwa zdjec danej animacji jednorozca to liczby 1.png, 2.png, 3.png, ...)
		char a[MAX_TEXTURE_FILE_LENGTH];
		_itoa(i, a, 10);
		strncat(path, a, strlen(a));
		strncat(path, ".png", strlen(".png"));

		if (!arrayOfFrames[i - 1].loadFromFile(path, renderer)) {
			printf("Failed to load unicorn animation images!\n");
			return false;
		}
	}

	return true;
};

void Unicorn::restartUnicorn() {
	mPosX = 0;
	mPosY = SCREEN_HEIGHT / 3;
	mHowManyTimesUnicornJumped = 0;
	mTimeWhenUnicornJumped = 0;
	mTimeWhenFreeFallStarted = 0;
	mTimeWhenUnicornExploded = 0;
	mTimeWhenUnicornFellOver = 0;
	mTimeWhenUnicornDashed = 0;
	mDoesUnicornExploded = false;
	mDoesUnicornFellOver = false;
	mIsUnicornDashing = false;
	mIsPlayerHoldingJumpKey = false;
	mIsUnicornFreeFallingAfterDash = false;
}