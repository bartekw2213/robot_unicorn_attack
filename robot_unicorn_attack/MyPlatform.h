#pragma once
#include <SDL.h>
#include "Texture.h"

class MyPlatform {
public:
	MyPlatform(int platformTypeNumber);
	~MyPlatform();

	bool loadTexture(SDL_Renderer* renderer);
	bool checkIfUnicornLandedOnPlatform(SDL_Rect* unicornCollider);
	bool checkIfUnicornCrashedIntoPlatform(SDL_Rect* unicornCollider);
	void render(SDL_Renderer* renderer, int scrollingOffsetVel, int scrollingYOffsetVel);
	SDL_Rect* getCollider(int colliderNum);

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

