#include "counter.h"

#include <math.h>
#include <stdio.h>

#include <iostream>

bool Counter::Init(Config* config, SDL_Renderer* renderer, Uint32 ticks_finish) {
  config_ = config;
  renderer_ = renderer;
  ticks_finish_ = ticks_finish;
  ticks_now_ = 0;
  seconds_left_ = -1;

  // get values from config
  color_ = config_->circle_color();

  font_texture_.set_renderer(renderer_);
  int font_size = config_->counter_font_size();
  const char* font_file = config_->counter_font_path();
  font_texture_.LoadFont(font_file, font_size);

  x_ = -1;
  y_ = -1;

  return true;
}

void Counter::RenderPre() {
}

int Counter::Update() {
  int seconds_left = floor((ticks_finish_ - ticks_now_) / 1000);
  if (seconds_left != seconds_left_) {
    // update texture only when remaining seconds change
    char txt[64];
    int min = floor(seconds_left / 60);
    int sec = seconds_left % 60;

    sprintf(txt, "%d:%02d", min, sec);
    if (seconds_left >= 0) {
      Config::Color color = config_->counter_color();
      SDL_Color sdl_color = { color.r, color.g, color.b };
      if (!font_texture_.LoadFromRenderedText(txt, sdl_color)) {
        std::cerr << "Failed to render text texture.\n";
        return -1;
      }

      // calculate x and y position
      if (x_ == -1) {
        // set text in the center
        x_ = (config_->width() / 2) - (font_texture_.width() / 2);
        // fine tuning: it looks better when moved a bit to right
        x_ += 2;
      }
      if (y_ == -1) {
        // set text vertically in the middle
        y_ = (config_->height() / 2) - (font_texture_.height() / 2);
      }
    }
    seconds_left_ = seconds_left;
    return 1;
  }
  return 0;
}

void Counter::RenderProgress() {
  font_texture_.Render(x_, y_);
}

void Counter::RenderPost() {
}

void Counter::Render(Uint32 ticks_now) {
  ticks_now_ = ticks_now;
  int start_duration = config_->circle_start_duration();
 	int end_duration = config_->circle_end_duration();
	if (ticks_now_ < start_duration) {
		RenderPre();
  } else if (ticks_now_ < ticks_finish_) {
    RenderProgress();
  } else if (ticks_now_ < ticks_finish_ + end_duration) {
		RenderPost();
  }
}
