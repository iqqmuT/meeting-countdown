#ifndef CIRCLE_H_
#define CIRCLE_H_

#include <SDL2/SDL.h>

#include "config.h"

class Circle {
public:
  void Init(Config* config, SDL_Renderer* renderer, Uint32 ticks_finish);
  void Render(Uint32 ticks_now);

private:
  void RenderPre();
  void RenderProgress();
  void RenderPost();

  Config* config_;
  SDL_Renderer* renderer_;
  Uint32 ticks_finish_;
  Uint32 ticks_now_;
  int line_width_;
  int radius_;
  int x_;
  int y_;
  Config::Color color_;
  Config::Color bg_color_;
};

#endif
