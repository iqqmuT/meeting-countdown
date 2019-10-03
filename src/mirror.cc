#include "mirror.h"

Mirror::Mirror() {
  initialized_ = true;
}

Mirror::~Mirror() {
  if (initialized_) {
#ifdef _WIN32
  	ReleaseDC(NULL, screen_dc_);
#endif
  }
}

bool Mirror::Init(Config* config, SDL_SysWMinfo* info) {
  config_ = config;
  info_ = info;

  // get values from config
  width_ = config_->getWidth();
  height_ = config_->getHeight();
  display_ = config_->getMirrorDisplay();

  // get number of displays
  int total_displays = SDL_GetNumVideoDisplays();

  x_ = 0;
  y_ = 0;

  int left = config_->getMirrorLeft();
  int right = config_->getMirrorRight();
  if (left == -1 && right == -1) {
    // default
    left = 50;
  }
  int top = config_->getMirrorTop();
  int bottom = config_->getMirrorBottom();
  if (top == -1 && bottom == -1) {
    // default
    bottom = 50;
  }

  if (display_ <= total_displays) {
    SDL_Rect display_bounds;
    SDL_GetDisplayBounds(display_ - 1, &display_bounds);

    // calculate position for mirroring
    x_ = display_bounds.x;
    if (left != -1) {
      // use 'left' config value
      x_ += left;
    } else if (right != -1) {
      // use 'right' config value
      x_ += display_bounds.w - width_ - right;
    }

    if (top != -1) {
      // use 'top' config value
      y_ = top;
    } else if (bottom != -1) {
      // use 'bottom' config value
      y_ = display_bounds.h - height_ - bottom;
    }
  }

#ifdef _WIN32
  // get device context for entire screen, including all displays
	screen_dc_ = GetDC(NULL);
#endif

  return true;
}

#ifdef _WIN32
// Copies pixels from window's device context and draws it directly in the screen.
void Mirror::Draw() {
	HWND hWindow = info_->info.win.window;
	HDC srcDC = GetDC(hWindow);

  // use BitBlt to copy pixels from rectangle area
	BitBlt(screen_dc_, x_, y_, width_, height_, srcDC, 0, 0, SRCCOPY);

  // release device context
	ReleaseDC(hWindow, srcDC);
}
#else
void Mirror::Draw() {
  // no implementation
}
#endif
