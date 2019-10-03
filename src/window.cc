#include "window.h"

#include <stdio.h>
#include <math.h>

#include <ctime>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_ttf.h>

#include "AHEasing/easing.h"
#include "SDL2_gfx/SDL2_gfxPrimitives.h"

Window::Window() {
	mWindow = NULL;
	mRenderer = NULL;
	SDL_VERSION(&mInfo.version);

	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mShown = false;
	mWindowID = -1;
	mWindowDisplayID = -1;

	mWidth = 0;
	mHeight = 0;

  // start counting after these ticks
	mStartCountingTicks = 1000;
}

SDL_Color textColor = { 0xFF, 0xFF, 0xFF };

bool Window::init(Config* config) {
	mConfig = config;

	bg_color_ = config->getBackgroundColor();
	int width = config->getWidth();
	int height = config->getHeight();

	// init ticks
	mStartTicks = SDL_GetTicks();
	time_t diff = config->getEndTime() - time(NULL);
	mEndTicks = mStartTicks + (diff * 1000);

	mWindow = SDL_CreateWindow("Countdown", SDL_WINDOWPOS_UNDEFINED,
	                           SDL_WINDOWPOS_UNDEFINED, width, height,
														 SDL_WINDOW_SHOWN);

	if (mWindow != NULL) {
		if (SDL_GetWindowWMInfo(mWindow, &mInfo)) {
			mMouseFocus = true;
			mKeyboardFocus = true;

			// create renderer for window
			mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (mRenderer != NULL) {
				// initialize renderer color
				SDL_SetRenderDrawColor(mRenderer, bg_color_.r, bg_color_.g, bg_color_.b, 0xFF);
				SDL_RenderClear(mRenderer);

				// grab window identifiers
				mWindowID = SDL_GetWindowID(mWindow);
				mWindowDisplayID = SDL_GetWindowDisplayIndex(mWindow);

				// flag as opened
				mShown = true;

        // initialize mirror functionality
				mirror_.Init(config, &mInfo);

				// initialize circle component
				circle_.Init(config, mRenderer, mEndTicks);

				// initialize counter component
				counter_.Init(config, mRenderer, mEndTicks);
			} else {
				printf("Could not create renderer. SDL Error: %s\n", SDL_GetError());
				SDL_DestroyWindow(mWindow);
				mWindow = NULL;
			}
		} else {
			printf("Could not get window information. SDL Error: %s\n", SDL_GetError());
		}
	} else {
		printf("Could not create window. SDL Error: %s\n", SDL_GetError());
	}

	return mWindow != NULL && mRenderer != NULL;
}

void Window::handleEvent(SDL_Event& e) {
	// TODO: stuff
	bool updateCaption = false;

	if (e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID) {
		switch (e.window.event) {
			case SDL_WINDOWEVENT_MOVED:
				mWindowDisplayID = SDL_GetWindowDisplayIndex(mWindow);
				updateCaption = true;
				break;

			case SDL_WINDOWEVENT_SHOWN:
				mShown = true;
				break;

			case SDL_WINDOWEVENT_HIDDEN:
				mShown = false;
				break;

			case SDL_WINDOWEVENT_SIZE_CHANGED:
				mWidth = e.window.data1;
				mHeight = e.window.data2;
				// repaint
				SDL_RenderPresent(mRenderer);
				break;

			case SDL_WINDOWEVENT_EXPOSED:
				// repaint
				SDL_RenderPresent(mRenderer);
				break;

			// mouse enter
			case SDL_WINDOWEVENT_ENTER:
				mMouseFocus = true;
				updateCaption = true;
				break;

			// mouse exit
			case SDL_WINDOWEVENT_LEAVE:
				mMouseFocus = false;
				updateCaption = true;
				break;

			case SDL_WINDOWEVENT_FOCUS_GAINED:
				mKeyboardFocus = true;
				updateCaption = true;
				break;

			case SDL_WINDOWEVENT_FOCUS_LOST:
				mKeyboardFocus = false;
				updateCaption = true;
				break;

			case SDL_WINDOWEVENT_MINIMIZED:
				mMinimized = true;
				break;

			case SDL_WINDOWEVENT_MAXIMIZED:
				mMinimized = false;
				break;

			case SDL_WINDOWEVENT_RESTORED:
				mMinimized = false;
				break;

			case SDL_WINDOWEVENT_CLOSE:
				// hide on close
				SDL_HideWindow(mWindow);
				break;
		}
	} else if (e.type == SDL_KEYDOWN) {
		// display change flag
		bool switchDisplay = false;

		switch (e.key.keysym.sym) {
			case SDLK_UP:
				++mWindowDisplayID;
				switchDisplay = true;
				break;

			case SDLK_DOWN:
				--mWindowDisplayID;
				switchDisplay = true;
				break;
		}

		int totalDisplays = SDL_GetNumVideoDisplays();

		if (switchDisplay) {
			// display needs to be updated
			if (mWindowDisplayID < 0) {
				mWindowDisplayID = totalDisplays - 1;
			} else if (mWindowDisplayID >= totalDisplays) {
				mWindowDisplayID = 0;
			}

			// get bounds of each display
			SDL_Rect *displayBounds = NULL;
			displayBounds = new SDL_Rect[totalDisplays];
			for (int i = 0; i < totalDisplays; ++i) {
				SDL_GetDisplayBounds(i, &displayBounds[i]);
			}

			// move window to center of next display
			SDL_SetWindowPosition(mWindow,
					displayBounds[mWindowDisplayID].x + (displayBounds[mWindowDisplayID].w - mWidth) / 2,
					displayBounds[mWindowDisplayID].y + (displayBounds[mWindowDisplayID].h - mHeight) / 2);
			updateCaption = true;
		}
	}

	if (updateCaption) {
		// SDL_SetWindowTitle(mWindow, "Morjesta.");
	}
}

void Window::focus() {
	// restore window if needed
	if (!mShown) {
		SDL_ShowWindow(mWindow);
	}

	// move window forward
	SDL_RaiseWindow(mWindow);
}

void Window::render() {
	// if (!mMinimized) {
	Uint32 ticks_now = SDL_GetTicks();

	// clear screen
	SDL_SetRenderDrawColor(mRenderer, bg_color_.r, bg_color_.g, bg_color_.b, 0xFF);
	SDL_RenderClear(mRenderer);

	counter_.Render(ticks_now);
	circle_.Render(ticks_now);

	// update screen
	SDL_RenderPresent(mRenderer);

  // mirror to another display, drawing directly on screen
	mirror_.Draw();

  // update counter and its texture if necessary
	counter_.Update();
}

void Window::free() {
	if (mWindow != NULL) {
		SDL_DestroyWindow(mWindow);
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int Window::getWidth() {
	return mWidth;
}

int Window::getHeight() {
	return mHeight;
}

bool Window::hasMouseFocus() {
	return mMouseFocus;
}

bool Window::hasKeyboardFocus() {
	return mKeyboardFocus;
}

bool Window::isMinimized() {
	return mMinimized;
}

bool Window::isShown() {
	return mShown;
}
