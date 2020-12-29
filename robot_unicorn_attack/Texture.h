#pragma once
#include <SDL.h>
#include "constants.h";

class Texture
{
public:
	Texture();
	~Texture();

	bool loadFromFile(char path[MAX_PATH_LENGTH], SDL_Renderer* renderer);
	void free();
	void render(int x, int y, SDL_Renderer* renderer);
	void resizeAndRender(int x, int y, int width, int height, SDL_Renderer* renderer);
	int getWidth();
	int getHeight();

private:
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
};

