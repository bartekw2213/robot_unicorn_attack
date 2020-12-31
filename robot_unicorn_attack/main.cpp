#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "constants.h"
#include "Texture.h"
#include "Unicorn.h"
#include "MyPlatform.h"

bool loadMedia(Texture* bgTexture, Unicorn* unicornObject, MyPlatform* platform1, MyPlatform* platform2, MyPlatform* platform3, SDL_Renderer* renderer) {
	bool success = true;

	char bgTexturePath[MAX_PATH_LENGTH] = "../images/background.png";
	if (!(*bgTexture).loadFromFile(bgTexturePath, renderer)) {
		printf("Failed to load background' texture image!\n");
		success = false;
	};

	if (!platform1->loadTexture(renderer) || !platform2->loadTexture(renderer) || !platform3->loadTexture(renderer) || !unicornObject->loadTextures(renderer))
		success = false;

	return success;
}

bool init(SDL_Window** window, SDL_Renderer** renderer) {
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		*window = SDL_CreateWindow("Robot Unicorn Attack", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (*window == NULL) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (*renderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

void projectDefaultControls(SDL_Event* e, Unicorn* unicornObject, bool* quit, int* scrollingOffsetVel, bool* gameDefaultControlsEnabled) {
	if ((*e).type == SDL_QUIT)
		*quit = true;
	else if ((*e).type == SDL_KEYDOWN && (*e).key.repeat == 0) {
		switch ((*e).key.keysym.sym) {
		case SDLK_RIGHT: *scrollingOffsetVel -= UNICORN_START_SPEED; break;
		case SDLK_UP: (*unicornObject).jump(true); break;
		case SDLK_d: *gameDefaultControlsEnabled = true; break;
		case SDLK_ESCAPE: *quit = true; break;
		}
	}
	else if ((*e).type == SDL_KEYUP && (*e).key.repeat == 0) {
		switch ((*e).key.keysym.sym) {
		case SDLK_RIGHT: *scrollingOffsetVel += UNICORN_START_SPEED; break;
		case SDLK_UP: (*unicornObject).jump(false); break;
		}
	}
}

void gameDefaultControls(SDL_Event* e, Unicorn* unicornObject, bool* quit, int* scrollingOffsetVel, bool* gameDefaultControlsEnabled) {
	if ((*e).type == SDL_QUIT)
		*quit = true;
	else if ((*e).type == SDL_KEYDOWN && (*e).key.repeat == 0) {
		switch ((*e).key.keysym.sym) {
		case SDLK_z: (*unicornObject).jump(true); break;
		case SDLK_x: (*unicornObject).dash(true); break;
		case SDLK_d: *gameDefaultControlsEnabled = false; *scrollingOffsetVel = 0; break;
		case SDLK_ESCAPE: *quit = true; break;
		}
	}
	else if ((*e).type == SDL_KEYUP && (*e).key.repeat == 0) {
		switch ((*e).key.keysym.sym) {
		case SDLK_z: (*unicornObject).jump(false); break;
		case SDLK_x: (*unicornObject).dash(false); break;
		}
	}
}

void controlGameSpeedBasedOnTime(Uint32 currentTime, bool isHorseDashing, int* scrollingOffsetVel) {
	if (isHorseDashing) {
		*scrollingOffsetVel = -UNICORN_DASH_SPEED;
		return;
	}
	
	if (*scrollingOffsetVel > -UNICORN_MAX_SPEED || *scrollingOffsetVel == -UNICORN_DASH_SPEED)
		*scrollingOffsetVel = -((int)currentTime / 1000) / UNICORN_ACCELERATION_TEMPO - UNICORN_START_SPEED;
}

bool checkIfUnicornLandedOnPlatform(Unicorn* unicornObject, MyPlatform* platform1, MyPlatform* platform2, MyPlatform* platform3) {
	if (platform1->checkIfUnicornLandedOnPlatform(unicornObject->getCollider()))
		return true;

	if (platform2->checkIfUnicornLandedOnPlatform(unicornObject->getCollider()))
		return true;

	if (platform3->checkIfUnicornLandedOnPlatform(unicornObject->getCollider()))
		return true;

	return false;
}

bool checkIfUnicornCrashedIntoPlatform(Unicorn* unicornObject, MyPlatform* platform1, MyPlatform* platform2, MyPlatform* platform3) {
	if (platform1->checkIfUnicornCrashedIntoPlatform(unicornObject->getCollider())) 
		return true;

	if (platform2->checkIfUnicornCrashedIntoPlatform(unicornObject->getCollider())) 
		return true;

	if (platform3->checkIfUnicornCrashedIntoPlatform(unicornObject->getCollider()))
		return true;

	return false;
}

void close(SDL_Window* window, SDL_Renderer* renderer) {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	window = NULL;
	renderer = NULL;
}

int main()
{
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Event e;
	Texture bgTexture;
	Unicorn unicornObject;
	MyPlatform platform1(1);
	MyPlatform platform2(2);
	MyPlatform platform3(3);

	bool quit = false;
	bool gameDefaultControlsEnabled = false;
	int scrollingOffsetVel = 0;
	int scrollingYOffsetVel = 0;
	srand(time(0));									//	przy pomocy rand() bede generowac losowo gdzie ma pojawic sie przeszkoda lub platforma


	if (!init(&window, &renderer))
		printf("Failed to initialize\n");
	else if (!loadMedia(&bgTexture, &unicornObject, &platform1, &platform2, &platform3, renderer))
		printf("Failed to load media\n");
	else {
		Uint32 gameStartTime = SDL_GetTicks();

		while (!quit) {
			Uint32 currentTime = SDL_GetTicks() - gameStartTime;

			while (SDL_PollEvent(&e) != 0) {
				if(gameDefaultControlsEnabled)
					gameDefaultControls(&e, &unicornObject, &quit, &scrollingOffsetVel, &gameDefaultControlsEnabled);
				else
					projectDefaultControls(&e, &unicornObject, &quit, &scrollingOffsetVel, &gameDefaultControlsEnabled);
			}

			if (gameDefaultControlsEnabled)
				controlGameSpeedBasedOnTime(currentTime, unicornObject.getIsUnicornDashing(), &scrollingOffsetVel);

			unicornObject.manipulateUnicornOnYAxis(checkIfUnicornLandedOnPlatform(&unicornObject, &platform1, &platform2, &platform3), &scrollingYOffsetVel);

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			bgTexture.resizeAndRender(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer);
			platform1.render(renderer, scrollingOffsetVel, scrollingYOffsetVel);
			platform2.render(renderer, scrollingOffsetVel, scrollingYOffsetVel);
			platform3.render(renderer, scrollingOffsetVel, scrollingYOffsetVel);
			unicornObject.render(renderer);

			if(checkIfUnicornCrashedIntoPlatform(&unicornObject, &platform1, &platform2, &platform3))
				printf("Kolizja");

			SDL_RenderPresent(renderer);
		}

		close(window, renderer);
	}

    return 0;
}

