#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <string.h>
#include <stdlib.h>

#define UNICORNE_TEXTURES_NUM 450

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define MAX_PATH_LENGTH 250
#define MAX_UNICORNE_TEXTURE_FILE_LENGTH 5

#define UNICORN_COLLISION_BOX_HEIGHT 145
#define UNICORN_COLLISION_BOX_WIDTH 260
#define UNICORN_COLLISION_BOX_X_OFFSET 60
#define UNICORN_COLLISION_BOX_Y_OFFSET 20

class Texture
{
public:
	Texture() {
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	};
	~Texture() {
		free();
	};

	bool loadFromFile(char path[MAX_PATH_LENGTH], SDL_Renderer* renderer) {
		free();
		SDL_Texture* newTexture = NULL;
		SDL_Surface* loadedSurface = IMG_Load(path);

		if (loadedSurface == NULL)
			printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
		else {
			SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
			newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

			if (newTexture == NULL)
				printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
			else {
				mWidth = loadedSurface->w;
				mHeight = loadedSurface->h;
			}

			SDL_FreeSurface(loadedSurface);
		}

		mTexture = newTexture;
		return mTexture != NULL;
	};

	void free() {
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
		}
	};

	void render(int x, int y, SDL_Renderer* renderer, SDL_Rect* clip) {
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };

		if (clip != NULL) {
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		SDL_RenderCopy(renderer, mTexture, clip, &renderQuad);
	};

	int getWidth() {
		return mWidth;
	};

	int getHeight() {
		return mHeight;
	};

private:
	SDL_Texture* mTexture;

	int mWidth;
	int mHeight;
};

class Horse {
public:
	static const int HORSE_VEL = 10;

	Horse(int posY) {
		mPosX = 0;
		mPosY = posY;
		mVelX = 0;
		mVelY = 0;
		horseGravity = 20;
		mCollider.w = UNICORN_COLLISION_BOX_WIDTH;
		mCollider.h = UNICORN_COLLISION_BOX_HEIGHT;
		shiftCollider();
		timeHorseIsAcceleratingHeight = 0;
		timeHorseIsFalling = 0;
	};

	void renderHorse(SDL_Renderer* renderer, Texture* horseTextureToRender) {
		(*horseTextureToRender).render(mPosX, mPosY, renderer, NULL);
	}

	void handleEvent(SDL_Event& e) {
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
			switch (e.key.keysym.sym) {
				//case SDLK_UP: isJumping = true; break;
				//case SDLK_DOWN: mVelY += HORSE_VEL; break;
			case SDLK_LEFT: mVelX -= HORSE_VEL; break;
			case SDLK_RIGHT: mVelX += HORSE_VEL; break;
			}
		}
		else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
			switch (e.key.keysym.sym) {
				//case SDLK_UP: mVelY += HORSE_VEL; break;
				//case SDLK_DOWN: mVelY -= HORSE_VEL; break;
			case SDLK_LEFT: mVelX += HORSE_VEL; break;
			case SDLK_RIGHT: mVelX -= HORSE_VEL; break;
			}
		}
	};

	void shiftCollider() {
		mCollider.x = mPosX + UNICORN_COLLISION_BOX_X_OFFSET;
		mCollider.y = mPosY + UNICORN_COLLISION_BOX_Y_OFFSET;
	};

	void move(bool horseLandedOnPlatform, bool horseIsJumping) {
		if (!horseLandedOnPlatform && !horseIsJumping) {
			if (timeHorseIsFalling == 0) {
				timeHorseIsFalling = SDL_GetTicks();
				timeHorseIsAcceleratingHeight = 0;
			}
			mPosY += (SDL_GetTicks() - timeHorseIsFalling) / 100;
		}
		else if (horseIsJumping) {
			if (timeHorseIsAcceleratingHeight == 0) {
				timeHorseIsAcceleratingHeight = SDL_GetTicks();
				timeHorseIsFalling = 0;
			}
			mPosY -= (SDL_GetTicks() - timeHorseIsAcceleratingHeight) / 20;
		}
		shiftCollider();
	};

	int getPosX() {
		return mPosX;
	};

	int getPosY() {
		return mPosY;
	};

	SDL_Rect* getCollider() {
		return &mCollider;
	};

private:
	Uint32 timeHorseIsFalling, timeHorseIsAcceleratingHeight;
	int horseGravity;
	int mPosX, mPosY;
	int mVelX, mVelY;
	SDL_Rect mCollider;
};


bool loadMedia(Texture unicorneSprites[UNICORNE_TEXTURES_NUM], Texture* bgTexture, Texture* platformTexture, SDL_Renderer* renderer) {
	bool success = true;

	for (int i = 1; i <= UNICORNE_TEXTURES_NUM; i++) {
		char path[MAX_PATH_LENGTH] = "../images/unicorn_images/";
		char a[MAX_UNICORNE_TEXTURE_FILE_LENGTH];
		_itoa(i, a, 10);
		strncat(path, a, strlen(a));
		strncat(path, ".png", 4);

		if (!unicorneSprites[i - 1].loadFromFile(path, renderer)) {
			printf("Failed to load Foo' texture image!\n");
			success = false;
			break;
		}
	}

	char bgTexturePath[MAX_PATH_LENGTH] = "../images/background.png";
	if (!(*bgTexture).loadFromFile(bgTexturePath, renderer)) {
		printf("Failed to load background' texture image!\n");
		success = false;
	};

	char platformTexturePath[MAX_PATH_LENGTH] = "../images/platform.png";
	if (!(*platformTexture).loadFromFile(platformTexturePath, renderer)) {
		printf("Failed to load platform' texture image!\n");
		success = false;
	};

	return success;
}

bool init(SDL_Window** window, SDL_Renderer** renderer) {
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		*window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

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

bool checkIfHorseLandedOnPlatform(SDL_Rect* horseColliser, SDL_Rect* platformColliser) {
	int bottomHorseCollisier, topPlatformCollisier;
	bottomHorseCollisier = (*horseColliser).y + (*horseColliser).h;
	topPlatformCollisier = (*platformColliser).y;

	if (bottomHorseCollisier >= topPlatformCollisier)
		return true;

	return false;
}

int main()
{
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Event e;
	Texture unicorneSprites[UNICORNE_TEXTURES_NUM], bgTexture, platformTexture;
	bool quit = false;

	bool isHorseJumping = false;
	int scrollingOffset = 0;
	int scrollingOffsetPlatform = 0;
	int scrollingOffsetVel = 0;
	Uint32 timeHorseStartedJumping = 0;

	if (!init(&window, &renderer))
		printf("Failed to initialize\n");
	else if (!loadMedia(unicorneSprites, &bgTexture, &platformTexture, renderer))
		printf("Failed to load media\n");
	else {
		Horse horseObject(0);
		//Horse horseObject(SCREEN_HEIGHT - platformTexture.getHeight() - unicorneSprites[0].getHeight() + 30);
		SDL_Rect platformCollider = { 0, 0, 2 * platformTexture.getWidth(), platformTexture.getHeight() };

		while (!quit) {
			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT)
					quit = true;
				else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
					switch (e.key.keysym.sym) {
					case SDLK_RIGHT: scrollingOffsetVel -= 10; break;
					case SDLK_UP: isHorseJumping = true; break;
					}
				}
				else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
					switch (e.key.keysym.sym) {
					case SDLK_RIGHT: scrollingOffsetVel += 10; break;
					}
				}
				//horseObject.handleEvent(e);
			}

			if (isHorseJumping && timeHorseStartedJumping == 0) {
				timeHorseStartedJumping = SDL_GetTicks();
			}

			if (SDL_GetTicks() - timeHorseStartedJumping > 300 && isHorseJumping) {
				timeHorseStartedJumping = 0;
				isHorseJumping = false;
			}


			scrollingOffset += scrollingOffsetVel;
			if (scrollingOffset < -bgTexture.getWidth())
				scrollingOffset = 0;

			scrollingOffsetPlatform += scrollingOffsetVel;
			if (scrollingOffsetPlatform < -platformTexture.getWidth())
				scrollingOffsetPlatform = 0;

			platformCollider.x = scrollingOffsetPlatform;
			platformCollider.y = SCREEN_HEIGHT - 100;
			horseObject.move(checkIfHorseLandedOnPlatform(horseObject.getCollider(), &platformCollider), isHorseJumping);
			//camera.x = (horseObject.getPosX() + unicorneSprites[0].getWidth() / 2) - SCREEN_WIDTH / 2;
			//camera.y = (horseObject.getPosY() + unicorneSprites[0].getHeight() / 2) - SCREEN_HEIGHT / 2;

			/*if (camera.x < 0)
				camera.x = 0;
			if (camera.y < 0)
				camera.y = 0;
			if (camera.x > LVL_WIDTH - camera.w)
				camera.x = LVL_WIDTH - camera.w;
			if (camera.y > LVL_HEIGHT - camera.h)
				camera.y = LVL_HEIGHT - camera.h;*/

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			bgTexture.render(scrollingOffset, 0, renderer, NULL);
			bgTexture.render(scrollingOffset + bgTexture.getWidth(), 0, renderer, NULL);

			platformTexture.render(scrollingOffsetPlatform, SCREEN_HEIGHT - 100, renderer, NULL);
			platformTexture.render(scrollingOffsetPlatform + platformTexture.getWidth(), SCREEN_HEIGHT - 100, renderer, NULL);
			/*platformTexture.render(scrollingOffsetPlatform, SCREEN_HEIGHT - platformTexture.getHeight(), renderer, NULL);
			platformTexture.render(scrollingOffsetPlatform + platformTexture.getWidth(), SCREEN_HEIGHT - platformTexture.getHeight(), renderer, NULL);*/

			int frameToDraw = (SDL_GetTicks() / 2) % UNICORNE_TEXTURES_NUM;
			horseObject.renderHorse(renderer, &unicorneSprites[frameToDraw]);


			SDL_RenderPresent(renderer);
		}
	}

    return 0;
}

