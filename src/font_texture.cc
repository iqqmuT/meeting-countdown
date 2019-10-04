#include "font_texture.h"

#include <stdio.h>

#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_ttf.h>

FontTexture::FontTexture() {
	font_ = NULL;
	renderer_ = NULL;
	texture_ = NULL;
	width_ = 0;
	height_ = 0;
}

FontTexture::~FontTexture() {
	Free();
	CloseFont();
}

void FontTexture::set_renderer(SDL_Renderer* renderer) {
	renderer_ = renderer;
}

bool FontTexture::LoadFont(const char *fontPath, int size) {
	CloseFont();
	font_ = TTF_OpenFont(fontPath, size);
	if (font_ == NULL) {
		std::cerr << "Unable to open font file. SDL_ttf error: " << TTF_GetError() << "\n";
		return false;
	}
	return true;
}

bool FontTexture::LoadFromRenderedText(const char *text, SDL_Color color) {
	// get rid of pre-existing texture
	Free();

	// render text surface
	// SDL_Surface* surface = TTF_RenderText_Solid(mFont, text, color);
	SDL_Surface* surface = TTF_RenderText_Blended(font_, text, color);
	if (surface) {
		// create texture from surface pixels
		texture_ = SDL_CreateTextureFromSurface(renderer_, surface);
		if (texture_) {
			width_ = surface->w;
			height_ = surface->h;
		} else {
			std::cerr << "Unable to create texture from rendered text. SDL Error: " << SDL_GetError() << "\n";
		}

		// get rid of used surface
		SDL_FreeSurface(surface);
	} else {
		std::cerr << "Unable to render text surface. SDL_ttf error: " << TTF_GetError() << "\n";
	}

	return texture_ != NULL;
}

void FontTexture::Free() {
	if (texture_ != NULL) {
		SDL_DestroyTexture(texture_);
		texture_ = NULL;
		width_ = 0;
		height_ = 0;
	}
}

void FontTexture::CloseFont() {
	if (font_ != NULL) {
		TTF_CloseFont(font_);
	}
}

void FontTexture::Render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	// set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, width_, height_ };

	// set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	// render
	SDL_RenderCopyEx(renderer_, texture_, clip, &renderQuad, angle, center, flip);
}
