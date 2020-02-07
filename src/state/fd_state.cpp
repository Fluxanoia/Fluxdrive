#include "fd_state.hpp"

FD_State::FD_State(int id, std::weak_ptr<FD_Scene> scene) : id{ id }, scene{ scene },
object_list{ getObjectList(scene) }, input_list{ getInputList(scene) }{}
FD_State::~FD_State() {}

int FD_State::getInputList(std::weak_ptr<FD_Scene> s) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(s, scene, true);
	return scene->getInputManager()->generateSet();
}
int FD_State::getObjectList(std::weak_ptr<FD_Scene> s) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(s, scene, true);
	return scene->generateObjectList();
}

void FD_State::wake() {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	scene->setObjectList(object_list);
	scene->getIOManager()->getInputManager()->setInputSet(input_list);
}

void FD_State::resized() {}

bool FD_State::getNextState(int& id) {
	if (nextState == FD_State::INVALID_STATE) return false;
	id = nextState;
	nextState = FD_State::INVALID_STATE;
	return true;
}
bool FD_State::hasClosed() {
	return closed;
}

int FD_State::getID() const { return id; }
