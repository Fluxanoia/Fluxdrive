#include "fd_timer.hpp"

FD_Timer::FD_Timer() {}
FD_Timer::~FD_Timer() {}

void FD_Timer::start(Uint32 ms) {
	this->start_time = SDL_GetTicks();
	this->length = ms;
}

bool FD_Timer::finished() const {
	return this->progress() > length;
}

Uint32 FD_Timer::progress() const {
	return (SDL_GetTicks() - start_time);
}
