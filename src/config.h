#ifndef CONFIG_H_
#define CONFIG_H_

#include <ctime>
#include <string>

#include "inih/INIReader.h"

class Config {
public:
	Config();
	~Config();
	bool Parse(const char* cfgFile);
	struct Color {
		int r;
		int g;
		int b;
	};

	time_t end_time() { return end_time_; }
	void set_end_time(time_t endTime);
	void set_end_time_str(const char* endTime);
	void set_end_time_secs(int secs = 0);

	Color bg_color() { return bg_color_; }
	int width() { return width_; }
	int height() { return height_; }
	bool circle_show() { return circle_show_; }
	bool circle_animate() { return circle_animate_; }
	int circle_width() { return circle_width_; }
	Color circle_color() { return circle_color_; }
	Color circle_bg_color() { return circle_bg_color_; }
	int circle_end_duration() { return circle_end_duration_; }
	int circle_start_duration() { return circle_start_duration_; }

	bool counter_show() { return counter_show_; }
	Color counter_color() { return counter_color_; }
	const char* counter_font_path() { return counter_font_path_.c_str(); }
	void set_counter_font_path(const char* font_path);
	int counter_font_size() { return counter_font_size_; }
	int counter_fade_in() { return counter_fade_in_; }
	int counter_fade_out() { return counter_fade_out_; }

	int mirror_display() { return mirror_display_; }
	int mirror_top() { return mirror_top_; }
	int mirror_bottom() { return mirror_bottom_; }
	int mirror_left() { return mirror_left_; }
	int mirror_right() { return mirror_right_; }

private:
	bool ParseColorVar(const char* section, const char* name, const char* def, Color& color);
	int ParseColorString(const char* value, Color& color);

	Color ParseColor(const char* value);

	INIReader *parser_;

	time_t end_time_;
	Color bg_color_;
	int width_;
	int height_;
	bool circle_show_;
	bool circle_animate_;
	int circle_width_;
	Color circle_color_;
	Color circle_bg_color_;
	int circle_end_duration_;
	int circle_start_duration_;

	bool counter_show_;
	Color counter_color_;
	std::string counter_font_path_;
	int counter_font_size_;
	int counter_fade_in_;
	int counter_fade_out_;

	int mirror_display_;
	std::string mirror_align_;
	int mirror_bottom_;
	int mirror_top_;
	std::string mirror_valign_;
	int mirror_left_;
	int mirror_right_;
};

#endif
