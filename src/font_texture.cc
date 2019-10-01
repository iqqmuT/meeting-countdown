#include "font_texture.h"

#include <stdio.h>

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_ttf.h>

FontTexture::FontTexture() {
	mFont = NULL;
	mRenderer = NULL;
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

FontTexture::~FontTexture() {
	free();
	closeFont();
}

void FontTexture::setRenderer(SDL_Renderer* renderer) {
	mRenderer = renderer;
}

bool FontTexture::loadFont(const char *fontPath, int size) {
	closeFont();
	mFont = TTF_OpenFont(fontPath, size);
	if (mFont == NULL) {
		printf("Unable to open font. SDL_ttf error: %s\n", TTF_GetError());
	}
}

bool FontTexture::loadFromRenderedText(const char *text, SDL_Color color) {
	// get rid of pre-existing texture
	free();

	// render text surface
	// SDL_Surface* surface = TTF_RenderText_Solid(mFont, text, color);
	SDL_Surface* surface = TTF_RenderText_Blended(mFont, text, color);
	if (surface) {
		// create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
		if (mTexture) {
			mWidth = surface->w;
			mHeight = surface->h;
		} else {
			printf("Unable to create texture from rendered text. SDL Error: %s\n", SDL_GetError());
		}

		// get rid of used surface
		SDL_FreeSurface(surface);
	} else {
		printf("Unable to render text surface. SDL_ttf error: %s\n", TTF_GetError());
	}

	return mTexture != NULL;
}

void FontTexture::free() {
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void FontTexture::closeFont() {
	if (mFont != NULL) {
		TTF_CloseFont(mFont);
	}
}

void FontTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	// set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	// set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	// render
	SDL_RenderCopyEx(mRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int FontTexture::getWidth() {
	return mWidth;
}

int FontTexture::getHeight() {
	return mHeight;
}
