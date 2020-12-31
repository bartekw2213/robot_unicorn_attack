#pragma once
#include <SDL.h>
#include "Texture.h"
#include "constants.h"

class Unicorn
{
public:
	Unicorn();
	~Unicorn();

	void render(SDL_Renderer* renderer);
	void shiftCollider();
	bool loadTextures(SDL_Renderer* renderer);
	void manipulateUnicornOnYAxis(bool unicornLandedOnPlatform, int* scrollingYOffset);
	void jump(bool isPlayerHoldingJumpKey);
	void dash(bool isPlayerHoldingDash);
	bool getIsUnicornDashing();
	SDL_Rect* getCollider();

private:
	bool mIsPlayerHoldingJumpKey, mIsUnicornDashing, mIsUnicornFreeFallingAfterDash;
	int mHowManyTimesUnicornJumped;
	int mPosX, mPosY;
	SDL_Rect mCollider;
	Uint32 mTimeWhenUnicornJumped, mTimeWhenFreeFallStarted, mTimeWhenUnicornDashed;
	Texture mUnicornRunningTextures[UNICORN_TEXTURES_NUM], mUnicornDashingTextures[UNICORN_DASHING_TEXTURES_NUM];

	bool mLoadUnicornAnimationFrames(int numOfFrames, char const* pathToFolderWithFrames, Texture* arrayOfFrames, SDL_Renderer* renderer);
};

