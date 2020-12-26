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
#define UNICORN_COLLISION_BOX_WIDTH 230
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
		mIsJumping = false;
		mPosX = 0;
		mPosY = posY;
		mVelX = 0;
		mVelY = 0;
		horseGravity = 20;
		mCollider.w = UNICORN_COLLISION_BOX_WIDTH;
		mCollider.h = UNICORN_COLLISION_BOX_HEIGHT;
		shiftCollider();
		mTimeWhenHorseJumped = 0;
	};

	void renderHorse(SDL_Renderer* renderer, Texture* horseTextureToRender) {
		(*horseTextureToRender).render(mPosX, mPosY, renderer, NULL);
	}

	void handleEvent(SDL_Event& e) {
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
			switch (e.key.keysym.sym) {
				//case SDLK_UP: mIsJumping = true; break;
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

	void move(bool horseLandedOnPlatform, bool horseLandedOnObstacle) {
		if (mIsJumping) {
			Uint32 jumpTime = SDL_GetTicks() - mTimeWhenHorseJumped;
			mPosY -= (10 - jumpTime/50);
			if ((horseLandedOnPlatform || horseLandedOnObstacle) && jumpTime > 500) {
				mIsJumping = false;
				jumpTime = 0;
			}
		}
		else if(!horseLandedOnPlatform && !horseLandedOnObstacle)
			mPosY += 10;
		shiftCollider();
	};

	void jump() {
		if (!mIsJumping) {
			mIsJumping = true;
			mTimeWhenHorseJumped = SDL_GetTicks();
		}
	}

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
	bool mIsJumping;
	int horseGravity;
	int mPosX, mPosY;
	int mVelX, mVelY;
	SDL_Rect mCollider;
	Uint32 mTimeWhenHorseJumped;
};

class Obstacle {
public:
	Obstacle(SDL_Renderer* renderer) {
		char obstacleTexturePath[MAX_PATH_LENGTH] = "../images/obstacle2.png";
		if (!obstacleTexture.loadFromFile(obstacleTexturePath, renderer)) 
			printf("Failed to load obstacle texture image!\n");
		mPosX = SCREEN_WIDTH + 100;
		mPosY = SCREEN_HEIGHT - 100 - obstacleTexture.getHeight();
	}

	~Obstacle() {
		obstacleTexture.free();
	}

	void render(SDL_Renderer* renderer, int scrollingOffsetVel) {
		mPosX += scrollingOffsetVel;
		obstacleTexture.render(mPosX, SCREEN_HEIGHT - 100 - obstacleTexture.getHeight(), renderer, NULL);
		if (mPosX < -obstacleTexture.getWidth())
			mPosX = SCREEN_WIDTH + 100;
	}

	bool horseRanIntoObstacle(SDL_Rect* horseCollider) {
		if ((horseCollider->x + horseCollider->w) >= mPosX &&
			mPosX >= horseCollider->x &&
			(horseCollider->y + horseCollider->h) > mPosY + 10) // zalicza granice bledu tego ze kon moze byc lekko nizej
			return true;
		return false;
	}

	bool checkIfHorseLandedOnObstacle(SDL_Rect* horseCollider) {
		if ((horseCollider->x + horseCollider->w) >= mPosX &&
			mPosX + obstacleTexture.getWidth() >= horseCollider->x &&
			(horseCollider->y + horseCollider->h) <= mPosY + 10 && (horseCollider->y + horseCollider->h) >= mPosY -10) 
			return true;

		return false;
	}

private:
	int mPosX, mPosY;
	Texture obstacleTexture;
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

	int scrollingOffset = 0;
	int scrollingOffsetPlatform = 0;
	int scrollingOffsetVel = 0;

	if (!init(&window, &renderer))
		printf("Failed to initialize\n");
	else if (!loadMedia(unicorneSprites, &bgTexture, &platformTexture, renderer))
		printf("Failed to load media\n");
	else {
		Horse horseObject(0);
		Obstacle obstacleObject(renderer);
		SDL_Rect platformCollider = { 0, 0, 2 * platformTexture.getWidth(), platformTexture.getHeight() };

		while (!quit) {
			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT)
					quit = true;
				else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
					switch (e.key.keysym.sym) {
					case SDLK_RIGHT: scrollingOffsetVel -= 10; break;
					case SDLK_UP: horseObject.jump(); break;
					}
				}
				else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
					switch (e.key.keysym.sym) {
					case SDLK_RIGHT: scrollingOffsetVel += 10; break;
					}
				}
				//horseObject.handleEvent(e);
			}


			scrollingOffset += scrollingOffsetVel;
			if (scrollingOffset < -bgTexture.getWidth())
				scrollingOffset = 0;

			scrollingOffsetPlatform += scrollingOffsetVel;
			if (scrollingOffsetPlatform < -platformTexture.getWidth())
				scrollingOffsetPlatform = 0;

			platformCollider.x = scrollingOffsetPlatform;
			platformCollider.y = SCREEN_HEIGHT - 100;
			horseObject.move(checkIfHorseLandedOnPlatform(horseObject.getCollider(), &platformCollider), obstacleObject.checkIfHorseLandedOnObstacle(horseObject.getCollider()));

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			bgTexture.render(scrollingOffset, 0, renderer, NULL);
			bgTexture.render(scrollingOffset + bgTexture.getWidth(), 0, renderer, NULL);

			platformTexture.render(scrollingOffsetPlatform, SCREEN_HEIGHT - 100, renderer, NULL);
			platformTexture.render(scrollingOffsetPlatform + platformTexture.getWidth(), SCREEN_HEIGHT - 100, renderer, NULL);

			obstacleObject.render(renderer, scrollingOffsetVel);

			int frameToDraw = (SDL_GetTicks() / 2) % UNICORNE_TEXTURES_NUM;
			horseObject.renderHorse(renderer, &unicorneSprites[frameToDraw]);

			if(obstacleObject.horseRanIntoObstacle(horseObject.getCollider())) 
				printf("Kolizja");


			SDL_RenderPresent(renderer);
		}
	}

    return 0;
}

