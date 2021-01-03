﻿#define SDL_MAIN_HANDLED
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

bool loadMedia(Texture* bgTexture, Texture* endScreenTexture, Texture* digitsTexture, Unicorn* unicornObject, MyPlatform platforms[PLATFORM_TYPES], 
	Texture wishesLeftIconsTextures[UNICORN_WISHES_NUM], Texture subtitlesTextures[SUBTITLES_TEXTURES_NUM], SDL_Renderer* renderer) {

	char bgTexturePath[MAX_PATH_LENGTH] = "../images/background.png";
	if (!bgTexture->loadFromFile(bgTexturePath, renderer)) {
		printf("Failed to load background' texture image!\n");
		return false;
	};

	char endScreenTexturePath[MAX_PATH_LENGTH] = "../images/end_screen.png";
	if (!endScreenTexture->loadFromFile(endScreenTexturePath, renderer)) {
		printf("Failed to load end screen texture image!\n");
		return false;
	};

	char startSubtitlePath[MAX_PATH_LENGTH] = "../images/subtitles/start_subtitle.png";
	if (!subtitlesTextures[START_SUBTITLE_INDEX].loadFromFile(startSubtitlePath, renderer)) {
		printf("Failed to load start subtitle texture image!\n");
		return false;
	};

	char againSubtitlePath[MAX_PATH_LENGTH] = "../images/subtitles/again_subtitle.png";
	if (!subtitlesTextures[AGAIN_SUBTITLE_INDEX].loadFromFile(againSubtitlePath, renderer)) {
		printf("Failed to load again subtitle texture image!\n");
		return false;
	};

	char digitsTexturePath[MAX_PATH_LENGTH] = "../images/digits.png";
	if (!digitsTexture->loadFromFile(digitsTexturePath, renderer)) {
		printf("Failed to load digits texture image!\n");
		return false;
	};

	for (int i = 0; i < PLATFORM_TYPES; i++) {
		if (!platforms[i].loadTexture(renderer)) {
			printf("Failed to load %i platform texture image\n", (i + 1));
			return false;
		}
	}

	for (int i = 1; i <= UNICORN_WISHES_NUM; i++) {
		char path[MAX_PATH_LENGTH] = "../images/wishes_left_icons/";

		char a[MAX_TEXTURE_FILE_LENGTH];
		_itoa(i, a, 10);
		strncat(path, a, strlen(a));
		strncat(path, ".png", strlen(".png"));

		if (!wishesLeftIconsTextures[i - 1].loadFromFile(path, renderer)) {
			printf("Failed to load unicorn left icons texture images!\n");
			return false;
		}
	}

	if (!unicornObject->loadTextures(renderer)) {
		printf("Failed to load unicorn texture images\n");
		return false;
	}

	return true;
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

bool loadStartingScreen(Texture* startingScreenTexture, SDL_Renderer* renderer) {
	char startingScreenTexturePath[MAX_PATH_LENGTH] = "../images/starting_screen.png";
	if (!(*startingScreenTexture).loadFromFile(startingScreenTexturePath, renderer)) {
		printf("Failed to load starting screen texture image!\n");
		return false;
	};

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	(*startingScreenTexture).resizeAndRender(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer);
	SDL_RenderPresent(renderer);

	return true;
}

void handleMainMenu(SDL_Event* e, SDL_Renderer* renderer, Texture* startingScreenTexture, Texture* startGameSubtitle, bool* closeProgram) {
	bool quit = false;
	
	while (!quit) {

		while (SDL_PollEvent(e) != 0) {
			if ((*e).type == SDL_QUIT) {
				quit = true;
				*closeProgram = true;
			}
			else if ((*e).type == SDL_KEYDOWN && (*e).key.repeat == 0) {
				switch ((*e).key.keysym.sym) {
				case SDLK_z: quit = true; break;
				case SDLK_ESCAPE: quit = true; *closeProgram = true; break;
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		startingScreenTexture->resizeAndRender(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer);
		startGameSubtitle->render(SCREEN_WIDTH / 2 - startGameSubtitle->getWidth() / 2, SCREEN_HEIGHT - startGameSubtitle->getHeight(), renderer);
		SDL_RenderPresent(renderer);
	}
}

void projectDefaultControls(SDL_Event* e, Unicorn* unicornObject, bool* quit, bool* closeProgram, int* scrollingOffsetVel, bool* gameDefaultControlsEnabled) {
	if ((*e).type == SDL_QUIT) {
		*quit = true;
		*closeProgram = true;
	}
	else if ((*e).type == SDL_KEYDOWN && (*e).key.repeat == 0) {
		switch ((*e).key.keysym.sym) {
		case SDLK_RIGHT: *scrollingOffsetVel -= UNICORN_START_SPEED; break;
		case SDLK_UP: (*unicornObject).jump(true); break;
		case SDLK_d: *gameDefaultControlsEnabled = true; break;
		case SDLK_ESCAPE: *quit = true, *closeProgram = true; break;
		}
	}
	else if ((*e).type == SDL_KEYUP && (*e).key.repeat == 0) {
		switch ((*e).key.keysym.sym) {
		case SDLK_RIGHT: *scrollingOffsetVel += UNICORN_START_SPEED; break;
		case SDLK_UP: (*unicornObject).jump(false); break;
		}
	}
}

void gameDefaultControls(SDL_Event* e, Unicorn* unicornObject, bool* quit, bool* closeProgram, int* scrollingOffsetVel, bool* gameDefaultControlsEnabled) {
	if ((*e).type == SDL_QUIT) {
		*quit = true;
		*closeProgram = true;
	}
	else if ((*e).type == SDL_KEYDOWN && (*e).key.repeat == 0) {
		switch ((*e).key.keysym.sym) {
		case SDLK_z: (*unicornObject).jump(true); break;
		case SDLK_x: (*unicornObject).dash(true); break;
		case SDLK_d: *gameDefaultControlsEnabled = false; *scrollingOffsetVel = 0; break;
		case SDLK_ESCAPE: *quit = true, *closeProgram = true; break;
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

bool checkIfUnicornLandedOnPlatform(Unicorn* unicornObject, MyPlatform platforms[PLATFORM_TYPES]) {
	for(int i = 0; i < PLATFORM_TYPES; i++) 
		if (platforms[i].checkIfUnicornLandedOnPlatform(unicornObject->getCollider()))
			return true;

	return false;
}

bool checkIfUnicornCrashedIntoPlatform(Unicorn* unicornObject, MyPlatform platforms[PLATFORM_TYPES]) {
	for (int i = 0; i < PLATFORM_TYPES; i++)
		if (platforms[i].checkIfUnicornCrashedIntoPlatform(unicornObject->getCollider()))
			return true;

	return false;
}

bool checkIfUnicornFellOfPlatforms(MyPlatform platforms[PLATFORM_TYPES]) {
	for (int i = 0; i < PLATFORM_TYPES; i++)
		if (platforms[i].getPosY() >= PLATFORMS_Y_WHEN_FELL_OVER_IS_DETECTED)
			return false;

	return true;
}

void close(SDL_Window* window, SDL_Renderer* renderer) {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	window = NULL;
	renderer = NULL;

	IMG_Quit();
	SDL_Quit();
}

void renderDigit(int digitNum, int digit, int oneDigitWidth, Texture* digitsTexture, SDL_Renderer* renderer) {
	int marginTop = oneDigitWidth, marginLeft = oneDigitWidth * 2;
	SDL_Rect clip = { oneDigitWidth * digit, 0, oneDigitWidth, 0 };

	digitsTexture->renderClipped(digitNum * oneDigitWidth + marginLeft, marginTop, &clip, renderer);
}

void renderTime(Texture* digitsTexture, Uint32 currentTime, SDL_Renderer* renderer) {
	int oneDigitWidth = digitsTexture->getWidth() / NUM_OF_SIGNS_IN_DIGIT_TEXTURE;
	int timeDigits[TIME_DIGITS] = { 0 };

	timeDigits[0] = currentTime / 100000;				// reprezentuje setki obecnego czasu
	timeDigits[1] = currentTime / 10000;				// reprezentuje dziesietne obecnego czasu
	timeDigits[2] = currentTime / 1000 % 10;			// reprezentuje jednosci obecnego czasu
	timeDigits[3] = NUM_OF_SIGNS_IN_DIGIT_TEXTURE - 1;	// reprezentuje kropke oddzielajaca sekundy od minisekund
	timeDigits[4] = (currentTime % 1000) / 100;			// reprezentuje czesci setne milisekund
	timeDigits[5] = (currentTime % 100) / 10;			// reprezentuje czesci dziesietne milisekund

	
	for (int i = 0; i < TIME_DIGITS; i++)
		renderDigit(i, timeDigits[i], oneDigitWidth, digitsTexture, renderer);
}

void renderTextures(SDL_Renderer* renderer, Texture* bgTexture, Texture* digitsTexture, Unicorn* unicornObject, MyPlatform platforms[PLATFORM_TYPES], 
	Texture wishesLeftIconsTextures[UNICORN_WISHES_NUM], Uint32 currentTime, int scrollingOffsetVel, int scrollingYOffsetVel) {

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	bgTexture->resizeAndRender(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer);

	for (int i = 0; i < PLATFORM_TYPES; i++)
		platforms[i].render(renderer, scrollingOffsetVel, scrollingYOffsetVel);

	unicornObject->render(renderer, scrollingYOffsetVel);
	wishesLeftIconsTextures->render(SCREEN_WIDTH - wishesLeftIconsTextures->getWidth(), 0, renderer);
	renderTime(digitsTexture, currentTime, renderer);

	SDL_RenderPresent(renderer);
}

void handleGameplay(SDL_Event* e, SDL_Renderer* renderer, bool* closeProgram, Unicorn* unicornObject, MyPlatform platforms[PLATFORM_TYPES], 
	Texture* bgTexture, Texture* digitTexture, Texture* wishesLeftIconsTextures) {

	bool quit = false, gameDefaultControlsEnabled = true;
	int scrollingOffsetVel = 0, scrollingYOffsetVel = 0;
	Uint32 gameStartTime = SDL_GetTicks();

	while (!quit) {
		Uint32 currentTime = SDL_GetTicks() - gameStartTime;

		while (SDL_PollEvent(e) != 0) {
			if (gameDefaultControlsEnabled)
				gameDefaultControls(e, unicornObject, &quit, closeProgram, &scrollingOffsetVel, &gameDefaultControlsEnabled);
			else
				projectDefaultControls(e, unicornObject, &quit, closeProgram, &scrollingOffsetVel, &gameDefaultControlsEnabled);
		}

		if (gameDefaultControlsEnabled)
			controlGameSpeedBasedOnTime(currentTime, unicornObject->getIsUnicornDashing(), &scrollingOffsetVel);

		if (checkIfUnicornCrashedIntoPlatform(unicornObject, platforms))
			unicornObject->explode();

		if (checkIfUnicornFellOfPlatforms(platforms))
			unicornObject->fellOver();

		if (unicornObject->getDoesUnicornExploded()) {
			scrollingOffsetVel = 0;
			scrollingYOffsetVel = 0;

			if (unicornObject->hasExplosionEnded())
				quit = true;
		}

		if (unicornObject->hasFallingOverEnded()) 
			quit = true;

		unicornObject->manipulateUnicornOnYAxis(checkIfUnicornLandedOnPlatform(unicornObject, platforms), &scrollingYOffsetVel);
		renderTextures(renderer, bgTexture, digitTexture, unicornObject, platforms, wishesLeftIconsTextures, currentTime, scrollingOffsetVel, scrollingYOffsetVel);
	}
}

void handleEndScreen(SDL_Event* e, SDL_Renderer* renderer, bool* closeProgram, Texture* endScreenTexture, Texture* againSubtitleTexture) {
	bool quit = false;

	while (!quit) {

		while (SDL_PollEvent(e) != 0) {
			if ((*e).type == SDL_QUIT) {
				quit = true;
				*closeProgram = true;
			}
			else if ((*e).type == SDL_KEYDOWN && (*e).key.repeat == 0) {
				switch ((*e).key.keysym.sym) {
				case SDLK_z: quit = true; break;
				case SDLK_ESCAPE: quit = true, *closeProgram = true; break;
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		endScreenTexture->resizeAndRender(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer);
		againSubtitleTexture->render(SCREEN_WIDTH / 2 - againSubtitleTexture->getWidth() / 2, SCREEN_HEIGHT - againSubtitleTexture->getHeight(), renderer);
		SDL_RenderPresent(renderer);
	}

}

void restartGame(Unicorn* unicornObject, MyPlatform platforms[PLATFORM_TYPES]) {
	unicornObject->initUnicornValues();

	for (int i = 0; i < PLATFORM_TYPES; i++)
		platforms[i].restartPlatform();
}


int main()
{
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Event e;
	Texture bgTexture, startingScreenTexture, endScreenTexture, digitsTexture, wishesLeftIconsTextures[UNICORN_WISHES_NUM];
	Texture subtitlesTextures[SUBTITLES_TEXTURES_NUM];
	Unicorn unicornObject;
	MyPlatform platforms[PLATFORM_TYPES];

	for (int i = 0; i < PLATFORM_TYPES; i++)
		platforms[i].initPlatformProperties(i + 1);

	bool closeProgram = false;
	srand(time(0));									


	if (!init(&window, &renderer))
		printf("Failed to initialize\n");
	else {
		if (!loadStartingScreen(&startingScreenTexture, renderer)) {
			printf("Failed to load starting screen\n");
			close(window, renderer);
			return 0;
		}

		if (!loadMedia(&bgTexture, &endScreenTexture, &digitsTexture, &unicornObject, platforms, wishesLeftIconsTextures, subtitlesTextures, renderer)) {
			printf("Failed to load media\n");
			close(window, renderer);
			return 0;
		}
		
		while (!closeProgram) {
			int wishes = UNICORN_WISHES_NUM;

			handleMainMenu(&e, renderer, &startingScreenTexture, &subtitlesTextures[START_SUBTITLE_INDEX], &closeProgram);

			if (closeProgram)
				break;
				
			while (wishes > 0) {
				handleGameplay(&e, renderer, &closeProgram, &unicornObject, platforms, &bgTexture, &digitsTexture, &wishesLeftIconsTextures[wishes - 1]);

				if (closeProgram)
					break;

				restartGame(&unicornObject, platforms);
				handleEndScreen(&e, renderer, &closeProgram, &endScreenTexture, &subtitlesTextures[AGAIN_SUBTITLE_INDEX]);

				if (closeProgram)
					break;

				--wishes;
			}

			if (closeProgram)
				break;
		}

	}
		
	close(window, renderer);

    return 0;
}

