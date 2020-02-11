#include "fd_window.hpp"

FD_Window::FD_Window(std::string title, int width, int height,
	Uint32 win_flags, Uint32 ren_flags) {
	// Initialise the window
	window = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, win_flags);
	if (window == nullptr) {
		FD_Handling::errorSDL("A window could not be initialised.");
		FD_Handling::error("Fatal SDL error.", true);
	}
	FD_Handling::debug("SDL_Window initialised...");
	// Create the renderer
	renderer = SDL_CreateRenderer(window, -1, ren_flags);
	if (renderer == nullptr) {
		FD_Handling::errorSDL("A renderer could not be initialised.");
		FD_Handling::error("Fatal SDL error.", true);
	}
	FD_Handling::debug("SDL_Renderer initialised...");
	// Set flags
	if (win_flags & SDL_WINDOW_FULLSCREEN 
		|| win_flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
		fullscreen = true;
	} else {
		fullscreen = false;
	}
	SDL_GetWindowSize(window, &window_width, &window_height);
	apply();
}
FD_Window::~FD_Window() {}

void FD_Window::setDefaultHints() {
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
}

void FD_Window::pushEvent(SDL_WindowEvent e) {
	switch (e.event) {
	case SDL_WINDOWEVENT_RESTORED:
	case SDL_WINDOWEVENT_MAXIMIZED:
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		for (auto r : resizeables) {
			if (fullscreen) {
				r->resized(getScreenWidth(), getScreenHeight());
			} else {
				r->resized(window_width, window_height);
			}
		}
		break;
	}
}

void FD_Window::apply() {
	updated = true;
	if (fullscreen) {
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	} else {
		SDL_SetWindowFullscreen(window, 0);
		SDL_SetWindowSize(window, window_width, window_height);
		SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED);
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, nullptr);
	SDL_RenderPresent(renderer);
}

void FD_Window::addResizable(std::shared_ptr<FD_Resizable> resizeable) {
	this->resizeables.push_back(resizeable);
}

void FD_Window::setFullscreen() {
	this->fullscreen = true;
	apply();
}
void FD_Window::setWindowed() {
	this->fullscreen = false;
	apply();
}
void FD_Window::setResolution(int window_w, int window_h) {
	window_width = window_w;
	window_height = window_h;
	apply();
}
void FD_Window::setPosition(int x, int y) {
	SDL_SetWindowPosition(window, x, y);
}

bool FD_Window::hasUpdated() {
	if (updated) {
		updated = false;
		return true;
	}
	return false;
}

SDL_Renderer* FD_Window::getRenderer() const {
	return renderer;
}
bool FD_Window::isFullscreen() const {
	return fullscreen;
}
int FD_Window::getWindowedWidth() const {
	return window_width;
}
int FD_Window::getWindowedHeight() const {
	return window_height;
}
int FD_Window::getWidth() const {
	if (fullscreen) return getScreenWidth();
	return window_width;
}
int FD_Window::getHeight() const {
	if (fullscreen) return getScreenHeight();
	return window_height;
}
int FD_Window::getScreenWidth() const {
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	return dm.w;
}
int FD_Window::getScreenHeight() const {
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	return dm.h;
}