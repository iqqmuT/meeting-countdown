#ifndef CONFIG_H_
#define CONFIG_H_

#include <ctime>
#include <string>

#include "inih/INIReader.h"

class Config {
public:
	Config();
	~Config();
	bool parse(const char* cfgFile);
	struct Color {
		int r;
		int g;
		int b;
	};

	time_t getEndTime() { return mEndTime; }
	void setEndTime(time_t endTime);
	void setEndTimeStr(const char* endTime);
	void setEndTimeSecs(int secs = 0);

	Color getBackgroundColor() { return mBgColor; }
	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }
	bool getShowProgress() { return mProgressShow; }
	bool getAnimateProgress() { return mProgressAnimate; }
	int getProgressWidth() { return mProgressWidth; }
	Color getProgressColor() { return mProgressColor; }
	Color getProgressBgColor() { return mProgressBgColor; }
	int getProgressEndDuration() { return mProgressEndDuration; }
	int getProgressStartDuration() { return mProgressStartDuration; }

	bool getCounterShow() { return mCounterShow; }
	Color getCounterColor() { return mCounterColor; }
	const char* getCounterFontFile() { return mCounterFontFile.c_str(); }
	void setCounterFontFile(const char* font_path);
	int getCounterFontSize() { return mCounterFontSize; }
	int getCounterFadeIn() { return mCounterFadeIn; }
	int getCounterFadeOut() { return mCounterFadeOut; }

	int getMirrorDisplay() { return mMirrorDisplay; }
	int getMirrorTop() { return mMirrorTop; }
	int getMirrorBottom() { return mMirrorBottom; }
	int getMirrorLeft() { return mMirrorLeft; }
	int getMirrorRight() { return mMirrorRight; }

private:
	bool parseColorVar(const char* section, const char* name, const char* def, Color& color);
	int parseColorString(const char* value, Color& color);

	Color parseColor(const char* value);

	INIReader *mParser;

	time_t mEndTime;
	Color mBgColor;
	int mWidth;
	int mHeight;
	bool mProgressShow;
	bool mProgressAnimate;
	int mProgressWidth;
	Color mProgressColor;
	Color mProgressBgColor;
	int mProgressEndDuration;
	int mProgressStartDuration;

	bool mCounterShow;
	Color mCounterColor;
	std::string mCounterFontFile;
	int mCounterFontSize;
	int mCounterFadeIn;
	int mCounterFadeOut;

	int mMirrorDisplay;
	std::string mMirrorAlign;
	int mMirrorBottom;
	int mMirrorTop;
	std::string mMirrorValign;
	int mMirrorLeft;
	int mMirrorRight;
};

#endif
