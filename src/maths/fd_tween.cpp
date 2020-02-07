#include "fd_tween.hpp"

#include <math.h>

#include <SDL_timer.h>

#include "../main/fd_handling.hpp"

FD_Tween::FD_Tween(double value) {
	this->set(value);
}
FD_Tween::~FD_Tween() {}

void FD_Tween::set(double value) {
	force_moved = true;
	last_checked = 0;
	tweenType = FD_TWEEN_LINEAR;
	start_time = 0;
	duration = 0;
	start_value = end_value = value;
	has_finished = false;
}
void FD_Tween::add(double value) {
	if (value == 0) return;
	start_value += value;
	end_value += value;
	force_moved = true;
	last_checked = getProgress();
}
void FD_Tween::move(FD_TweenType tweenType, double destination, Uint32 duration, Uint32 pause) {
	if (duration == 0) {
		set(destination); 
		return;
	}
	// Tween to the value from the current position
	start_value = this->value();
	force_moved = true;
	last_checked = 0;
	this->tweenType = tweenType;
	this->duration = duration;
	start_time = SDL_GetTicks() + pause;
	end_value = destination;
	has_finished = false;
}
void FD_Tween::apply(FD_TweenAction &action) {
	this->move(action.type, 
			   action.destination + action.uses * action.value_increment, 
			   action.duration, 
			   action.pause + action.uses * action.pause_increment);
	action.uses++;
}

double FD_Tween::value() {
	// Returns the value of the tween at this time
	Uint32 progress = getProgress();
	if (progress == 0) return start_value;
	switch (tweenType) {
	case FD_TWEEN_LINEAR: return linearTween(start_value, end_value, progress, duration);
	case FD_TWEEN_EASE_IN: return easeInTween(start_value, end_value, progress, duration);
	case FD_TWEEN_EASE_OUT: return easeOutTween(start_value, end_value, progress, duration);
	case FD_TWEEN_ELASTIC: return elasticTween(start_value, end_value, progress, duration);
	}
	FD_Handling::debug("Unhandled tween.");
	return 0;
}
double FD_Tween::destination() { return end_value; }

bool FD_Tween::moved() {
	bool moved{ false };
	if (force_moved) {
		force_moved = false;
		moved = true;
	} else {
		moved = getProgress() != last_checked;
	}
	last_checked = getProgress();
	return moved;
}
bool FD_Tween::going(double value) const { return end_value == value; }
bool FD_Tween::done() const { return SDL_GetTicks() - start_time >= duration; }
bool FD_Tween::finished() {
	if (has_finished || getProgress() != duration) return false;
	has_finished = true;
	return true;
}

Uint32 FD_Tween::getProgress() {
	int progress = static_cast<int>(SDL_GetTicks()) - start_time;
	if (progress <= 0) return 0;
	Uint32 uprogress = static_cast<Uint32>(progress);
	if (uprogress > duration) uprogress = duration;
	return uprogress;
}

// --- Each following function takes parameters: start, end, time, duration

double FD_Tween::linearTween(double s, double e, Uint32 t, Uint32 d) {
	return s + (static_cast<double>(t) / static_cast<double>(d)) * (e - s);
}
double FD_Tween::easeInTween(double s, double e, Uint32 t, Uint32 d) {
	return s + pow(static_cast<double>(t) / static_cast<double>(d), 3) * (e - s);
}
double FD_Tween::easeOutTween(double s, double e, Uint32 t, Uint32 d) {
	return s + (1 - pow(1 - (static_cast<double>(t) / static_cast<double>(d)), 3)) * (e - s);
}
double FD_Tween::elasticTween(double s, double e, Uint32 t, Uint32 d) {
	double p = 0.3;
	double inter = static_cast<double>(t) / static_cast<double>(d);
	return s + (pow(2, -10 * inter) * sin((inter - p / 4) * (2 * 3.14159) / p) + 1) * (e - s);
}