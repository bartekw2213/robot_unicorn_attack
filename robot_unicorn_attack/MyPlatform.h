#pragma once
#include <SDL.h>
#include "Texture.h"
#include "Star.h"

class MyPlatform {
public:
	MyPlatform();
	~MyPlatform();

	void initPlatformProperties(int platformType);
	bool loadTexture(SDL_Renderer* renderer);
	bool checkIfUnicornLandedOnPlatform(SDL_Rect* unicornCollider);
	bool checkIfUnicornCrashedIntoPlatformOrStar(SDL_Rect* unicornCollider, bool isUnicornDashing);
	void render(SDL_Renderer* renderer, int XscrollingVelocity, int YscrollingVelocity);
	void restartPlatform();
	SDL_Rect* getCollider(int colliderNum);
	int getPosY();

private:
	int mPosX, mPosY;
	int mPlatformTypeNumber;
	int mPlatformWidth, mPlatformHeight;
	int mMinPlatformY, mMaxPlatformY;
	int mPlatfromCollidersNum;
	bool mRenderStar;
	Star mStarRenderedOnPlatform;
	Texture mMyPlatformTexture;
	SDL_Rect mPlatformColliders[MAX_COLLIDERS];

	int generatePlatformY();
	int generatePlatformX();
	void shiftColliders();
	void createPlatformColliders(SDL_Rect platformColliders[MAX_COLLIDERS]);
	void createStarIfNeeded(SDL_Renderer* renderer);
	bool checkIfUnicornLandedOnCollider(SDL_Rect* platformCollider, SDL_Rect* unicornCollider);
	bool checkIfUnicornCrashedIntoColliderOrStar(SDL_Rect* platformCollider, SDL_Rect* unicornCollider, bool isUnicornDashing);
};

