#include "config.h"

#include <stdlib.h>
#include <string.h>

#include <ctime>
#include <iostream>
#include <string>

const char* SECTION_CIRCLE = "circle";
const char* SECTION_COUNTER = "counter";
const char* SECTION_MIRROR = "mirror";

const int DEFAULT_TIME_SECS = 3 * 60; // default 3 min countdown

const char* DEFAULT_BACKGROUND_COLOR = "#000000";
const int DEFAULT_WIDTH = 200;
const int DEFAULT_HEIGHT = 200;

const int DEFAULT_CIRCLE_WIDTH = 10;
const char* DEFAULT_CIRCLE_COLOR = "#ffffff";
const char* DEFAULT_CIRCLE_BG_COLOR = "#333333";
const int DEFAULT_CIRCLE_DURATION_START = 1000;
const int DEFAULT_CIRCLE_DURATION_END = 1500;

// Default font in Windows: Segoe UI
const char* DEFAULT_COUNTER_FONT_FILE = "C:\\Windows\\Fonts\\segoeui.ttf";
const char* DEFAULT_COUNTER_COLOR = "#ffffff";
const int DEFAULT_COUNTER_FONT_SIZE = 40;
const int DEFAULT_COUNTER_FADE_IN = 200;
const int DEFAULT_COUNTER_FADE_OUT = 200;

const int DEFAULT_MIRROR_DISPLAY = 2;
const int DEFAULT_MIRROR_TOP = -1;
const int DEFAULT_MIRROR_BOTTOM = -1;
const int DEFAULT_MIRROR_LEFT = -1;
const int DEFAULT_MIRROR_RIGHT = -1;

int convertHexChar(char c) {
	static const char* up = "0123456789ABCDEF";
	const char* p1 = std::lower_bound(up, up + 16, c);
	if (*p1 == c) {
		return p1 - up;
	}

	static const char* down = "0123456789abcdef";
	const char* p2 = std::lower_bound(down, down + 16, c);
	if (*p2 == c) {
		return p2 - down;
	}

	// illegal character
	return -1;
}

int convertHex(char c1, char c2) {
	int v1 = convertHexChar(c1);
	int v2 = convertHexChar(c2);
	if (v1 < 0 || v2 < 0) {
		return -1;
	}
	return (v1 << 4) | v2;
}

Config::Config() {
	parser_ = NULL;

	// defaults
	ParseColorString(DEFAULT_BACKGROUND_COLOR, bg_color_);
	width_ = DEFAULT_WIDTH;
	height_ = DEFAULT_HEIGHT;

	circle_width_ = DEFAULT_CIRCLE_WIDTH;
	ParseColorString(DEFAULT_CIRCLE_COLOR, circle_color_);
	ParseColorString(DEFAULT_CIRCLE_BG_COLOR, circle_bg_color_);
	circle_start_duration_ = DEFAULT_CIRCLE_DURATION_START;
	circle_end_duration_ = DEFAULT_CIRCLE_DURATION_END;

	counter_font_path_ = DEFAULT_COUNTER_FONT_FILE;
	counter_font_size_ = DEFAULT_COUNTER_FONT_SIZE;
	ParseColorString(DEFAULT_COUNTER_COLOR, counter_color_);
	counter_fade_in_ = DEFAULT_COUNTER_FADE_IN;
	counter_fade_out_ = DEFAULT_COUNTER_FADE_OUT;

	mirror_display_ = DEFAULT_MIRROR_DISPLAY;
	mirror_left_ = DEFAULT_MIRROR_LEFT;
	mirror_right_ = DEFAULT_MIRROR_RIGHT;
	mirror_top_ = DEFAULT_MIRROR_TOP;
	mirror_bottom_ = DEFAULT_MIRROR_BOTTOM;
}

bool Config::Parse(const char* cfgFile) {
	parser_ = new INIReader(cfgFile);
	int err = parser_->ParseError();
	if (err < 0) {
		return false;
	}

	bool success = true;

	if (!ParseColorVar("", "background-color", DEFAULT_BACKGROUND_COLOR, bg_color_)) {
		success = false;
	}

	width_ = parser_->GetInteger("", "width", DEFAULT_WIDTH);
	height_ = parser_->GetInteger("", "height", DEFAULT_HEIGHT);

	// [circle]
	circle_show_ = parser_->GetBoolean(SECTION_CIRCLE, "show", true);
	circle_animate_ = parser_->GetBoolean(SECTION_CIRCLE, "animate", true);
	circle_width_ = parser_->GetInteger(SECTION_CIRCLE, "width", DEFAULT_CIRCLE_WIDTH);

	if (!ParseColorVar(SECTION_CIRCLE, "color", DEFAULT_CIRCLE_COLOR, circle_color_)) {
		success = false;
	}

	if (!ParseColorVar(SECTION_CIRCLE, "background-color", DEFAULT_CIRCLE_BG_COLOR, circle_bg_color_)) {
		success = false;
	}

	circle_end_duration_ = parser_->GetInteger(SECTION_CIRCLE, "duration-end", DEFAULT_CIRCLE_DURATION_END);
	circle_start_duration_ = parser_->GetInteger(SECTION_CIRCLE, "duration-start", DEFAULT_CIRCLE_DURATION_START);

	// [counter]
	counter_show_ = parser_->GetBoolean(SECTION_COUNTER, "show", true);
	if (!ParseColorVar(SECTION_COUNTER, "color", DEFAULT_COUNTER_COLOR, counter_color_)) {
		success = false;
	}
	counter_font_path_ = parser_->Get(SECTION_COUNTER, "font-file", DEFAULT_COUNTER_FONT_FILE);
	counter_font_size_ = parser_->GetInteger(SECTION_COUNTER, "font-size", DEFAULT_COUNTER_FONT_SIZE);
	counter_fade_in_ = parser_->GetInteger(SECTION_COUNTER, "fade-in", DEFAULT_COUNTER_FADE_IN);
	counter_fade_out_ = parser_->GetInteger(SECTION_COUNTER, "fade-out", DEFAULT_COUNTER_FADE_OUT);

	// [mirror]
	mirror_display_ = parser_->GetInteger(SECTION_MIRROR, "display", DEFAULT_MIRROR_DISPLAY);
	mirror_align_ = parser_->Get(SECTION_MIRROR, "align", "center");
	mirror_bottom_ = parser_->GetInteger(SECTION_MIRROR, "bottom", DEFAULT_MIRROR_BOTTOM);
	mirror_top_ = parser_->GetInteger(SECTION_MIRROR, "top", DEFAULT_MIRROR_TOP);
	mirror_valign_ = parser_->Get(SECTION_MIRROR, "valign", "middle");
	mirror_left_ = parser_->GetInteger(SECTION_MIRROR, "left", DEFAULT_MIRROR_LEFT);
	mirror_right_ = parser_->GetInteger(SECTION_MIRROR, "right", DEFAULT_MIRROR_RIGHT);

	return success;
}

Config::~Config() {
	if (parser_ != NULL) {
		delete parser_;
	}
}

void Config::set_width(int width) {
	width_ = width;
}

void Config::set_height(int height) {
	height_ = height;
}

void Config::set_counter_font_size(int size) {
	counter_font_size_ = size;
}

bool Config::ParseColorVar(const char* section, const char* name, const char* def, Config::Color& color) {
	std::string value = parser_->Get(section, name, def);
	if (ParseColorString(value.c_str(), color) < 0) {
		std::cout << "Error parsing " << section << "." << name << ": Invalid format: " << value << "\n";
		return false;
	}
	return true;
}

int Config::ParseColorString(const char* value, Config::Color& color) {
	size_t len = strlen(value);
	if (value[0] == '#' && len == 7) {
		int v[3];
		for (int i = 0; i < 3; i++) {
			v[i] = convertHex(value[i*2+1], value[i*2+2]);
			if (v[i] < 0) {
				return -2;
			}
		}
		color.r = v[0];
		color.g = v[1];
		color.b = v[2];
		return 0;
	}
	return -1;
}

bool ParseTime(const char* ts, int &hour, int &min) {
	char h[3] = { 0, 0, 0 };
	char m[3] = { 0, 0, 0 };
	bool isPM = false;
	if (strlen(ts) >= 4 && isdigit(ts[0]) && isdigit(ts[2])) {
		h[0] = ts[0];
		h[1] = ts[1];
		m[0] = ts[2];
		m[1] = ts[3];
		if (strlen(ts) > 4 && (ts[4] == 'P' || ts[4] == 'p')) {
			isPM = true;
		}
	} else if (strlen(ts) >= 5 && isdigit(ts[0]) && isdigit(ts[3])) {
		h[0] = ts[0];
		h[1] = ts[1];
		m[0] = ts[3];
		m[1] = ts[4];
		if (strlen(ts) > 5 && (ts[5] == 'P' || ts[5] == 'p')) {
			isPM = true;
		}
	} else {
		return false;
	}

	hour = atoi(h);
	if (isPM) {
		hour += 12;
	}
	min = atoi(m);

	return true;
}

void Config::set_end_time(time_t endTime) {
	end_time_ = endTime;
}

void Config::set_end_time_str(const char* endTime) {
	// get current time so we have current date
	time_t tNow = time(NULL);
	struct tm* tmEnd = localtime(&tNow);

	int hour = 0;
	int min = 0;
	if (ParseTime(endTime, hour, min)) {
		int nowHours = tmEnd->tm_hour * 100 + tmEnd->tm_min;
		int endHours = hour * 100 + min;
		if (endHours <= nowHours) {
			// move end time to next day
			tmEnd->tm_mday++;
		}

		tmEnd->tm_hour = hour;
		tmEnd->tm_min = min;
		tmEnd->tm_sec = 0;

		set_end_time(mktime(tmEnd));
	} else {
		std::cout << "Error: could not parse " << endTime << "\n";
	}
}

void Config::set_end_time_secs(int secs) {
	time_t t_now = time(NULL);
	struct tm* tm_end = localtime(&t_now);
	if (secs == 0) {
		secs = DEFAULT_TIME_SECS;
	}
	// add extra second so beginning numbers look better
	tm_end->tm_sec += secs + 1;
	set_end_time(mktime(tm_end));
}

void Config::set_counter_font_path(const char* font_path) {
	counter_font_path_ = font_path;
}
