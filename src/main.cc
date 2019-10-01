#include <getopt.h>
#include <stdio.h>
#ifdef _WIN32
	#include <windows.h>
#endif

#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "config.h"
#include "window.h"

Config config;
SDL_Rect* gDisplayBounds = NULL;
Window gWindow;

bool init(const char *cfgFile) {
	bool success = true;
	if (cfgFile != NULL) {
		if (!config.parse(cfgFile)) {
			printf("Parse error with '%s'\n", cfgFile);
			success = false;
		}
	}

	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Could not initialize SDL. Error: %s\n", SDL_GetError());
		success = false;
	} else {
		// get number of displays
		int totalDisplays = SDL_GetNumVideoDisplays();
		printf("Number of displays: %d\n", totalDisplays);

		// get bounds of each display
		gDisplayBounds = new SDL_Rect[totalDisplays];
		for (int i = 0; i < totalDisplays; ++i) {
			SDL_GetDisplayBounds(i, &gDisplayBounds[i]);
		}

		if (TTF_Init() < 0) {
			printf("Could not initialize SDL_ttf. Error: %s\n", TTF_GetError());
			success = false;
		}

		// create window
		if (!gWindow.init(&config)) {
			printf("Window could not be created!\n");
			success = false;
		}
	}

	return success;
}

void close() {
	// destroy window
	gWindow.free();

	// deallocate bounds
	delete []gDisplayBounds;
	gDisplayBounds = NULL;

	TTF_Quit();
	SDL_Quit();
}

void printUsage(char* argv[]) {
	fprintf(stderr, "Usage: %s [-t time]\n", argv[0]);
}

int main(int argc, char* argv[]) {
	int opt;
	const char *cfgFile = NULL;
	const char *timeOpt = NULL;
	// "t:" means that -t option requires an argument
	while ((opt = getopt(argc, argv, "t:c:")) != -1) {
		switch (opt) {
			case 'c':
				cfgFile = optarg;
				break;

			case 't':
				timeOpt = optarg;
				config.setEndTimeStr(timeOpt);
				break;

			default:
				printUsage(argv);
				exit(EXIT_FAILURE);
		}
	}

	if (timeOpt == NULL) {
		printUsage(argv);
		exit(EXIT_FAILURE);
	}
	printf("Time: '%s'\n", timeOpt);
	printf("Config: '%s'\n", cfgFile);


	if (init(cfgFile)) {
		bool quit = false;

		SDL_Event e;

		while (!quit) {
			while (SDL_PollEvent(&e) != 0) {
				// user requests quit
				if (e.type == SDL_QUIT) {
					quit = true;
				}

				// handle window events
				gWindow.handleEvent(e);
			}

			// update window
			gWindow.render();
		}
	} else {
		printf("Initialization failed.\n");
	}

	// free resources and close SDL
	close();

	return 0;
}
