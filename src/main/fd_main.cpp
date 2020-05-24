#include "fd_main.hpp"

#ifdef FD_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "fd_handling.hpp"
#include "test/fd_testing.hpp"

bool FD_Main::initialise() {
	// Hide the console
#ifdef FD_DEBUG
	ShowWindow(GetConsoleWindow(), SW_SHOW);
#else
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
	// Start initialisation
	FD_Handling::debug("Fluxdrive - initialising...");
	// Initialise SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) < 0) {
		FD_Handling::errorSDL("SDL could not be initialised.");
		return false;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	FD_Handling::debug("SDL initialised...");
	// Initialise SDL mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		FD_Handling::errorSDL("SDL Mixer could not be initialised.");
		return false;
	}
	FD_Handling::debug("SDL_Mixer initialised...");
	// Initialise PNG loading
	int IMG_flags{ IMG_INIT_PNG || IMG_INIT_JPG };
	if ((IMG_Init(IMG_flags) & IMG_flags) != IMG_flags) {
		FD_Handling::errorIMG("SDL Image could not be initialised.");
		return false;
	}
	FD_Handling::debug("SDL_Image initialised...");
	// Initialise SDL TTF
	if (TTF_Init() == -1) {
		FD_Handling::errorTTF("SDL TTF could not be initialised.");
		return false;
	}
	FD_Handling::debug("SDL_TTF initialised...");
	SDL_StartTextInput();
	FD_Handling::debug("TextInput initialised...");
	return true;
}

void FD_Main::test() {
	FD_Handling::debug("Fluxdrive Tests - initialising...\n");
	FD_Testing::test();
	FD_Handling::debug("\nAll tests pass, resuming normal execution...\n");
}

void FD_Main::close() {
	FD_Handling::debug("Deallocating memory...");
	SDL_StopTextInput();
	TTF_Quit();
	Mix_Quit();
	Mix_CloseAudio();
	IMG_Quit();
	SDL_Quit();
	FD_Handling::debug("Fluxdrive closed.");
	_CrtDumpMemoryLeaks();
}

#ifdef FD_MAIN
int main() {
	FD_Main::initialise();
	FD_Main::test();
	FD_Main::close();
}
#endif
