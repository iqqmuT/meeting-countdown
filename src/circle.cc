#include "circle.h"

#include "AHEasing/easing.h"
#include "SDL2_gfx/SDL2_gfxPrimitives.h"

void Circle::Init(Config* config, SDL_Renderer* renderer, Uint32 ticks_finish) {
  config_ = config;
  renderer_ = renderer;
  ticks_finish_ = ticks_finish;
  ticks_now_ = 0;

  // get values from config
  color_ = config_->circle_color();
  bg_color_ = config_->circle_bg_color();
  line_width_ = config_->circle_width();
  radius_ = config_->width() / 2 - (line_width_ / 2) - 2;
  x_ = config_->width() / 2;
  y_ = x_;
}

void Circle::RenderPre() {
  int startDuration = config_->circle_start_duration();
	float progress = (float) ticks_now_ / startDuration;
	// use easing in animation
	progress = CubicEaseInOut(progress);
	aaArcRGBA(renderer_, x_, y_, radius_, radius_,
            -90, (progress * 360.0) - 90.0, line_width_,
            bg_color_.r, bg_color_.g, bg_color_.b, 0xFF);
}

void Circle::RenderProgress() {
	float progress = (float) ticks_now_ / (float) ticks_finish_;

  // draw full circle in background
	aaArcRGBA(renderer_, x_, y_, radius_, radius_,
            -90, 270, line_width_,
            bg_color_.r, bg_color_.g, bg_color_.b, 0xFF);

  // draw progress
	aaArcRGBA(renderer_, x_, y_, radius_, radius_,
            -90, (progress * 360.0) - 90.0, line_width_,
            color_.r, color_.g, color_.b, 0xFF);
}

void Circle::RenderPost() {
	int duration = config_->circle_end_duration();
	float progress = (float) (ticks_now_ - ticks_finish_) / duration;
	// use easing in animation
	progress = CubicEaseInOut(progress);
	aaArcRGBA(renderer_, x_, y_, radius_, radius_,
            (progress * 360.0) - 90.0, 270, line_width_,
            color_.r, color_.g, color_.b, 0xFF);
}

void Circle::Render(Uint32 ticks_now) {
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
