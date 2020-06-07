#include "fd_stateManager.hpp"

#include "../main/fd_handling.hpp"

FD_StateManager::FD_StateManager(std::weak_ptr<FD_Scene> scene) : scene{ scene } {}

FD_StateManager::~FD_StateManager() {
	FD_Handling::debug("FD_StateManager destroyed.");
}

void FD_StateManager::logState(std::weak_ptr<FD_State> s) {
	std::shared_ptr<FD_State> state;
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(s, state, true);
	FD_Handling::lock(this->scene, scene, true);
	this->states.push_back(state);
	scene->getWindow()->addResizable(state);
	if (states.size() == 1) currentState = state->getID();
}

void FD_StateManager::logEventListener(std::weak_ptr<FD_EventListener> el) {
	this->event_list.push_back(el);
}

void FD_StateManager::setState(int id) {
	if (currentState != FD_State::INVALID_STATE) {
		std::shared_ptr<FD_State> state;
		FD_Handling::lock(states.at(currentState), state, true);
		state->sleep();
	}
	currentState = id;
	if (currentState != FD_State::INVALID_STATE) {
		std::shared_ptr<FD_State> state;
		FD_Handling::lock(states.at(currentState), state, true);
		state->wake();
	}
}

void FD_StateManager::update() {
	if (currentState == FD_State::INVALID_STATE) return;
	int id;
	std::weak_ptr<FD_State> state = states.at(currentState);
	if (auto s = state.lock()) {
		s->update();
		if (s->hasClosed()) {
			this->closed = true;
		} else if (s->getNextState(id)) {
			this->setState(id);
		}
	} else {
		this->closed = true;
	}
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	scene->update();
}

void FD_StateManager::render() {
	if (currentState == FD_State::INVALID_STATE) return;
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	scene->render();
}

void FD_StateManager::pushEvent(const SDL_Event* e) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	scene->pushEvent(e);
	auto it = event_list.begin();
	while (it != event_list.end()) {
		if (auto el = (*it).lock()) {
			el->pushEvent(e);
			it++;
		} else {
			it = event_list.erase(it);
		}
	}
}
