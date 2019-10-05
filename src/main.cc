#include <getopt.h>

#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "config.h"
#include "window.h"

Config config;
Window main_window;

const char* cfg_file = NULL;


bool Init(const char *cfg_file) {
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

void Close() {
	// destroy window
	main_window.Free();

	TTF_Quit();
	SDL_Quit();
}

void PrintUsage(char* argv[]) {
	std::string cmd = argv[0];
	std::string usg = "Usage: " + cmd + " [OPTIONS]...\n";
	usg += "\n";
	usg += "  -i, --ini=INI_FILE\n";
	usg += "      --font=FONT_FILE\n";
	usg += "      --font-size=FONT_SIZE\n";
	usg += "  -m, --min=MINUTES\n";
	usg += "  -s, --size=SIZE\n";
	usg += "  -t, --time=HH:MM\n";
	std::cerr << usg;
}

int ParseArgs(int argc, char* argv[]) {
	static struct option long_options[] = {
		{"ini",       required_argument, 0, 'i'},
		{"font",      required_argument, 0, 'f'},
		{"font-size", required_argument, 0, 'x'},
		{"min",       required_argument, 0, 'm'},
		{"size",      required_argument, 0, 's'},
		{"time",      required_argument, 0, 't'},
		{0,           0,                 0, 0},
	};

	int c;
	int mins = -1;
	int size;
	const char* time_opt = NULL;
	while (true) {
		int option_index = 0;
		c = getopt_long(argc, argv, "i:m:s:t:", long_options, &option_index);
		if (c == -1) {
			break;
		}
		switch (c) {
			case 'i':
				cfg_file = optarg;
				break;

			case 'f':
			  config.set_counter_font_path(optarg);
				break;

			case 'm':
				mins = atoi(optarg);
				config.set_end_time_secs(mins * 60);
				break;

			case 's':
			  size = atoi(optarg);
				config.set_width(size);
				config.set_height(size);
				break;

			case 't':
				time_opt = optarg;
				config.set_end_time_str(time_opt);
				break;

			case 'x':
			  size = atoi(optarg);
			  config.set_counter_font_size(size);
				break;

			default:
				PrintUsage(argv);
				exit(EXIT_FAILURE);
		}
	}

	if (time_opt == NULL && mins == -1) {
		// no time given
		config.set_end_time_secs();
	}
}

int main(int argc, char* argv[]) {
	ParseArgs(argc, argv);

	std::string txt = "Meeting Countdown";
#ifdef VERSION
  txt += " ";
	txt += VERSION;
#endif
  txt += " https://github.com/iqqmuT/meeting-countdown\n";
  std::cout << txt;

	if (Init(cfg_file)) {
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
	Close();

	return 0;
}
