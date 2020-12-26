#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define UNICORN_TEXTURES_NUM 450

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define PLATFORM_Y 380
#define OBSTACLE_COLLISION_Y_OFFSET 10

#define MAX_PATH_LENGTH 250
#define MAX_UNICORNE_TEXTURE_FILE_LENGTH 5

#define UNICORN_COLLISION_BOX_HEIGHT 145
#define UNICORN_COLLISION_BOX_WIDTH 180
#define UNICORN_COLLISION_BOX_X_OFFSET 100
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

	void render(int x, int y, SDL_Renderer* renderer) {
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };
		SDL_RenderCopy(renderer, mTexture, NULL, &renderQuad);
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
	Horse() {
		mPosX = 0;
		mPosY = 0;
		mIsJumping = false;
		mTimeWhenHorseJumped = 0;
		mTimeWhenFreeFallStarted = 0;
		mCollider.w = UNICORN_COLLISION_BOX_WIDTH;
		mCollider.h = UNICORN_COLLISION_BOX_HEIGHT;
		shiftCollider();
	};

	~Horse() {
		for (int i = 0; i < UNICORN_TEXTURES_NUM; i++)
			mUnicornRunningTextures[i].free();
	}

	void render(SDL_Renderer* renderer) {
		int frameToDraw = SDL_GetTicks() % UNICORN_TEXTURES_NUM;
		mUnicornRunningTextures[frameToDraw].render(mPosX, mPosY, renderer);
	}

	void shiftCollider() {
		mCollider.x = mPosX + UNICORN_COLLISION_BOX_X_OFFSET;
		mCollider.y = mPosY + UNICORN_COLLISION_BOX_Y_OFFSET;
	};

	bool loadTextures(SDL_Renderer* renderer) {
		for (int i = 1; i <= UNICORN_TEXTURES_NUM; i++) {
			char path[MAX_PATH_LENGTH] = "../images/unicorn_images/";
			char a[MAX_UNICORNE_TEXTURE_FILE_LENGTH];
			_itoa(i, a, 10);
			strncat(path, a, strlen(a));
			strncat(path, ".png", strlen(".png"));

			if (!mUnicornRunningTextures[i - 1].loadFromFile(path, renderer)) {
				printf("Failed to load Foo' texture image!\n");
				return false;
			}
		}

		return true;
	}

	void manipulateHorseOnYAxis(bool horseLandedOnPlatform, bool horseLandedOnObstacle) {
		if (mIsJumping) {
			Uint32 jumpTime = SDL_GetTicks() - mTimeWhenHorseJumped;
			mPosY -= (10 - jumpTime/50);
			if ((horseLandedOnPlatform || horseLandedOnObstacle) && jumpTime > 50)
				mIsJumping = false;
		}
		else if (!horseLandedOnPlatform && !horseLandedOnObstacle) {
			if (mTimeWhenFreeFallStarted == 0)
				mTimeWhenFreeFallStarted = SDL_GetTicks();
			Uint32 freeFallTime = SDL_GetTicks() - mTimeWhenFreeFallStarted;
			mPosY += freeFallTime/50;
		}
		else
			mTimeWhenFreeFallStarted = 0;

		shiftCollider();
	};

	void jump() {
		if (!mIsJumping) {
			mIsJumping = true;
			mTimeWhenHorseJumped = SDL_GetTicks();
		}
	}

	SDL_Rect* getCollider() {
		return &mCollider;
	};

private:
	bool mIsJumping;
	int mPosX, mPosY;
	SDL_Rect mCollider;
	Uint32 mTimeWhenHorseJumped, mTimeWhenFreeFallStarted;
	Texture mUnicornRunningTextures[UNICORN_TEXTURES_NUM];
};

class Obstacle {
public:
	Obstacle() {
		srand(time(0));									//	przy pomocy rand() bede generowac losowo gdzie ma pojawic sie przeszkoda
		mPosX = SCREEN_WIDTH + (rand() % SCREEN_WIDTH);
	}

	~Obstacle() {
		obstacleTexture.free();
	}

	bool loadTexture(SDL_Renderer* renderer) {
		char obstacleTexturePath[MAX_PATH_LENGTH] = "../images/obstacle2.png";
		if (!obstacleTexture.loadFromFile(obstacleTexturePath, renderer)) {
			printf("Failed to load obstacle texture image!\n");
			return false;
		}
		else
			mPosY = PLATFORM_Y - obstacleTexture.getHeight();

		return true;
	}

	void render(SDL_Renderer* renderer, int scrollingOffsetVel) {
		mPosX += scrollingOffsetVel;
		obstacleTexture.render(mPosX, mPosY, renderer);

		if (mPosX < -obstacleTexture.getWidth())
			mPosX = (SCREEN_WIDTH * (rand() % 2 + 1)) + (rand() % SCREEN_WIDTH);
	}

	bool horseRanIntoObstacle(SDL_Rect* horseCollider) {
		if ((horseCollider->x + horseCollider->w) >= mPosX &&
			mPosX >= horseCollider->x &&
			(horseCollider->y + horseCollider->h) > mPosY + OBSTACLE_COLLISION_Y_OFFSET) // zalicza granice bledu tego ze kon moze byc lekko nizej
			return true;
		return false;
	}

	bool checkIfHorseLandedOnObstacle(SDL_Rect* horseCollider) {
		if ((horseCollider->x + horseCollider->w) >= mPosX &&
			mPosX + obstacleTexture.getWidth() >= horseCollider->x &&
			(horseCollider->y + horseCollider->h) <= mPosY + OBSTACLE_COLLISION_Y_OFFSET && (horseCollider->y + horseCollider->h) >= mPosY - OBSTACLE_COLLISION_Y_OFFSET) {
			return true;
		}

		return false;
	}

private:
	int mPosX, mPosY;
	Texture obstacleTexture;
};

class Platform {
public: 
	Platform() {
		mPosX = 0;
		mPosY = PLATFORM_Y;
	}

	~Platform() {
		mPlatformTexture.free();
	}

	bool loadTexture(SDL_Renderer* renderer) {
		char platformTexturePath[MAX_PATH_LENGTH] = "../images/platform.png";
		if (!(mPlatformTexture).loadFromFile(platformTexturePath, renderer)) {
			printf("Failed to load platform' texture image!\n");
			return false;
		};
		
		return true;
	}

	void render(SDL_Renderer* renderer, int scrollingOffsetVel) {
		mPosX += scrollingOffsetVel;
		mPlatformTexture.render(mPosX, mPosY, renderer);
		mPlatformTexture.render(mPosX + mPlatformTexture.getWidth(), mPosY, renderer);
		if (mPosX < -mPlatformTexture.getWidth())
			mPosX = 0;
	}

	bool checkIfHorseLandedOnPlatform(SDL_Rect* horseCollider) {
		int bottomHorseCollider;
		bottomHorseCollider = (*horseCollider).y + (*horseCollider).h;

		if (bottomHorseCollider >= mPosY)
			return true;

		return false;
	}

private:
	int mPosX, mPosY;
	Texture mPlatformTexture;
};


bool loadMedia(Texture* bgTexture, Horse* horseObject, Obstacle* obstacleObject, Platform* platformObject, SDL_Renderer* renderer) {
	bool success = true;

	char bgTexturePath[MAX_PATH_LENGTH] = "../images/background.png";
	if (!(*bgTexture).loadFromFile(bgTexturePath, renderer)) {
		printf("Failed to load background' texture image!\n");
		success = false;
	};

	if (!obstacleObject->loadTexture(renderer) || !platformObject->loadTexture(renderer) || !horseObject->loadTextures(renderer))
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

bool checkIfHorseLandedOnPlatform(SDL_Rect* horseColliser, SDL_Rect* platformColliser) {
	int bottomHorseCollisier, topPlatformCollisier;
	bottomHorseCollisier = (*horseColliser).y + (*horseColliser).h;
	topPlatformCollisier = (*platformColliser).y;

	if (bottomHorseCollisier >= topPlatformCollisier)
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
	Texture unicorneSprites[UNICORN_TEXTURES_NUM], bgTexture;
	Horse horseObject;
	Obstacle obstacleObject;
	Platform platformObject;
	bool quit = false;
	int scrollingOffsetVel = 0;

	if (!init(&window, &renderer))
		printf("Failed to initialize\n");
	else if (!loadMedia(&bgTexture, &horseObject, &obstacleObject, &platformObject, renderer))
		printf("Failed to load media\n");
	else {
		while (!quit) {
			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT)
					quit = true;
				else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
					switch (e.key.keysym.sym) {
					case SDLK_RIGHT: scrollingOffsetVel -= 15; break;
					case SDLK_UP: horseObject.jump(); break;
					}
				}
				else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
					switch (e.key.keysym.sym) {
					case SDLK_RIGHT: scrollingOffsetVel += 15; break;
					}
				}
			}

			horseObject.manipulateHorseOnYAxis(platformObject.checkIfHorseLandedOnPlatform(horseObject.getCollider()), obstacleObject.checkIfHorseLandedOnObstacle(horseObject.getCollider()));

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			bgTexture.render(0, 0, renderer);
			platformObject.render(renderer, scrollingOffsetVel);
			obstacleObject.render(renderer, scrollingOffsetVel);
			horseObject.render(renderer);

			if(obstacleObject.horseRanIntoObstacle(horseObject.getCollider())) 
				printf("Kolizja");

			SDL_RenderPresent(renderer);
		}

		close(window, renderer);
	}

    return 0;
}

