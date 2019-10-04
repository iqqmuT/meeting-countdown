#include "config.h"

#include <stdlib.h>
#include <string.h>

#include <ctime>
#include <iostream>
#include <string>

const char* SECTION_PROGRESS = "progress";
const char* SECTION_COUNTER = "counter";
const char* SECTION_MIRROR = "mirror";

const int DEFAULT_TIME_SECS = 3 * 60; // default 3 min countdown

const char* DEFAULT_BACKGROUND_COLOR = "#000000";
const int DEFAULT_WIDTH = 200;
const int DEFAULT_HEIGHT = 200;

const int DEFAULT_PROGRESS_WIDTH = 10;
const char* DEFAULT_PROGRESS_COLOR = "#ffffff";
const char* DEFAULT_PROGRESS_BG_COLOR = "#333333";
const int DEFAULT_PROGRESS_DURATION_START = 1000;
const int DEFAULT_PROGRESS_DURATION_END = 2000;

// Default font in Windows: Segoe UI
const char* DEFAULT_COUNTER_FONT_FILE = "C:\\Windows\\Fonts\\segoeui.ttf";
const char* DEFAULT_COUNTER_COLOR = "#ffffff";
const int DEFAULT_COUNTER_FONT_SIZE = 40;

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
	mParser = NULL;

	// defaults
	parseColorString(DEFAULT_BACKGROUND_COLOR, mBgColor);
	mWidth = DEFAULT_WIDTH;
	mHeight = DEFAULT_HEIGHT;

	mProgressWidth = DEFAULT_PROGRESS_WIDTH;
	parseColorString(DEFAULT_PROGRESS_COLOR, mProgressColor);
	parseColorString(DEFAULT_PROGRESS_BG_COLOR, mProgressBgColor);
	mProgressStartDuration = DEFAULT_PROGRESS_DURATION_START;
	mProgressEndDuration = DEFAULT_PROGRESS_DURATION_END;

	mCounterFontFile = DEFAULT_COUNTER_FONT_FILE;
	mCounterFontSize = DEFAULT_COUNTER_FONT_SIZE;
	parseColorString(DEFAULT_COUNTER_COLOR, mCounterColor);

	mMirrorDisplay = DEFAULT_MIRROR_DISPLAY;
	mMirrorLeft = DEFAULT_MIRROR_LEFT;
	mMirrorRight = DEFAULT_MIRROR_RIGHT;
	mMirrorTop = DEFAULT_MIRROR_TOP;
	mMirrorBottom = DEFAULT_MIRROR_BOTTOM;
}

bool Config::parse(const char* cfgFile) {
	mParser = new INIReader(cfgFile);
	int err = mParser->ParseError();
	if (err < 0) {
		return false;
	}

	bool success = true;

	if (!parseColorVar("", "background-color", DEFAULT_BACKGROUND_COLOR, mBgColor)) {
		success = false;
	}

	mWidth = mParser->GetInteger("", "width", DEFAULT_WIDTH);
	mHeight = mParser->GetInteger("", "height", DEFAULT_HEIGHT);

	// [progress]
	mProgressShow = mParser->GetBoolean(SECTION_PROGRESS, "show", true);
	mProgressAnimate = mParser->GetBoolean(SECTION_PROGRESS, "animate", true);
	mProgressWidth = mParser->GetInteger(SECTION_PROGRESS, "width", DEFAULT_PROGRESS_WIDTH);

	if (!parseColorVar(SECTION_PROGRESS, "color", DEFAULT_PROGRESS_COLOR, mProgressColor)) {
		success = false;
	}

	if (!parseColorVar(SECTION_PROGRESS, "background-color", DEFAULT_PROGRESS_BG_COLOR, mProgressBgColor)) {
		success = false;
	}

	mProgressEndDuration = mParser->GetInteger(SECTION_PROGRESS, "duration-end", DEFAULT_PROGRESS_DURATION_END);
	mProgressStartDuration = mParser->GetInteger(SECTION_PROGRESS, "duration-start", DEFAULT_PROGRESS_DURATION_START);

	// [counter]
	mCounterShow = mParser->GetBoolean(SECTION_COUNTER, "show", true);
	if (!parseColorVar(SECTION_COUNTER, "color", DEFAULT_COUNTER_COLOR, mCounterColor)) {
		success = false;
	}
	mCounterFontFile = mParser->Get(SECTION_COUNTER, "font-file", DEFAULT_COUNTER_FONT_FILE);
	mCounterFontSize = mParser->GetInteger(SECTION_COUNTER, "font-size", DEFAULT_COUNTER_FONT_SIZE);
	mCounterFadeIn = mParser->GetInteger(SECTION_COUNTER, "fade-in", 300);
	mCounterFadeOut = mParser->GetInteger(SECTION_COUNTER, "fade-out", 300);

	// [mirror]
	mMirrorDisplay = mParser->GetInteger(SECTION_MIRROR, "display", DEFAULT_MIRROR_DISPLAY);
	mMirrorAlign = mParser->Get(SECTION_MIRROR, "align", "center");
	mMirrorBottom = mParser->GetInteger(SECTION_MIRROR, "bottom", DEFAULT_MIRROR_BOTTOM);
	mMirrorTop = mParser->GetInteger(SECTION_MIRROR, "top", DEFAULT_MIRROR_TOP);
	mMirrorValign = mParser->Get(SECTION_MIRROR, "valign", "middle");
	mMirrorLeft = mParser->GetInteger(SECTION_MIRROR, "left", DEFAULT_MIRROR_LEFT);
	mMirrorRight = mParser->GetInteger(SECTION_MIRROR, "right", DEFAULT_MIRROR_RIGHT);

	return success;
}

Config::~Config() {
	if (mParser != NULL) {
		delete mParser;
	}
}

bool Config::parseColorVar(const char* section, const char* name, const char* def, Config::Color& color) {
	std::string value = mParser->Get(section, name, def);
	if (parseColorString(value.c_str(), color) < 0) {
		std::cout << "Error parsing " << section << "." << name << ": Invalid format: " << value << "\n";
		return false;
	}
	return true;
}

int Config::parseColorString(const char* value, Config::Color& color) {
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

bool parseTime(const char* ts, int &hour, int &min) {
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

void Config::setEndTime(time_t endTime) {
	mEndTime = endTime;
}

void Config::setEndTimeStr(const char* endTime) {
	// get current time so we have current date
	time_t tNow = time(NULL);
	struct tm* tmEnd = localtime(&tNow);

	int hour = 0;
	int min = 0;
	if (parseTime(endTime, hour, min)) {
		int nowHours = tmEnd->tm_hour * 100 + tmEnd->tm_min;
		int endHours = hour * 100 + min;
		if (endHours <= nowHours) {
			// move end time to next day
			tmEnd->tm_mday++;
		}

		tmEnd->tm_hour = hour;
		tmEnd->tm_min = min;
		tmEnd->tm_sec = 0;

		setEndTime(mktime(tmEnd));
	} else {
		std::cout << "Error: could not parse " << endTime << "\n";
	}
}

void Config::setEndTimeSecs(int secs) {
	time_t t_now = time(NULL);
	struct tm* tm_end = localtime(&t_now);
	if (secs == 0) {
		secs = DEFAULT_TIME_SECS;
	}
	tm_end->tm_sec += secs;
	setEndTime(mktime(tm_end));
}

void Config::setCounterFontFile(const char* font_path) {
	mCounterFontFile = font_path;
}
