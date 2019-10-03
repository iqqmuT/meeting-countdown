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
	bool init(Config *config);
	bool enableTransparency();
	void handleEvent(SDL_Event& e);
	void focus();
	void render();
	void free();
	int getWidth();
	int getHeight();
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimized();
	bool isShown();

private:
	Config* mConfig;

	// window data
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	SDL_SysWMinfo mInfo;
	int mWindowID;
	int mWindowDisplayID;

	// window dimensions
	int mWidth;
	int mHeight;

	// window focus
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimized;
	bool mShown;

	Uint32 mStartTicks;
	Uint32 mStartCountingTicks;
	Uint32 mEndTicks;
	Config::Color bg_color_;
	Circle circle_;
	Counter counter_;
	Mirror mirror_;
};

#endif
