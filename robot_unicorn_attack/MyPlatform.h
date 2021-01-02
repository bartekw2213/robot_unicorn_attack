#pragma once
#include <SDL.h>
#include "Texture.h"

class MyPlatform {
public:
	MyPlatform();
	~MyPlatform();

	void initPlatformProperties(int platformType);
	bool loadTexture(SDL_Renderer* renderer);
	bool checkIfUnicornLandedOnPlatform(SDL_Rect* unicornCollider);
	bool checkIfUnicornCrashedIntoPlatform(SDL_Rect* unicornCollider);
	void render(SDL_Renderer* renderer, int scrollingOffsetVel, int scrollingYOffsetVel);
	void restartPlatform();
	SDL_Rect* getCollider(int colliderNum);
	int getPosY();

private:
	int mPosX, mPosY;
	int mPlatformTypeNumber;
	int mPlatformWidth, mPlatformHeight;
	int mMinPlatformY, mMaxPlatformY;
	int mPlatfromCollidersNum;
	Texture mMyPlatformTexture;
	SDL_Rect mPlatformColliders[MAX_COLLIDERS];

	int generatePlatformY();
	int generatePlatformX();
	void createPlatformColliders(SDL_Rect platformColliders[MAX_COLLIDERS]);
	void shiftColliders();
};

