#pragma once
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Texture.h"
#include "constants.h"

class Obstacle {
public:
	Obstacle();
	~Obstacle();

	bool loadTexture(SDL_Renderer* renderer);
	void render(SDL_Renderer* renderer, int scrollingOffsetVel);
	bool checkIfHorseRanIntoObstacle(SDL_Rect* horseCollider);
	bool checkIfHorseLandedOnObstacle(SDL_Rect* horseCollider);

private:
	int mPosX, mPosY;
	Texture obstacleTexture;
};