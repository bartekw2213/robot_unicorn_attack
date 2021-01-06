#pragma once
#include "Texture.h"

class Star
{
public:
	Star();
	~Star();
	bool loadTexture(SDL_Renderer* renderer);
	bool checkIfUnicornCrashedIntoStar(SDL_Rect* unicornCollider);
	void render(SDL_Renderer* renderer, SDL_Rect platformColliders[MAX_COLLIDERS]);
	void initStarPosition(SDL_Rect platformColliders[MAX_COLLIDERS], int numOfPlatformColliders);
	void restartStar();
	void explode();

private:
	Texture mStarTexture, mExplosionTextures[STAR_EXPLOSION_TEXTURE_NUM];
	Uint32 mTimeStarExploded;
	int mColliderIndexWhereStarIsRendered, mStarXOffset;
	int mPosX, mPosY;
	bool mExploded;
};

