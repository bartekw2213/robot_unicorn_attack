#pragma once
#include <SDL.h>
#include "Texture.h"
#include "constants.h"

class Unicorn
{
public:
	Unicorn();
	~Unicorn();

	void render(SDL_Renderer* renderer, int scrollingYOffsetVel);
	void shiftCollider();
	bool loadTextures(SDL_Renderer* renderer);
	void manipulateUnicornOnYAxis(bool unicornLandedOnPlatform, int* scrollingYOffset);
	void restartUnicorn();
	void jump(bool isPlayerHoldingJumpKey);
	void dash(bool isPlayerHoldingDash);
	void explode();
	void fellOver();
	bool getIsUnicornDashing();
	bool getDoesUnicornExploded();
	bool hasExplosionEnded();
	bool hasFallingOverEnded();
	SDL_Rect* getCollider();

private:
	bool mIsPlayerHoldingJumpKey, mIsUnicornDashing, mIsUnicornFreeFallingAfterDash, mDoesUnicornExploded, mDoesUnicornFellOver;
	int mHowManyTimesUnicornJumped;
	int mPosX, mPosY;
	SDL_Rect mCollider;
	Uint32 mTimeWhenUnicornJumped, mTimeWhenFreeFallStarted, mTimeWhenUnicornDashed, mTimeWhenUnicornExploded, mTimeWhenUnicornFellOver;
	Texture mUnicornRunningTextures[UNICORN_TEXTURES_NUM], mUnicornDashingTextures[UNICORN_DASHING_TEXTURES_NUM], 
		mUnicornFallingTextures[UNICORN_FALLING_TEXTURES_NUM], mUnicornJumpingTextures[UNICORN_JUMPING_TEXTURES_NUM],
		mUnicornExplosionTextures[UNICORN_EXPLOSION_TEXTURES_NUM];

	bool mLoadUnicornAnimationFrames(int numOfFrames, char const* pathToFolderWithFrames, Texture* arrayOfFrames, SDL_Renderer* renderer);
};

