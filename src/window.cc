#include "window.h"

#include <stdio.h>
#include <math.h>

#include <ctime>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_ttf.h>
#ifdef _WIN32
	#include <windows.h>
#endif

#include "AHEasing/easing.h"
#include "SDL2_gfx/SDL2_gfxPrimitives.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
// const int TRANSPARENCY_COLOR[3] = {0xFF, 0, 0xFF};
const int TRANSPARENCY_COLOR[3] = {0, 0, 0};


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

	mFrame = 0;

  // start counting after these ticks
	mStartCountingTicks = 1000;
}

#ifdef _WIN32
// Makes window transparent using Windows API.
// https://stackoverflow.com/a/51956224/3325516
bool Window::enableTransparency() {
	HWND hWindow = mInfo.info.win.window;

	// Change window type to layered
	SetWindowLong(hWindow, GWL_EXSTYLE, GetWindowLong(hWindow, GWL_EXSTYLE) | WS_EX_LAYERED);

	// Set transparency color
	COLORREF colorKey = RGB(TRANSPARENCY_COLOR[0], TRANSPARENCY_COLOR[1], TRANSPARENCY_COLOR[2]);
	return SetLayeredWindowAttributes(hWindow, colorKey, 0, LWA_COLORKEY);
}
#else
bool Window::enableTransparency() {
	return false;
}
#endif


SDL_Color textColor = { 0xFF, 0xFF, 0xFF };

bool Window::init(Config* config) {
	mConfig = config;

	// init ticks
	mStartTicks = SDL_GetTicks();
	time_t diff = config->getEndTime() - time(NULL);
	mEndTicks = mStartTicks + (diff * 1000);

#ifdef _WIN32
	mHdc = GetDC(NULL);
	// CreateWindowIndirect();

	// CreateWindowInBand();
	/*
	char title[500];
	GetConsoleTitleA(title, 500);
	printf("Console Title: %s\n", title);

	HWND hwndConsole = FindWindowA(NULL, title);

	HINSTANCE hInstance = (HINSTANCE) GetWindowLong(hwndConsole, GWL_HINSTANCE);


	HWND foo = CreateWindowEx(
			WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
			// "MonitorNumberWindow",
			"Button",
			"hoppa",
			WS_POPUP|WS_VISIBLE|WS_CLIPSIBLINGS|WS_SYSMENU|WS_GROUP|WS_TABSTOP,
			200,
			100,
			640,
			480,
			NULL,
			NULL,
			hInstance,
			NULL
			);
	if (foo == NULL) {
		printf("%d\n", GetLastError());
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
	} else {
		ShowWindow(foo, true);
		UpdateWindow(foo);
	}
	*/
#endif

	// create window
	// mWindow = SDL_CreateWindow("Countdown", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS);

	mWindow = SDL_CreateWindow("Countdown", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (mWindow != NULL) {
		if (SDL_GetWindowWMInfo(mWindow, &mInfo)) {
			mMouseFocus = true;
			mKeyboardFocus = true;
			mWidth = SCREEN_WIDTH;
			mHeight = SCREEN_HEIGHT;

			// create renderer for window
			mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (mRenderer != NULL) {
				// initialize renderer color
				SDL_SetRenderDrawColor(mRenderer, TRANSPARENCY_COLOR[0], TRANSPARENCY_COLOR[1], TRANSPARENCY_COLOR[2], 0xFF);
				SDL_RenderClear(mRenderer);

				// grab window identifiers
				mWindowID = SDL_GetWindowID(mWindow);
				mWindowDisplayID = SDL_GetWindowDisplayIndex(mWindow);

				// enableTransparency();
				//

				mFontTexture.setRenderer(mRenderer);
				// mFontTexture.loadFont("lazy.ttf", 28);
				int fontSize = mConfig->getCounterFontSize();
				const char *fontFile = mConfig->getCounterFontFile();
				mFontTexture.loadFont(fontFile, fontSize);

				/*
				if (!mFontTexture.loadFromRenderedText("Hello.", textColor)) {
					printf("Failed to render text texture.\n");
				}
				*/

				// flag as opened
				mShown = true;

				// initialize circle component
				circle_.Init(config, mRenderer, mEndTicks);

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
		SDL_SetWindowTitle(mWindow, "Morjesta.");
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

COLORREF black = RGB(0, 0, 0);
COLORREF colorKey = RGB(0xFF, 0, 0);


HBRUSH hbr = CreateSolidBrush(colorKey);
HBRUSH blackHbr = CreateSolidBrush(black);

#ifdef _WIN32
// Copies pixels from window's device context and draws it directly in the screen.
void Window::mirror() {
	HWND hWindow = mInfo.info.win.window;
	HDC srcDC = GetDC(hWindow);
	// HDC dstDC = CreateCompatibleDC(srcDC);

	// BitBlt(orig, 0, 0, 200, 200, newDC, 0, 0, SRCCOPY);

	BitBlt(mHdc, 1250, 510, mConfig->getWidth(), mConfig->getHeight(), srcDC, 0, 0, SRCCOPY);

	// DeleteDC(newDC);
	ReleaseDC(hWindow, srcDC);
}
#else
void Window::mirror() {
}
#endif

int secsLeft = -1;

void Window::render() {
	++mFrame;
	// if (!mMinimized) {

	mTicksNow = SDL_GetTicks();
	int endDuration = mConfig->getProgressEndDuration();
	if (mTicksNow < mEndTicks + endDuration) {
		int newSecsLeft = floor((mEndTicks - mTicksNow) / 1000);
		int min = floor(secsLeft / 60);
		int sec = secsLeft % 60;

		time_t now = time(0);
		tm *ltm = localtime(&now);

		// printf("Seconds left: %d\n", secsLeft);

		if (secsLeft != newSecsLeft) {
			if (min == 0 && sec == 0) {
				printf("Hetkinen: %d %d %d\n", min, sec, secsLeft);
			}
			char foo[500];
			sprintf(foo, "%d:%02d", min, sec);
			if (secsLeft >= 0) {
				if (!mFontTexture.loadFromRenderedText(foo, textColor)) {
					printf("Failed to render text texture.\n");
				}
			}
			secsLeft = newSecsLeft;
		}

		// printf("Texture width: %d\n", mFontTexture.getWidth());


		Config::Color bgColor = mConfig->getBackgroundColor();

		// clear screen
		SDL_SetRenderDrawColor(mRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
		SDL_RenderClear(mRenderer);

		mFontTexture.render(70, 70);

		// draw red square
		// SDL_Rect rect1 = {0, 0, 100, 100};
		// SDL_SetRenderDrawColor(mRenderer, 0, 0, 0xFF, 0xFF);
		// SDL_RenderFillRect(mRenderer, &rect1);
		// SDL_RenderPresent(mRenderer);


		// pixelRGBA(mRenderer, 100, 100, 0, 0, 0, 0xFF);
		int x = mFrame;

		float progress = (float) mTicksNow / (float) mEndTicks;

		int width = mConfig->getProgressWidth();
		Config::Color lineColor = mConfig->getProgressColor();

		// std::cout << "progress: " << progress << "\n";
		// filledPieRGBA(mRenderer, 100, 100, 100, -90, (progress * 360.0) - 90.0, lineColor.r, lineColor.g, lineColor.b, 0xFF);
		// filledCircleRGBA(mRenderer, 100, 100, 100 - width, bgColor.r, bgColor.g, bgColor.b, 0xFF);

		// int size = 80;
		// aaArcRGBA(mRenderer, 100, 100, size, size, -90, 360 - 89.9, width, 0x11, 0x11, 0x11, 0xFF);
		// aaArcRGBA(mRenderer, 100, 100, size, size, -90, (progress * 360.0) - 90.0, width, lineColor.r, lineColor.g, lineColor.b, 0xFF);

		// filledCircleRGBA(mRenderer, x, 200, 99, 0, 0xFF, 0, 0xFF);
		// aacircleRGBA(mRenderer, x, 200, 100, 0, 0xFF, 0, 0xFF);
		// SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a)

		// filledCircleRGBA(mRenderer, x, 200, 90, 0, 0, 0, 0xFF);
		// circleRGBA(mRenderer, 100, 100, 10, 0, 0XFF, 0, 0xFF);

		// Go to next frame
		// ++mFrame;



/*
#ifdef _WIN32
		RECT rect = { 50, 400, 350, 700 };
		FillRect(mHdc, &rect, hbr);

		RECT rectBlack = { 1600 + mFrame - 1, 10, 1800 + mFrame - 1, 200 };
		FillRect(mHdc, &rectBlack, blackHbr);


		RECT rect2 = { 1600 + mFrame, 10, 1800 + mFrame, 200 };
		FillRect(mHdc, &rect2, hbr);


		HWND hWindow = mInfo.info.win.window;
		HDC orig = GetDC(hWindow);
		HDC newDC = CreateCompatibleDC(orig);

		// BitBlt(orig, 0, 0, 200, 200, newDC, 0, 0, SRCCOPY);

		BitBlt(mHdc, 1600, 500, 200, 200, orig, 0, 0, SRCCOPY);

		DeleteDC(newDC);
		DeleteDC(orig);


#endif
*/

	}
	circle_.Render(mTicksNow);

	// update screen
	SDL_RenderPresent(mRenderer);

	mirror();
	// }
}

void Window::free() {
	if (mWindow != NULL) {
		SDL_DestroyWindow(mWindow);
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;

#ifdef _WIN32
	ReleaseDC(NULL, mHdc);
#endif
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
