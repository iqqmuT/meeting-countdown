#include "counter.h"

#include <math.h>
#include <stdio.h>

bool Counter::Init(Config* config, SDL_Renderer* renderer, Uint32 ticks_finish) {
  config_ = config;
  renderer_ = renderer;
  ticks_finish_ = ticks_finish;
  ticks_now_ = 0;
  seconds_left_ = -1;

  // get values from config
  color_ = config_->getProgressColor();

  font_texture_.setRenderer(renderer_);
  int font_size = config_->getCounterFontSize();
  const char* font_file = config_->getCounterFontFile();
  font_texture_.loadFont(font_file, font_size);

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
      Config::Color color = config_->getCounterColor();
      SDL_Color sdl_color = { color.r, color.g, color.b };
      if (!font_texture_.loadFromRenderedText(txt, sdl_color)) {
        printf("Failed to render text texture.\n");
        return -1;
      }

      // calculate x and y position
      if (x_ == -1) {
        // set text in the center
        x_ = (config_->getWidth() / 2) - (font_texture_.getWidth() / 2);
      }
      if (y_ == -1) {
        // set text vertically in the middle
        y_ = (config_->getHeight() / 2) - (font_texture_.getHeight() / 2);
      }
    }
    seconds_left_ = seconds_left;
    return 1;
  }
  return 0;
}

void Counter::RenderProgress() {
  font_texture_.render(x_, y_);
}

void Counter::RenderPost() {
}

void Counter::Render(Uint32 ticks_now) {
  ticks_now_ = ticks_now;
  int start_duration = config_->getProgressStartDuration();
 	int end_duration = config_->getProgressEndDuration();
	if (ticks_now_ < start_duration) {
		RenderPre();
  } else if (ticks_now_ < ticks_finish_) {
    RenderProgress();
  } else if (ticks_now_ < ticks_finish_ + end_duration) {
		RenderPost();
  }
}
