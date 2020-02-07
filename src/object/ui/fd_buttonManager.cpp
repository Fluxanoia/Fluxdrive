#include "fd_buttonManager.hpp"

#include "../../main/fd_handling.hpp"

FD_ButtonManager::FD_ButtonManager(std::weak_ptr<FD_Scene> s,
	std::weak_ptr<FD_CameraSet> cameras,
	int inputList)
	: scene{ s }, cameras{ cameras } {
	// Get the parent set
	std::shared_ptr<FD_Scene> scene;
	std::shared_ptr<FD_InputSet> parent_set;
	FD_Handling::lock(s, scene, true);
	FD_Handling::lock(scene->getInputManager()->getInputSet(inputList),
		parent_set, true);
	inputSet = parent_set->generateSharedSet();
}
FD_ButtonManager::~FD_ButtonManager() {
	buttons.clear();
	events.clear();
}

FD_ButtonActivity FD_ButtonManager::prepareActivity() const {
	FD_ButtonActivity a{};
	std::shared_ptr<FD_InputSet> inputSet;
	std::shared_ptr<FD_CameraSet> cameras;
	std::shared_ptr<FD_Camera> camera;
	FD_Handling::lock(this->inputSet, inputSet, true);
	FD_Handling::lock(this->cameras, cameras, true);
	FD_Handling::lock(cameras->getCurrentCamera(), camera, true);
	if (this->controlMethod == MOUSE && this->mouse_activated) {
		a.mouse = true;
		a.x = a.camera_x = inputSet->getMouseX();
		a.y = a.camera_y = inputSet->getMouseY();
		if (auto scene = this->scene.lock()) {
			camera->toCameraCoordinates(a.camera_x, a.camera_y);
		} else {
			FD_Handling::error("The scene was lost.", true);
		}
	}
	return a;
}
void FD_ButtonManager::update() {
	std::shared_ptr<FD_InputSet> inputSet;
	FD_Handling::lock(this->inputSet, inputSet, true);
	if (!active) {
		inputSet->empty();
		return;
	}
	// Update buttons
	if (inputSet->mouseMoved()) this->controlMethod = MOUSE;
	FD_ButtonActivity a{ prepareActivity() };
	bool moused_on{ false };
	size_t index{ 0 }, next_selected{ selected };
	for (std::shared_ptr<FD_Button> button : buttons) {
		if (controlMethod == DIRECTIONAL) {
			a.selected = (this->selected == index);
		}
		button->update(a);
		if (a.mouse && button->isSelected()) {
			next_selected = index;
			moused_on = true;
		}
		index++;
	}
	selected = next_selected;
	if (buttons.size() == 0) return;
	// React to maps
	FD_InputEvent e;
	std::shared_ptr<FD_Button> button;
	while (inputSet->getEvent(e)) {
		switch (e.code) {
		case FD_BUTTON_UP:
			if (controlMethod != DIRECTIONAL) {
				controlMethod = DIRECTIONAL;
				if (!moused_on) break;
			}
			if (buttons.size() != 0) {
				button = buttons.at(selected);
				if (button->isEntered()) {
					button->buttonUp();
				} else {
					if (selected == 0) selected = buttons.size();
					selected--;
				}
			}
			break;
		case FD_BUTTON_DOWN:
			if (controlMethod != DIRECTIONAL) {
				controlMethod = DIRECTIONAL;
				if (!moused_on) break;
			}
			if (buttons.size() != 0) {
				button = buttons.at(selected);
				if (button->isEntered()) {
					button->buttonDown();
				} else {
					selected++;
					selected %= buttons.size();
				}
			}
			break;
		case FD_BUTTON_MOUSE_PRESS:
			buttons.at(selected)->press();
			break;
		case FD_BUTTON_MOUSE_RELEASE:
			button = buttons.at(selected);
			if (button->release()) events.push_back(button->getCode());
			break;
		case FD_BUTTON_OTHER_PRESS:
			buttons.at(selected)->press();
			break;
		case FD_BUTTON_OTHER_RELEASE:
			button = buttons.at(selected);
			if (button->release()) events.push_back(button->getCode());
			break;
		case FD_BUTTON_ENTER:
			button = buttons.at(selected);
			button->enter();
			break;
		case FD_BUTTON_EXIT:
			button = buttons.at(selected);
			button->exit();
			break;
		}
	}
}
void FD_ButtonManager::reset() {
	for (std::shared_ptr<FD_Button> button : buttons) button->reset();
}

bool FD_ButtonManager::getEvent(int& code) {
	if (!active) return false;
	if (events.size() == 0) return false;
	code = events.back();
	events.pop_back();
	return true;
}

// Button appending

void FD_ButtonManager::addButton(std::shared_ptr<FD_ObjectGroup> group,
	std::shared_ptr<FD_Button> button) {
	buttons.push_back(button);
	button->assimilate(group);
}

void FD_ButtonManager::addBasicButton(const FD_ButtonTemplate& temp,
	const int x, const int y,
	const int code,
	const std::string text,
	const std::shared_ptr<FD_Font> font,
	const SDL_Colour colour) {
	std::shared_ptr<FD_Scene> scene;
	std::shared_ptr<FD_TextImage> image;
	std::shared_ptr<FD_ObjectGroup> group;
	if (!FD_Handling::lock(temp.group, group)) return;
	if (!FD_Handling::lock(this->scene, scene)) return;
	if (!FD_Handling::lock(scene->getImageManager()->
		loadImage(font, text, colour), image)) return;
	std::shared_ptr<FD_BasicButton> button
		= std::make_shared<FD_BasicButton>(temp, x, y, code, image);
	buttons.push_back(button);
	button->assimilate(group);
}

void FD_ButtonManager::addDropdownButton(const FD_ButtonTemplate& temp,
	const int x, const int y,
	const std::vector<int> codes,
	const std::vector<std::string> texts,
	const std::shared_ptr<FD_Font> font,
	const SDL_Colour colour,
	const size_t preselection) {
	std::shared_ptr<FD_Scene> scene;
	std::vector<std::weak_ptr<FD_Image>> images;
	std::shared_ptr<FD_ObjectGroup> group;
	if (!FD_Handling::lock(temp.group, group)) return;
	if (!FD_Handling::lock(this->scene, scene)) return;
	for (auto s : texts) {
		images.push_back(scene->getImageManager()->
			loadImage(font, s, colour));
	}
	std::shared_ptr<FD_DropdownButton> button
		= std::make_shared<FD_DropdownButton>(temp, x, y, codes,
			images, preselection);
	buttons.push_back(button);
	button->assimilate(group);
}

bool FD_ButtonManager::mouseSelected() const {
	FD_ButtonActivity a{ prepareActivity() };
	for (std::shared_ptr<FD_Button> button : buttons) {
		if (button->mouseSelected(a)) return true;
	}
	return false;
}

void FD_ButtonManager::setActive(bool active) {
	if (active != this->active) reset();
	this->active = active;
}
void FD_ButtonManager::setMouseActivated(bool mouse_activated) {
	this->mouse_activated = mouse_activated;
}

std::shared_ptr<FD_InputSet> FD_ButtonManager::getInputSet() {
	std::shared_ptr<FD_InputSet> inputSet;
	FD_Handling::lock(this->inputSet, inputSet, true);
	return inputSet;
}
void FD_ButtonManager::addDefaultMaps() {
	addDefaultMouseMaps();
	addDefaultKeyboardMaps();
	addDefaultJoystickMaps();
}
void FD_ButtonManager::addDefaultMouseMaps() {
	std::shared_ptr<FD_InputSet> inputSet;
	FD_Handling::lock(this->inputSet, inputSet, true);
	this->mouse_activated = true;
	inputSet->addMouseButtonMap(FD_MAP_PRESSED, SDL_BUTTON_LEFT, FD_BUTTON_MOUSE_PRESS);
	inputSet->addMouseButtonMap(FD_MAP_RELEASED, SDL_BUTTON_LEFT, FD_BUTTON_MOUSE_RELEASE);
}
void FD_ButtonManager::addDefaultKeyboardMaps() {
	std::shared_ptr<FD_InputSet> inputSet;
	FD_Handling::lock(this->inputSet, inputSet, true);
	inputSet->addKeyMap(FD_MAP_RELEASED, SDLK_w, FD_BUTTON_UP);
	inputSet->addKeyMap(FD_MAP_RELEASED, SDLK_s, FD_BUTTON_DOWN);
	inputSet->addKeyMap(FD_MAP_RELEASED, SDLK_a, FD_BUTTON_EXIT);
	inputSet->addKeyMap(FD_MAP_RELEASED, SDLK_d, FD_BUTTON_ENTER);
	inputSet->addKeyMap(FD_MAP_RELEASED, SDLK_UP, FD_BUTTON_UP);
	inputSet->addKeyMap(FD_MAP_RELEASED, SDLK_DOWN, FD_BUTTON_DOWN);
	inputSet->addKeyMap(FD_MAP_RELEASED, SDLK_LEFT, FD_BUTTON_EXIT);
	inputSet->addKeyMap(FD_MAP_RELEASED, SDLK_RIGHT, FD_BUTTON_ENTER);
	inputSet->addKeyMap(FD_MAP_PRESSED, SDLK_RETURN, FD_BUTTON_OTHER_PRESS);
	inputSet->addKeyMap(FD_MAP_RELEASED, SDLK_RETURN, FD_BUTTON_OTHER_RELEASE);
}
void FD_ButtonManager::addDefaultJoystickMaps() {
	std::shared_ptr<FD_InputSet> inputSet;
	FD_Handling::lock(this->inputSet, inputSet, true);
	inputSet->addJoystickDpadMap(FD_MAP_RELEASED, FD_ALL_JOYSTICKS, SDL_HAT_UP, FD_BUTTON_UP);
	inputSet->addJoystickDpadMap(FD_MAP_RELEASED, FD_ALL_JOYSTICKS, SDL_HAT_DOWN, FD_BUTTON_DOWN);
	inputSet->addJoystickDpadMap(FD_MAP_RELEASED, FD_ALL_JOYSTICKS, SDL_HAT_LEFT, FD_BUTTON_EXIT);
	inputSet->addJoystickDpadMap(FD_MAP_RELEASED, FD_ALL_JOYSTICKS, SDL_HAT_RIGHT, FD_BUTTON_ENTER);
	inputSet->addJoystickButtonMap(FD_MAP_PRESSED, FD_ALL_JOYSTICKS, SDL_CONTROLLER_BUTTON_A, FD_BUTTON_OTHER_PRESS);
	inputSet->addJoystickButtonMap(FD_MAP_RELEASED, FD_ALL_JOYSTICKS, SDL_CONTROLLER_BUTTON_A, FD_BUTTON_OTHER_RELEASE);
	inputSet->addJoystickAxisMap(FD_ALL_JOYSTICKS, FD_LEFT_Y_DOWN, FD_BUTTON_DOWN, 10);
	inputSet->addJoystickAxisMap(FD_ALL_JOYSTICKS, FD_LEFT_Y_UP, FD_BUTTON_UP, 10);
	inputSet->addJoystickAxisMap(FD_ALL_JOYSTICKS, FD_LEFT_X_LEFT, FD_BUTTON_EXIT, 10);
	inputSet->addJoystickAxisMap(FD_ALL_JOYSTICKS, FD_LEFT_X_RIGHT, FD_BUTTON_ENTER, 10);
}

void FD_ButtonManager::applyTweenX(FD_TweenAction& action) {
	for (auto b : buttons) b->getTweenX()->apply(action);
}
void FD_ButtonManager::applyTweenY(FD_TweenAction& action) {
	for (auto b : buttons) b->getTweenY()->apply(action);
}
void FD_ButtonManager::applyTweenWidth(FD_TweenAction& action) {
	for (auto b : buttons) b->getTweenWidth()->apply(action);
}
void FD_ButtonManager::applyTweenHeight(FD_TweenAction& action) {
	for (auto b : buttons) b->getTweenHeight()->apply(action);
}
void FD_ButtonManager::applyTweenOpacity(FD_TweenAction& action) {
	for (auto b : buttons) b->getTweenOpacity()->apply(action);
}
