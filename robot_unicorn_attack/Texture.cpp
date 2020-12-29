#include "Texture.h"
#include <stdio.h>
#include <SDL_image.h>

Texture::Texture() {
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
};

Texture::~Texture() {
	free();
};

bool Texture::loadFromFile(char path[MAX_PATH_LENGTH], SDL_Renderer* renderer) {
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

void Texture::free() {
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
};

void Texture::render(int x, int y, SDL_Renderer* renderer) {
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	SDL_RenderCopy(renderer, mTexture, NULL, &renderQuad);
};

void Texture::resizeAndRender(int x, int y, int width, int height, SDL_Renderer* renderer) {
	SDL_Rect renderQuad = { x, y, width, height };
	SDL_RenderCopy(renderer, mTexture, NULL, &renderQuad);
};

int Texture::getWidth() {
	return mWidth;
};

int Texture::getHeight() {
	return mHeight;
};
