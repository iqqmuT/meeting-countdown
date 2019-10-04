#ifndef WINDOW_H_
#define WINDOW_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "circle.h"
#include "config.h"
#include "counter.h"
#include "mirror.h"

class Window {
public:
	Window();
	bool Init(Config *config);
	void HandleEvent(SDL_Event& e);
	void Focus();
	int Render();
	void Free();

private:
	Config* config_;

	SDL_Window* sdl_window_;
	SDL_Renderer* renderer_;
	SDL_SysWMinfo info_;
	int window_id_;

	bool minimized_;
	bool shown_;

	Uint32 start_ticks_;
	Uint32 end_ticks_;
	Config::Color bg_color_;
	Circle circle_;
	Counter counter_;
	Mirror mirror_;
};

#endif
