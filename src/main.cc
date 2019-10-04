#include <getopt.h>
#ifdef _WIN32
	#include <windows.h>
#endif

#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "config.h"
#include "window.h"

#ifdef VERSION
const volatile static char version[] = VERSION;
#endif

Config config;
Window main_window;

bool init(const char *cfg_file) {
	bool success = true;
	if (cfg_file != NULL) {
		if (!config.Parse(cfg_file)) {
			std::cerr << "Parse error with " << cfg_file << "\n";
			success = false;
		}
	}

	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Could not initialize SDL. Error: " << SDL_GetError() << "\n";
		success = false;
	} else {
		if (TTF_Init() < 0) {
			std::cerr << "Could not initialize SDL_ttf. Error: " << TTF_GetError() << "\n";
			success = false;
		}

		// create window
		if (!main_window.Init(&config)) {
			std::cerr << "Window could not be created!\n";
			success = false;
		}
	}

	return success;
}

void close() {
	// destroy window
	main_window.Free();

	TTF_Quit();
	SDL_Quit();
}

void printUsage(char* argv[]) {
	std::cerr << "Usage: " << argv[0] << " [-c configfile] [-f fontfile] [-m minutes] [-t HH:MM]\n";
}

int main(int argc, char* argv[]) {
	int opt;
	int mins = -1;
	const char* cfg_file = NULL;
	const char* font_path = NULL;
	const char* time_opt = NULL;
	// "t:" means that -t option requires an argument
	while ((opt = getopt(argc, argv, "t:c:m:f:")) != -1) {
		switch (opt) {
			case 'c':
				cfg_file = optarg;
				break;

			case 'f':
				config.set_counter_font_path(optarg);
				break;

			case 'm':
				mins = atoi(optarg);
				config.set_end_time_secs(mins * 60);
				break;

			case 't':
				time_opt = optarg;
				config.set_end_time_str(time_opt);
				break;

			default:
				printUsage(argv);
				exit(EXIT_FAILURE);
		}
	}

	if (time_opt == NULL && mins == -1) {
		// no time given
		config.set_end_time_secs();
	}

#ifdef VERSION
	std::cout << "Meeting Countdown " << version << "\n";
#endif

	if (init(cfg_file)) {
		bool quit = false;

		SDL_Event e;

		while (!quit) {
			while (SDL_PollEvent(&e) != 0) {
				// user requests quit
				if (e.type == SDL_QUIT) {
					quit = true;
				}

				// handle window events
				main_window.HandleEvent(e);
			}

			// update window
			if (main_window.Render() < 0) {
				// finished or error
				quit = true;
			}
		}
	} else {
		std::cerr << "Initialization failed.\n";
	}

	// free resources and close SDL
	close();

	return 0;
}
