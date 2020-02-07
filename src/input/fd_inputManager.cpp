#include "fd_inputManager.hpp"

#include <cmath>
#include <string>

#include "fd_input.hpp"
#include "../maths/fd_maths.hpp"
#include "../main/fd_handling.hpp"

// Input Manager Member Functions

FD_InputManager::FD_InputManager() { }
FD_InputManager::~FD_InputManager() {
	// Deallocate all joysticks
	for (auto j : joysticks) SDL_JoystickClose(j.second);
	for (auto h : haptics) SDL_HapticClose(h.second);
	joysticks.clear();
	haptics.clear();
	// Clear all vectors
	axis_values.clear();
	pressed.clear();
	held.clear();
	released.clear();
	analog.clear();
	other.clear();
	maps.clear();
	FD_Handling::debug("FD_InputManager destroyed.");
}

void FD_InputManager::update() {
	// Alert the current set of maps to the inputs
	if (auto set = getInputSet().lock()) {
		for (std::shared_ptr<FD_Input> i : pressed) set->call(FD_MAP_PRESSED, i);
		for (std::shared_ptr<FD_Input> i : held) set->call(FD_MAP_HELD, i);
		for (std::shared_ptr<FD_Input> i : released) set->call(FD_MAP_RELEASED, i);
		for (std::shared_ptr<FD_Input> i : analog) set->call(FD_MAP_ANALOG, i);
		for (std::shared_ptr<FD_Input> i : other) set->call(FD_MAP_OTHER, i);
		set->update();
	}
	// Clear vectors for inputs that only last a single update
	pressed.clear();
	other.clear();
	released.clear();
}

void FD_InputManager::pushMouseMotionEvent(const SDL_MouseMotionEvent* e) {
	mouse_x = e->x;
	mouse_y = e->y;
	updateDevice(FD_DEVICE_MOUSE);
	if (auto set = getInputSet().lock()) set->updateMouse(mouse_x, mouse_y);
}
void FD_InputManager::pushKeyboardEvent(const SDL_KeyboardEvent* e) {
	updateDevice(FD_DEVICE_KEYBOARD);
	std::shared_ptr<FD_KeyInput> i = std::make_shared<FD_KeyInput>(e->keysym.sym);
	switch (e->type) {
	case SDL_KEYDOWN:
		if (!isHeld(i)) {
			pressed.insert(pressed.begin(), i);
			this->addHeldInput(i);
		}
		break;
	case SDL_KEYUP:
		this->removeHeldInput(i);
		released.insert(released.begin(), i);
		break;
	}
}
void FD_InputManager::pushMouseButtonEvent(const SDL_MouseButtonEvent* e) {
	updateDevice(FD_DEVICE_MOUSE);
	std::shared_ptr<FD_MouseButtonInput> i = std::make_shared<FD_MouseButtonInput>(e->button);
	switch (e->type) {
	case SDL_MOUSEBUTTONDOWN:
		pressed.insert(pressed.begin(), i);
		this->addHeldInput(i);
		break;
	case SDL_MOUSEBUTTONUP:
		this->removeHeldInput(i);
		released.insert(released.begin(), i);
		break;
	}
}
void FD_InputManager::pushMouseWheelEvent(const SDL_MouseWheelEvent* e) {
	if (e->y == 0) return;
	updateDevice(FD_DEVICE_MOUSE);
	std::shared_ptr<FD_MouseWheelInput> i
		= std::make_shared<FD_MouseWheelInput>((e->y < 0) ? FD_SCROLL_DOWN : FD_SCROLL_UP);
	other.insert(other.begin(), i);
}
void FD_InputManager::pushJoyButtonEvent(const SDL_JoyButtonEvent* e) {
	updateDevice(FD_DEVICE_JOYSTICK_BUTTON, e->which);
	std::shared_ptr<FD_JoystickButtonInput> i
		= std::make_shared<FD_JoystickButtonInput>(e->which, e->button);
	switch (e->type) {
	case SDL_JOYBUTTONDOWN:
		if (!isHeld(i)) {
			pressed.insert(pressed.begin(), i);
			this->addHeldInput(i);
		}
		break;
	case SDL_JOYBUTTONUP:
		this->removeHeldInput(i);
		released.insert(released.begin(), i);
		break;
	}
}
void FD_InputManager::pushJoyDpadEvent(const SDL_JoyHatEvent* e) {
	updateDevice(FD_DEVICE_JOYSTICK_BUTTON, e->which);
	this->removeHeldDpadInput(e->which);
	if (e->value == SDL_HAT_CENTERED) return;
	std::shared_ptr<FD_JoystickDpadInput> i
		= std::make_shared<FD_JoystickDpadInput>(e->which, e->value);
	pressed.insert(pressed.begin(), i);
	this->addHeldInput(i);
}
void FD_InputManager::pushJoyAxisEvent(const SDL_JoyAxisEvent* e) {
	// The set of altered axes
	std::vector<FD_ControllerAxis> changed{};
	FD_ControllerAxis other, axis, subaxis = FD_ALL_AXES;
	Sint16 value = e->value;
	// Translate the axis to the public enum
	switch (e->axis) {
	case AXIS_LEFT_X:
		subaxis = FD_AXIS_LEFT;
		axis = FD_LEFT_X_RIGHT;
		break;
	case AXIS_LEFT_Y:
		subaxis = FD_AXIS_LEFT;
		axis = FD_LEFT_Y_DOWN;
		break;
	case AXIS_RIGHT_X:
		subaxis = FD_AXIS_RIGHT;
		axis = FD_RIGHT_X_RIGHT;
		break;
	case AXIS_RIGHT_Y:
		subaxis = FD_AXIS_RIGHT;
		axis = FD_RIGHT_Y_DOWN;
		break;
	case AXIS_LEFT_TRIGGER:
		axis = FD_LEFT_TRIGGER;
		break;
	case AXIS_RIGHT_TRIGGER:
		axis = FD_RIGHT_TRIGGER;
		break;
	default: return;
	}
	switch (e->axis) {
	case AXIS_LEFT_X:
	case AXIS_LEFT_Y:
	case AXIS_RIGHT_X:
	case AXIS_RIGHT_Y:
		// This would be the other side of this stick's axis
		other = static_cast<FD_ControllerAxis>(axis - 1);
		// Reset for both of them
		this->removeHeldAxisInput(e->which, axis);
		this->removeHeldAxisInput(e->which, other);
		// If the value is negative, reset the axis and switch
		// If postive, reset the other axis
		if (value < 0) {
			value++; value *= -1;
			axis_values[e->which][axis] = 0;
			changed.push_back(axis);
			axis = other;
		} else {
			axis_values[e->which][other] = 0;
			changed.push_back(other);
		}
		break;
	case AXIS_LEFT_TRIGGER:
	case AXIS_RIGHT_TRIGGER:
		// Change the trigger value to be only positive
		value = static_cast<Uint16>(
			(static_cast<double>(value) - static_cast<double>(SDL_JOYSTICK_AXIS_MIN)) / 2.0
			);
		this->removeHeldAxisInput(e->which, axis);
		break;
	}
	// Update the last device
	updateDevice(FD_DEVICE_JOYSTICK_AXIS, e->which);
	// Update the main axis value
	axis_values[e->which][axis] = value;
	// If the left stick has been altered, update it
	if (subaxis == FD_AXIS_LEFT) {
		this->removeHeldAxisInput(e->which, subaxis);
		Uint32 left_x = axis_values[e->which][FD_LEFT_X_LEFT] + axis_values[e->which][FD_LEFT_X_RIGHT];
		Uint32 left_y = axis_values[e->which][FD_LEFT_Y_UP] + axis_values[e->which][FD_LEFT_Y_DOWN];
		axis_values[e->which][FD_AXIS_LEFT] = static_cast<Sint16>(std::sqrt(left_x * left_x + left_y * left_y));
		changed.push_back(subaxis);
	}
	// If the right stick has been altered, update it
	if (subaxis == FD_AXIS_RIGHT) {
		this->removeHeldAxisInput(e->which, subaxis);
		Uint32 right_x = axis_values[e->which][FD_RIGHT_X_LEFT] + axis_values[e->which][FD_RIGHT_X_RIGHT];
		Uint32 right_y = axis_values[e->which][FD_RIGHT_Y_UP] + axis_values[e->which][FD_RIGHT_Y_DOWN];
		axis_values[e->which][FD_AXIS_RIGHT] = static_cast<Sint16>(std::sqrt(right_x * right_x + right_y * right_y));
		changed.push_back(subaxis);

	}
	// Update the input set with everything that has changed
	changed.push_back(axis);
	if (auto set = getInputSet().lock()) {
		for (FD_ControllerAxis a : changed) set->updateAxis(e->which, a, axis_values[e->which][a]);
	}
	// Add the stick input if needed
	Uint16 dead_zone;
	DEAD_ZONE(dead_zone);
	if (subaxis != FD_ALL_AXES && axis_values[e->which][subaxis] >= dead_zone) {
		analog.push_back(std::make_shared<FD_AnalogInput>(e->which, subaxis));
	}
	// Add the axis input if needed
	if (axis_values[e->which][axis] >= dead_zone) {
		analog.push_back(std::make_shared<FD_AnalogInput>(e->which, axis));
	}
}
void FD_InputManager::pushJoyDeviceEvent(const SDL_JoyDeviceEvent* e) {
	switch (e->type) {
	case SDL_JOYDEVICEADDED:
	{
		SDL_Joystick* j = SDL_JoystickOpen(e->which);
		joysticks[SDL_JoystickInstanceID(j)] = j;
		SDL_Haptic* h = SDL_HapticOpenFromJoystick(j);
		if (h == nullptr) break;
		if (SDL_HapticRumbleInit(h) != 0) {
			SDL_HapticClose(h);
			break;
		}
		haptics[SDL_JoystickInstanceID(j)] = h;
	}
	break;
	case SDL_JOYDEVICEREMOVED:
		this->removeHeldDpadInput(e->which);
		this->removeHeldAxisInput(e->which, FD_ALL_AXES);
		this->removeHeldInput(
			std::make_shared<FD_JoystickButtonInput>(e->which, FD_ALL_JOYSTICK_BUTTONS));
		auto jit = joysticks.begin();
		while (jit != joysticks.end()) {
			if ((*jit).first == e->which) {
				SDL_JoystickClose(jit->second);
				joysticks.erase(jit);
				break;
			}
			jit++;
		}
		auto hit = haptics.begin();
		while (hit != haptics.end()) {
			if ((*hit).first == e->which) {
				SDL_HapticClose(hit->second);
				haptics.erase(hit);
				break;
			}
			hit++;
		}
		break;
	}
}

void FD_InputManager::setInputSet(const int id) {
	// Check the id bounds
	if (id <= 0) return;
	if (id > idCount) return;
	// Reset the current set
	if (auto set = getInputSet().lock()) set->reset();
	// Clear the key lists
	this->pressed.clear();
	this->held.clear();
	this->released.clear();
	this->analog.clear();
	this->other.clear();
	// Set the id
	this->currentInputSet = id;
	if (auto set = getInputSet().lock()) {
		set->updateMouse(mouse_x, mouse_y);
		FD_ControllerAxis a;
		for (auto joy : axis_values) {
			for (int i = 0; i < FD_AXIS_COUNT; i++) {
				a = static_cast<FD_ControllerAxis>(i);
				set->updateAxis(joy.first, a, axis_values[joy.first][a]);
			}
		}
		set->updateDevice(lastDevice, lastJoystick);
	}
}
int FD_InputManager::generateSet() {
	this->maps.push_back(std::make_shared<FD_InputSet>(++idCount));
	return idCount;
}

bool FD_InputManager::isHeld(const std::shared_ptr<FD_Input> input) {
	for (std::shared_ptr<FD_Input> i : held) if (i->verify(*input)) return true;
	return false;
}
void FD_InputManager::addHeldInput(std::shared_ptr<FD_Input> input) {
	if (!isHeld(input)) held.insert(held.begin(), input);
}
void FD_InputManager::removeHeldInput(std::shared_ptr<FD_Input> input) {
	// Remove the instances of a held input from the list
	auto it = held.begin();
	while (it != held.end()) {
		if ((*it)->verify(*input)) {
			it = held.erase(it);
		} else {
			it++;
		}
	}
}
void FD_InputManager::removeHeldDpadInput(SDL_JoystickID id) {
	// Remove the instance of a held dpad from a specific joystick
	FD_JoystickDpadInput* input = new FD_JoystickDpadInput(id, -1);
	auto it = held.begin();
	while (it != held.end()) {
		if ((*it)->verify(*input)) {
			released.insert(released.begin(), (*it));
			held.erase(it);
			break;
		} else {
			it++;
		}
	}
	delete input;
}
void FD_InputManager::removeHeldAxisInput(SDL_JoystickID id, FD_ControllerAxis axis) {
	// Remove the instance of an axis from a specific joystick
	FD_AnalogInput* input = new FD_AnalogInput(id, axis);
	auto it = analog.begin();
	while (it != analog.end()) {
		if ((*it)->verify(*input)) {
			it = analog.erase(it);
		} else {
			it++;
		}
	}
	delete input;
}

void FD_InputManager::updateDevice(FD_Device device, SDL_JoystickID id) {
	lastDevice = device;
	if (lastDevice == FD_DEVICE_JOYSTICK_BUTTON || lastDevice == FD_DEVICE_JOYSTICK_AXIS) lastJoystick = id;
	if (auto set = getInputSet().lock()) set->updateDevice(lastDevice, lastJoystick);
}

std::weak_ptr<FD_InputSet> FD_InputManager::getInputSet() const {
	return getInputSet(currentInputSet);
}
std::weak_ptr<FD_InputSet> FD_InputManager::getInputSet(const int id) const {
	for (std::shared_ptr<FD_InputSet> i : maps) if (i->getID() == id) return i;
	return std::make_shared<FD_InputSet>(-1);
}

int FD_InputManager::getMouseX() const {
	return mouse_x;
}
int FD_InputManager::getMouseY() const {
	return mouse_y;
}
double FD_InputManager::getAxisValue(SDL_JoystickID id, FD_ControllerAxis a) const {
	if (axis_values.find(id) != axis_values.end()) {
		if (axis_values.at(id).find(a) != axis_values.at(id).end()) {
			double power = static_cast<double>(axis_values.at(id).at(a))
				/ static_cast<double>(SDL_JOYSTICK_AXIS_MAX);
			if (power > 1) power = 1;
			if (power < 0) power = 0;
			return power;
		}
	}
	return 0;
}
FD_Device FD_InputManager::getLastDevice() const {
	return lastDevice;
}
SDL_JoystickID FD_InputManager::getLastJoystick() const {
	return lastJoystick;
}

void FD_InputManager::hapticFeedback(SDL_JoystickID id, float power, Uint32 duration) {
	for (auto h : haptics) {
		if (id == FD_ALL_JOYSTICKS || h.first == id) {
			SDL_HapticRumblePlay(h.second, power, duration);
		}
	}
}

// Input Set Member Functions

FD_InputSet::FD_InputSet(const int id) : id{ id } { }
FD_InputSet::~FD_InputSet() {
	event_queue.clear();
	maps.clear();
	axis_values.clear();
	axis_moved.clear();
	shared_sets.clear();
}

void FD_InputSet::update() {
	for (auto it = maps.begin(); it != maps.end(); it++) (*it)->update();
	for (std::shared_ptr<FD_InputSet> set : shared_sets) set->update();
}
void FD_InputSet::clear() {
	maps.clear();
	event_queue.clear();
	for (std::shared_ptr<FD_InputSet> set : shared_sets) set->clear();
}
void FD_InputSet::reset() {
	for (auto it = maps.begin(); it != maps.end(); it++) (*it)->reset();
	event_queue.clear();
	for (std::shared_ptr<FD_InputSet> set : shared_sets) set->reset();
}
void FD_InputSet::empty() {
	event_queue.clear();
}

void FD_InputSet::call(const FD_MapType t, std::shared_ptr<FD_Input> input) {
	for (auto it = maps.begin(); it != maps.end(); it++) {
		if ((*it)->call(t, input)) {
			event_queue.insert(event_queue.begin(), (*it)->getEvent());
		}
	}
	for (std::shared_ptr<FD_InputSet> set : shared_sets) set->call(t, input);
}

bool FD_InputSet::getEvent(FD_InputEvent& code) {
	if (event_queue.size() == 0) return false;
	code = event_queue.back();
	event_queue.pop_back();
	return true;
}

void FD_InputSet::updateMouse(int mouse_x, int mouse_y) {
	this->mouse_x = mouse_x;
	this->mouse_y = mouse_y;
	mouse_moved = true;
	for (std::shared_ptr<FD_InputSet> set : shared_sets) set->updateMouse(mouse_x, mouse_y);
}
void FD_InputSet::updateAxis(SDL_JoystickID id, FD_ControllerAxis axis, Uint16 value) {
	if (axis_values.find(id) == axis_values.end()) {
		axis_values.insert(std::make_pair(id, std::unordered_map<FD_ControllerAxis, Uint16>()));
	}
	axis_values.at(id).insert_or_assign(axis, value);
	if (axis_moved.find(id) == axis_moved.end()) {
		axis_moved.insert(std::make_pair(id, std::unordered_map<FD_ControllerAxis, bool>()));
	}
	axis_moved.at(id).insert_or_assign(axis, true);
	for (std::shared_ptr<FD_InputSet> set : shared_sets) set->updateAxis(id, axis, value);
}
void FD_InputSet::updateDevice(FD_Device device, SDL_JoystickID id) {
	lastDevice = device;
	if (device == FD_DEVICE_JOYSTICK_BUTTON || device == FD_DEVICE_JOYSTICK_AXIS) lastJoystick = id;
	for (std::shared_ptr<FD_InputSet> set : shared_sets) set->updateDevice(lastDevice, lastJoystick);
}

void FD_InputSet::addMap
(const FD_MapType t, const std::shared_ptr<FD_Input> input,
	const int map_code, const Uint16 pause) {
	// Check if that key and type is already bound
	for (auto it = this->maps.begin(); it != this->maps.end(); it++) {
		if (((*it)->getInput()->verify(*input)) && ((*it)->getType() == t)) return;
	}
	// Add the map to the list
	this->maps.push_back(std::make_unique<FD_InputSet::FD_InputMap>(t, input, map_code, pause));
}
void FD_InputSet::addJoystickAxisMap(const SDL_JoystickID id, const FD_ControllerAxis axis,
	const int map_code, const Uint16 pause) {
	this->addMap(FD_MAP_ANALOG, std::make_shared<FD_AnalogInput>(id, axis), map_code, pause);
}
void FD_InputSet::addKeyMap(const FD_MapType t,
	const SDL_Keycode k, const int map_code, const Uint16 pause) {
	this->addMap(t, std::make_shared<FD_KeyInput>(k), map_code, pause);
}
void FD_InputSet::addMouseButtonMap(const FD_MapType t,
	const Uint8 b, const int map_code, const Uint16 pause) {
	this->addMap(t, std::make_shared<FD_MouseButtonInput>(b), map_code, pause);
}
void FD_InputSet::addMouseWheelMap(const FD_ScrollDirection d, const int map_code, const Uint16 pause) {
	this->addMap(FD_MAP_OTHER, std::make_shared<FD_MouseWheelInput>(d), map_code, pause);
}
void FD_InputSet::addJoystickButtonMap(const FD_MapType t, const SDL_JoystickID id,
	const Uint8 b, const int map_code, const Uint16 pause) {
	this->addMap(t, std::make_shared<FD_JoystickButtonInput>(id, b), map_code, pause);
}
void FD_InputSet::addJoystickDpadMap(const FD_MapType t, const SDL_JoystickID id,
	const Uint8 b, const int map_code, const Uint16 pause) {
	this->addMap(t, std::make_shared<FD_JoystickDpadInput>(id, b), map_code, pause);
}

bool FD_InputSet::removeMap(const FD_MapType t, const std::shared_ptr<FD_Input> input) {
	// Remove a certain map from the set
	for (auto it = this->maps.begin(); it != this->maps.end(); it++) {
		if (((*it)->getInput()->verify(*input)) && ((*it)->getType() == t)) {
			this->maps.erase(it);
			return true;
		}
	}
	return false;
}

std::shared_ptr<FD_InputSet> FD_InputSet::generateSharedSet() {
	std::shared_ptr<FD_InputSet> set = std::make_shared<FD_InputSet>(shared_ids++);
	shared_sets.push_back(set);
	return set;
}
bool FD_InputSet::removeSharedSet(const int id) {
	auto it = shared_sets.begin();
	while (it != shared_sets.end()) {
		if ((*it)->getID() == id) {
			shared_sets.erase(it);
			return true;
		}
		it++;
	}
	return false;
}

int FD_InputSet::getID() {
	return id;
}

int FD_InputSet::getMouseX() const {
	return mouse_x;
}
int FD_InputSet::getMouseY() const {
	return mouse_y;
}
double FD_InputSet::getAxisValue(SDL_JoystickID id, FD_ControllerAxis a) const {
	if (id == FD_ALL_JOYSTICKS) id = lastJoystick;
	if (axis_values.find(id) != axis_values.end()) {
		if (axis_values.at(id).find(a) != axis_values.at(id).end()) {
			double power = static_cast<double>(axis_values.at(id).at(a))
				/ static_cast<double>(SDL_JOYSTICK_AXIS_MAX);
			if (power > 1) power = 1;
			if (power < 0) power = 0;
			return power;
		}
	}
	return 0;
}
FD_Device FD_InputSet::getLastDevice() const {
	return lastDevice;
}
SDL_JoystickID FD_InputSet::getLastJoystick() const {
	return lastJoystick;
}

double FD_InputSet::getAngle(int x, int y) const {
	return atan2(mouse_y - y, mouse_x - x);
}
double FD_InputSet::getAngle(double x, double y) const {
	return atan2(mouse_y - y, mouse_x - x);
}
double FD_InputSet::getAngle(SDL_JoystickID id, FD_ControllerAxis axis) const {
	Uint16 x{ 0 }, y{ 0 };
	if (id == FD_ALL_JOYSTICKS) id = lastJoystick;
	if (axis_values.find(id) != axis_values.end()) {
		switch (axis) {
		case FD_AXIS_LEFT:
		{
			Uint16 lxr{ 0 }, lxl{ 0 }, lyd{ 0 }, lyu{ 0 };
			if (axis_values.at(id).find(FD_LEFT_X_RIGHT) == axis_values.at(id).end()) {
				lxr = axis_values.at(id).at(FD_LEFT_X_RIGHT);
			}
			if (axis_values.at(id).find(FD_LEFT_X_LEFT) == axis_values.at(id).end()) {
				lxl = axis_values.at(id).at(FD_LEFT_X_LEFT);
			}
			x = lxr - lxl;
			if (axis_values.at(id).find(FD_LEFT_Y_DOWN) == axis_values.at(id).end()) {
				lyd = axis_values.at(id).at(FD_LEFT_Y_DOWN);
			}
			if (axis_values.at(id).find(FD_LEFT_Y_UP) == axis_values.at(id).end()) {
				lyu = axis_values.at(id).at(FD_LEFT_Y_UP);
			}
			y = lyd - lyu;
			break;
		}
		case FD_AXIS_RIGHT:
			Uint16 rxr{ 0 }, rxl{ 0 }, ryd{ 0 }, ryu{ 0 };
			if (axis_values.at(id).find(FD_RIGHT_X_RIGHT) == axis_values.at(id).end()) {
				rxr = axis_values.at(id).at(FD_RIGHT_X_RIGHT);
			}
			if (axis_values.at(id).find(FD_RIGHT_X_RIGHT) == axis_values.at(id).end()) {
				rxl = axis_values.at(id).at(FD_RIGHT_X_RIGHT);
			}
			x = rxr - rxl;
			if (axis_values.at(id).find(FD_RIGHT_Y_DOWN) == axis_values.at(id).end()) {
				ryd = axis_values.at(id).at(FD_RIGHT_Y_DOWN);
			}
			if (axis_values.at(id).find(FD_RIGHT_Y_UP) == axis_values.at(id).end()) {
				ryu = axis_values.at(id).at(FD_RIGHT_Y_UP);
			}
			y = ryd - ryu; 
			break;
		}
	}
	return atan2(x, y);
}

bool FD_InputSet::mouseMoved() {
	if (mouse_moved) {
		mouse_moved = false;
		return true;
	}
	return false;
}
bool FD_InputSet::axisMoved(SDL_JoystickID id, FD_ControllerAxis a) {
	if (id == FD_ALL_JOYSTICKS) id = lastJoystick;
	if (axis_moved.find(id) != axis_moved.end()) {
		if (axis_moved.at(id).find(a) != axis_moved.at(id).end()) {
			if (axis_moved[id][a]) {
				axis_moved[id][a] = false;
				return true;
			}
		}
	}
	return false;
}

// Input Map Member Functions

FD_InputSet::FD_InputMap::FD_InputMap
(const FD_MapType t, const std::shared_ptr<FD_Input> input, const int map_code, const Uint16 pause)
	: type{ t }, input{ input }, map_code{ map_code }, pause{ pause } {}
FD_InputSet::FD_InputMap::~FD_InputMap() { }

void FD_InputSet::FD_InputMap::update() {
	if (wait > 0) wait--;
}
void FD_InputSet::FD_InputMap::reset() {
	wait = 0;
}

bool FD_InputSet::FD_InputMap::call(const FD_MapType t, const std::shared_ptr<FD_Input> input) {
	if (wait > 0) return false;
	if ((input->verify(*this->input)) && (t == this->type)) {
		wait = pause;
		return true;
	}
	return false;
}

std::shared_ptr<FD_Input> FD_InputSet::FD_InputMap::getInput() {
	return this->input;
}
FD_MapType FD_InputSet::FD_InputMap::getType() {
	return this->type;
}

FD_InputEvent FD_InputSet::FD_InputMap::getEvent() {
	FD_InputEvent e;
	e.code = map_code;
	e.device = input->getDevice();
	e.joystick_id = input->getJoystickID();
	return e;
}
