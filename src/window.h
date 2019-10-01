#ifndef WINDOW_H_
#define WINDOW_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#ifdef _WIN32
	#include <windows.h>
#endif

#include "config.h"
#include "font_texture.h"

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

		// animation
		void mirror();
		int mFrame;
		FontTexture mFontTexture;
#ifdef _WIN32
		HDC mHdc;
#endif
		Uint32 mStartTicks;
		Uint32 mEndTicks;
};

#endif
