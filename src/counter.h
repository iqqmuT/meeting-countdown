#ifndef COUNTER_H_
#define COUNTER_H_

#include <SDL2/SDL.h>

#include "config.h"
#include "font_texture.h"

class Counter {
public:
  bool Init(Config* config, SDL_Renderer* renderer, Uint32 ticks_finish);
  void Render(Uint32 ticks_now);
  int Update();

private:
  void RenderPre();
  void RenderProgress();
  void RenderPost();

  Config* config_;
  SDL_Renderer* renderer_;
  FontTexture font_texture_;
  Uint32 ticks_finish_;
  Uint32 ticks_now_;
  Config::Color color_;
  int x_;
  int y_;
  int seconds_left_;
  bool alpha_is_full_;
  bool is_zero_;
};

#endif
