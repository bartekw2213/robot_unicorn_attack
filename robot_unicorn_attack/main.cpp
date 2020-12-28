#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define UNICORN_TEXTURES_NUM 450						// ilosc tekstur skladajacych sie na animacje biegnacego konia
#define UNICORN_DASHING_TEXTURES_NUM 16					// ilosc tekstur skladajacych sie na anmiacje dashujacego jednorozca
#define UNICORN_TEXTURES_DASH_TO_RUN_SIZE_RATIO 1.5		// ile razy wieksza powinna byc tekstura dashujacego jednorozca od tego biegnacego
#define UNICORN_WIDTH 200								// szerokosc tekstury jednorozca
#define UNICORN_HEIGHT 106								// wysokosc tekstury jednorozca
#define UNICORN_START_SPEED 10							// poczatkowa predkosc jednorozca
#define UNICORN_MAX_SPEED 20							// maksymalna predkosc do jakiej rozpedza sie jednorozec
#define UNICORN_DASH_SPEED 40							// predkosc jaka osiaga jednorozec podczas wykonywania zrywu
#define UNICORN_DASH_TIME 500							// czas trwania zrywu jednorozca
#define UNICORN_TIME_BETWEEN_DASHES 2000				// czas jaki musi minac by gracz ponownie mogl wykonac zryw
#define UNICORN_ACCELERATION_TEMPO 5					// im większa stała tym wolniej gra będzie przyśpieszać
#define UNICORN_NORMAL_JUMP_POWER 15					// moc normalnego skoku jednorozca
#define UNICORN_EXTENDED_JUMP_POWER 500					// moc wydluzonego skoku jednorozca
#define UNICORN_MAX_LONG_JUMP_TIME 500					// maksymalny czas trwania dlugiego skoku jednorozca

#define SCREEN_WIDTH 640								// szerokosc ekranu gry
#define SCREEN_HEIGHT 480								// wysokosc ekranu gry
#define GRAVITATION_FACTOR 25							// wspolczynnik grawitacji - im bedzie mniejszy tym grawitacja bedzie silniejsza
#define JUMP_TIME_OFFSET 50								// czas po ktorym jest nasluchiwane czy jednorozec dotknal ziemi po wyskoczeniu

#define PLATFORM_Y SCREEN_HEIGHT - 100					// wysokosc na jakiej ustawiana jest platforma
#define OBSTACLE_COLLISION_Y_OFFSET 10					// granica bledu w jakiej przeszkoda jest czula na kolizje z jednorozcem

#define MAX_PATH_LENGTH 250								// maksymalna dlugosc sciezki pliku
#define MAX_UNICORNE_TEXTURE_FILE_LENGTH 5				// maksymalna dlugosc nazwy pliku z tekstura konia


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

	void resizeAndRender(int x, int y, int width, int height, SDL_Renderer* renderer) {
		SDL_Rect renderQuad = { x, y, width, height };
		SDL_RenderCopy(renderer, mTexture, NULL, &renderQuad);
	}

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
		mHowManyTimesHorseJumped = 0;
		mTimeWhenHorseJumped = 0;
		mTimeWhenFreeFallStarted = 0;
		mTimeWhenHorseDashed = 0;
		mIsHorseDashing = false;
		mIsPlayerHoldingJumpKey = false;
		mIsHorseFreeFallingAfterDash = false;
		mCollider.w = UNICORN_WIDTH / 2;
		mCollider.h = UNICORN_HEIGHT * 3 / 4;
		shiftCollider();
	};

	~Horse() {
		for (int i = 0; i < UNICORN_TEXTURES_NUM; i++)
			mUnicornRunningTextures[i].free();
	}

	void render(SDL_Renderer* renderer) {
		if (mIsHorseDashing) {
			int frameToDraw = (SDL_GetTicks() - mTimeWhenHorseDashed) / (float)UNICORN_DASH_TIME * (UNICORN_DASHING_TEXTURES_NUM - 1);
			mUnicornDashingTextures[frameToDraw].resizeAndRender(mPosX, mPosY, UNICORN_WIDTH * UNICORN_TEXTURES_DASH_TO_RUN_SIZE_RATIO, UNICORN_HEIGHT * UNICORN_TEXTURES_DASH_TO_RUN_SIZE_RATIO, renderer);
		}
		else {
			int frameToDraw = SDL_GetTicks() % UNICORN_TEXTURES_NUM; 
			mUnicornRunningTextures[frameToDraw].resizeAndRender(mPosX, mPosY, UNICORN_WIDTH, UNICORN_HEIGHT, renderer);
		}
	}

	void shiftCollider() {
		mCollider.x = mPosX + UNICORN_WIDTH / 3;
		mCollider.y = mPosY + UNICORN_HEIGHT / 5;

		if(mIsHorseDashing) 
			mCollider.x = mPosX + UNICORN_WIDTH / 1.5;
	};

	bool loadTextures(SDL_Renderer* renderer) {
		if(!mLoadHorseAnimationFrames(UNICORN_TEXTURES_NUM, "../images/unicorn_run/", mUnicornRunningTextures, renderer))
			return false;

		if (!mLoadHorseAnimationFrames(UNICORN_DASHING_TEXTURES_NUM, "../images/unicorn_dash/", mUnicornDashingTextures, renderer))
			return false;

		return true;
	}

	void manipulateHorseOnYAxis(bool horseLandedOnPlatform, bool horseLandedOnObstacle) {
		// jednorozec pozostaje nieruchomy na osi Y podczas gdy wykonuje zryw
		if (mIsHorseDashing) {
			mPosY -= 0;

			// wylacza zryw jednorozca po okreslonym czasie
			if (SDL_GetTicks() - mTimeWhenHorseDashed > UNICORN_DASH_TIME) {
				mIsHorseDashing = false;
				mIsHorseFreeFallingAfterDash = true;
				mTimeWhenFreeFallStarted = SDL_GetTicks();
			}
		}
		// obsluga skoku jednorozca
		else if (mHowManyTimesHorseJumped != 0 && !mIsHorseFreeFallingAfterDash) {
			Uint32 jumpTime = SDL_GetTicks() - mTimeWhenHorseJumped;

			if ((horseLandedOnPlatform || horseLandedOnObstacle) && jumpTime > JUMP_TIME_OFFSET)
				mHowManyTimesHorseJumped = 0;
			else if (mIsPlayerHoldingJumpKey && jumpTime <= UNICORN_MAX_LONG_JUMP_TIME && jumpTime > JUMP_TIME_OFFSET)
				mPosY -= (UNICORN_EXTENDED_JUMP_POWER - jumpTime) / GRAVITATION_FACTOR;
			else if ((UNICORN_NORMAL_JUMP_POWER - jumpTime / GRAVITATION_FACTOR) > 0)
				mPosY -= (UNICORN_NORMAL_JUMP_POWER - jumpTime / GRAVITATION_FACTOR);
			else 
				mTimeWhenFreeFallStarted = SDL_GetTicks();
		}
		// obsluga swobodnego spadania jednorozca - np. gdy spada z platformy lub przeszkody
		else if (!horseLandedOnPlatform && !horseLandedOnObstacle) {
			if (mTimeWhenFreeFallStarted == 0) 
				mTimeWhenFreeFallStarted = SDL_GetTicks();

			Uint32 freeFallTime = SDL_GetTicks() - mTimeWhenFreeFallStarted;
			mPosY += freeFallTime/GRAVITATION_FACTOR;
		}
		else {
			mTimeWhenFreeFallStarted = 0;
			mIsHorseFreeFallingAfterDash = false;
		}

		shiftCollider();
	};

	void jump(bool isPlayerHoldingJumpKey) {
		mIsPlayerHoldingJumpKey = isPlayerHoldingJumpKey;
		if (mHowManyTimesHorseJumped < 2 && isPlayerHoldingJumpKey && !mIsHorseDashing) {
			++mHowManyTimesHorseJumped;
			mTimeWhenHorseJumped = SDL_GetTicks();
			mIsHorseFreeFallingAfterDash = false;
		}
	}

	void dash(bool isPlayerHoldingDash) {
		// gdy gracz przestanie trzymacz przycisk X przestan wykonywac zryw
		if (!isPlayerHoldingDash) {		
			mIsHorseDashing = false;
			return;
		}

		// pozwol na wykonanie zrywu jesli poprzedni byl wystarczajaco czasu temu
		if (SDL_GetTicks() - mTimeWhenHorseDashed > UNICORN_TIME_BETWEEN_DASHES && isPlayerHoldingDash) { 
			mIsHorseDashing = true;
			mTimeWhenHorseDashed = SDL_GetTicks();
			if (mHowManyTimesHorseJumped == 2)
				--mHowManyTimesHorseJumped;
		}
	}

	bool getIsHorseDashing() {
		return mIsHorseDashing;
	}

	SDL_Rect* getCollider() {
		return &mCollider;
	};

private:
	bool mIsPlayerHoldingJumpKey, mIsHorseDashing, mIsHorseFreeFallingAfterDash;
	int mHowManyTimesHorseJumped;
	int mPosX, mPosY;
	SDL_Rect mCollider;
	Uint32 mTimeWhenHorseJumped, mTimeWhenFreeFallStarted, mTimeWhenHorseDashed;
	Texture mUnicornRunningTextures[UNICORN_TEXTURES_NUM], mUnicornDashingTextures[UNICORN_DASHING_TEXTURES_NUM];

	bool mLoadHorseAnimationFrames(int numOfFrames, char const* pathToFolderWithFrames, Texture* arrayOfFrames, SDL_Renderer* renderer) {

		for (int i = 1; i <= numOfFrames; i++) {
			char path[MAX_PATH_LENGTH] = "";
			strncat(path, pathToFolderWithFrames, strlen(pathToFolderWithFrames));

			// dodanie do sciezki nazwe wczytywanego pliku (nazwa zdjec danej animacji jednorozca to liczby 1.png, 2.png, 3.png, ...)
			char a[MAX_UNICORNE_TEXTURE_FILE_LENGTH];
			_itoa(i, a, 10);
			strncat(path, a, strlen(a));
			strncat(path, ".png", strlen(".png"));

			if (!arrayOfFrames[i - 1].loadFromFile(path, renderer)) {
				printf("Failed to load unicorn animation images!\n");
				return false;
			}
		}

		return true;
	}
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
			printf("Failed to load platform texture image!\n");
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

void projectDefaultControls(SDL_Event* e, Horse* horseObject, bool* quit, int* scrollingOffsetVel, bool* gameDefaultControlsEnabled) {
	if ((*e).type == SDL_QUIT)
		*quit = true;
	else if ((*e).type == SDL_KEYDOWN && (*e).key.repeat == 0) {
		switch ((*e).key.keysym.sym) {
		case SDLK_RIGHT: *scrollingOffsetVel -= UNICORN_START_SPEED; break;
		case SDLK_UP: (*horseObject).jump(true); break;
		case SDLK_d: *gameDefaultControlsEnabled = true; break;
		case SDLK_ESCAPE: *quit = true; break;
		}
	}
	else if ((*e).type == SDL_KEYUP && (*e).key.repeat == 0) {
		switch ((*e).key.keysym.sym) {
		case SDLK_RIGHT: *scrollingOffsetVel += UNICORN_START_SPEED; break;
		case SDLK_UP: (*horseObject).jump(false); break;
		}
	}
}

void gameDefaultControls(SDL_Event* e, Horse* horseObject, bool* quit, int* scrollingOffsetVel, bool* gameDefaultControlsEnabled) {
	if ((*e).type == SDL_QUIT)
		*quit = true;
	else if ((*e).type == SDL_KEYDOWN && (*e).key.repeat == 0) {
		switch ((*e).key.keysym.sym) {
		case SDLK_z: (*horseObject).jump(true); break;
		case SDLK_x: (*horseObject).dash(true); break;
		case SDLK_d: *gameDefaultControlsEnabled = false; *scrollingOffsetVel = 0; break;
		case SDLK_ESCAPE: *quit = true; break;
		}
	}
	else if ((*e).type == SDL_KEYUP && (*e).key.repeat == 0) {
		switch ((*e).key.keysym.sym) {
		case SDLK_z: (*horseObject).jump(false); break;
		case SDLK_x: (*horseObject).dash(false); break;
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
	bool gameDefaultControlsEnabled = false;
	int scrollingOffsetVel = 0;

	if (!init(&window, &renderer))
		printf("Failed to initialize\n");
	else if (!loadMedia(&bgTexture, &horseObject, &obstacleObject, &platformObject, renderer))
		printf("Failed to load media\n");
	else {
		Uint32 gameStartTime = SDL_GetTicks();

		while (!quit) {
			Uint32 currentTime = SDL_GetTicks() - gameStartTime;

			while (SDL_PollEvent(&e) != 0) {
				if(gameDefaultControlsEnabled)
					gameDefaultControls(&e, &horseObject, &quit, &scrollingOffsetVel, &gameDefaultControlsEnabled);
				else
					projectDefaultControls(&e, &horseObject, &quit, &scrollingOffsetVel, &gameDefaultControlsEnabled);
			}

			if (gameDefaultControlsEnabled)
				controlGameSpeedBasedOnTime(currentTime, horseObject.getIsHorseDashing(), &scrollingOffsetVel);

			horseObject.manipulateHorseOnYAxis(platformObject.checkIfHorseLandedOnPlatform(horseObject.getCollider()), 
				obstacleObject.checkIfHorseLandedOnObstacle(horseObject.getCollider()));

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			bgTexture.resizeAndRender(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer);
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

