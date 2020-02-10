#include "fd_eventListener.hpp"

FD_EventListener::FD_EventListener() {}
FD_EventListener::~FD_EventListener() {}

void FD_EventListener::pushEvent(const SDL_Event* e) {
	if (!accepting) return;
	SDL_Event our{ *e };
	queue.push_back(our);
}

void FD_EventListener::setAccepting(bool a) {
	accepting = a;
}

bool FD_EventListener::pullEvent(SDL_Event& e) {
	if (queue.size() != 0) {
		e = queue.front();
		queue.erase(queue.begin());
		return true;
	} else {
		return false;
	}
}

void FD_EventListener::clear() {
	queue.clear();
}