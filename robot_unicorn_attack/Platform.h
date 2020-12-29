#pragma once
#include <SDL.h>
#include "Texture.h"

class Platform {
public:
	Platform();
	~Platform();

	bool loadTexture(SDL_Renderer* renderer);
	void render(SDL_Renderer* renderer, int scrollingOffsetVel);
	bool checkIfHorseLandedOnPlatform(SDL_Rect* horseCollider);

private:
	int mPosX, mPosY;
	Texture mPlatformTexture;
};

