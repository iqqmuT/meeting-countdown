#include "window.h"

#include <math.h>

#include <ctime>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_ttf.h>

#include "AHEasing/easing.h"
#include "SDL2_gfx/SDL2_gfxPrimitives.h"

Window::Window() {
	sdl_window_ = NULL;
	renderer_ = NULL;
	SDL_VERSION(&info_.version);

	shown_ = false;
	window_id_ = -1;
}

SDL_Color textColor = { 0xFF, 0xFF, 0xFF };

bool Window::Init(Config* config) {
	config_ = config;

	bg_color_ = config->bg_color();
	int width = config->width();
	int height = config->height();

	// init ticks
	start_ticks_ = SDL_GetTicks();
	time_t diff = config->end_time() - time(NULL);
	end_ticks_ = start_ticks_ + (diff * 1000);

	sdl_window_ = SDL_CreateWindow("Countdown", SDL_WINDOWPOS_UNDEFINED,
	                           SDL_WINDOWPOS_UNDEFINED, width, height,
														 SDL_WINDOW_SHOWN);

	if (sdl_window_ != NULL) {
		if (SDL_GetWindowWMInfo(sdl_window_, &info_)) {
			// create renderer for window
			renderer_ = SDL_CreateRenderer(sdl_window_, -1,
			                               SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (renderer_ != NULL) {
				// initialize renderer color
				SDL_SetRenderDrawColor(renderer_, bg_color_.r, bg_color_.g, bg_color_.b, 0xFF);
				SDL_RenderClear(renderer_);

				// grab window identifiers
				window_id_ = SDL_GetWindowID(sdl_window_);

				// flag as opened
				shown_ = true;

        // initialize mirror functionality
				mirror_.Init(config, &info_);

				// initialize circle component
				circle_.Init(config, renderer_, end_ticks_);

				// initialize counter component
				counter_.Init(config, renderer_, end_ticks_);
			} else {
				std::cerr << "Could not create renderer. SDL Error: " << SDL_GetError() << "\n";
				SDL_DestroyWindow(sdl_window_);
				sdl_window_ = NULL;
			}
		} else {
			std::cerr << "Could not get window information. SDL Error: " << SDL_GetError() << "\n";
		}
	} else {
		std::cerr << "Could not create window. SDL Error: " << SDL_GetError() << "\n";
	}

	return sdl_window_ != NULL && renderer_ != NULL;
}

void Window::HandleEvent(SDL_Event& e) {
	if (e.type == SDL_WINDOWEVENT && e.window.windowID == window_id_) {
		switch (e.window.event) {
			case SDL_WINDOWEVENT_SHOWN:
				shown_ = true;
				break;

			case SDL_WINDOWEVENT_HIDDEN:
				shown_ = false;
				break;

			case SDL_WINDOWEVENT_SIZE_CHANGED:
				// repaint
				SDL_RenderPresent(renderer_);
				break;

			case SDL_WINDOWEVENT_EXPOSED:
				// repaint
				SDL_RenderPresent(renderer_);
				break;

			case SDL_WINDOWEVENT_CLOSE:
				// hide on close
				SDL_HideWindow(sdl_window_);
				break;
		}
	}
}

void Window::Focus() {
	// restore window if needed
	if (!shown_) {
		SDL_ShowWindow(sdl_window_);
	}

	// move window forward
	SDL_RaiseWindow(sdl_window_);
}

int Window::Render() {
	Uint32 ticks_now = SDL_GetTicks();

	// render only when the window is not minimized to avoid some bugs
	if (!minimized_) {

		// clear screen
		SDL_SetRenderDrawColor(renderer_, bg_color_.r, bg_color_.g, bg_color_.b, 0xFF);
		SDL_RenderClear(renderer_);

		// draw counter
		counter_.Render(ticks_now);

		// draw progress circle
		circle_.Render(ticks_now);

		// update screen
		SDL_RenderPresent(renderer_);

		// mirror to another display, drawing directly on screen
		mirror_.Draw();

		// update counter and its texture if necessary
		counter_.Update();
	}

 	int end_duration = config_->circle_end_duration();
	if (ticks_now > end_ticks_ + end_duration) {
		// countdown finished
		return -1;
	}
	return 0;
}

void Window::Free() {
	if (sdl_window_ != NULL) {
		SDL_DestroyWindow(sdl_window_);
	}
}
