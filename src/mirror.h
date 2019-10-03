#ifndef MIRROR_H_
#define MIRROR_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#ifdef _WIN32
	#include <windows.h>
#endif

#include "config.h"

class Mirror {
public:
  Mirror();
  ~Mirror();
  bool Init(Config* config, SDL_SysWMinfo* info);
  void Draw();

private:
  Config* config_;
  SDL_SysWMinfo* info_;
  bool initialized_;
  int display_;
  int width_;
  int height_;
  int x_;
  int y_;
  // SDL_Rect display_bounds_;

#ifdef _WIN32
	HDC screen_dc_;
#endif
};

#endif
