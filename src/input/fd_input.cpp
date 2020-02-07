#include "fd_input.hpp"

#include <sstream>

#include "fd_inputManager.hpp"
#include "../main/fd_handling.hpp"
#include "../input/fd_serialisation.hpp"

// Input Member Functions

FD_Input::FD_Input(const FD_InputType type) : type{ type } { }
FD_Input::~FD_Input() { }

bool FD_Input::verify(const FD_Input& input) const { return false; }
bool FD_Input::verify(const FD_KeyInput& input) const { return false; }
bool FD_Input::verify(const FD_MouseButtonInput& input) const { return false; }
bool FD_Input::verify(const FD_MouseWheelInput& input) const { return false; }
bool FD_Input::verify(const FD_JoystickButtonInput& input) const { return false; }
bool FD_Input::verify(const FD_JoystickDpadInput& input) const { return false; }
bool FD_Input::verify(const FD_AnalogInput& input) const { return false; }

const FD_InputType FD_Input::getType() const {
	return type;
}

std::string FD_Input::serialise() const {
	std::string s{ std::to_string(getType()) };
	FD_Serialisation::append(s, getInfo());
	return s;
}

std::shared_ptr<FD_Input> FD_Input::deserialise(const std::string s) {
	std::string info{ s };
	switch (FD_Serialisation::trimInteger(info)) {
	case FD_JOYSTICK_AXIS: return std::make_shared<FD_AnalogInput>(info);
	case FD_KEYBOARD: return std::make_shared<FD_KeyInput>(info);
	case FD_MOUSE_BUTTON: return std::make_shared<FD_MouseButtonInput>(info);
	case FD_MOUSE_WHEEL: return std::make_shared<FD_MouseWheelInput>(info);
	case FD_JOYSTICK_BUTTON: return std::make_shared<FD_JoystickButtonInput>(info);
	case FD_JOYSTICK_DPAD: return std::make_shared<FD_JoystickDpadInput>(info);
	}
	return nullptr;
}

// Analog Input Member Functions

FD_AnalogInput::FD_AnalogInput(std::string s)
	: FD_Input(FD_JOYSTICK_AXIS), id{ getID(s) }, axis{ getAxis(s) } {}
FD_AnalogInput::FD_AnalogInput(const SDL_JoystickID id, const FD_ControllerAxis axis)
	: FD_Input(FD_JOYSTICK_AXIS), id{ id }, axis{ axis } {}
FD_AnalogInput::~FD_AnalogInput() { }

SDL_JoystickID FD_AnalogInput::getID(std::string s) {
	return static_cast<SDL_JoystickID>(FD_Serialisation::trimInteger(s));
}
FD_ControllerAxis FD_AnalogInput::getAxis(std::string s) {
	std::string t{ FD_Serialisation::trim(s) };
	int value = FD_Serialisation::trimInteger(s);
	if (value < -1 || value >= FD_AXIS_COUNT) FD_Handling::error("Value could not be read.", true);
	return static_cast<FD_ControllerAxis>(value);
}

bool FD_AnalogInput::verify(const FD_Input& input) const {
	return input.verify(static_cast<FD_AnalogInput>(*this));
}
bool FD_AnalogInput::verify(const FD_AnalogInput& input) const {
	bool ids = id < 0 || input.getID() < 0 || id == input.getID();
	bool axes = axis < 0 || input.getAxis() < 0 || axis == input.getAxis();
	return ids && axes;
}

SDL_JoystickID FD_AnalogInput::getID() const {
	return id;
}
FD_ControllerAxis FD_AnalogInput::getAxis() const {
	return axis;
}

std::string FD_AnalogInput::getInfo() const {
	std::string s{ std::to_string(id) };
	FD_Serialisation::append(s, axis);
	return s;
}
FD_Device FD_AnalogInput::getDevice() const {
	return FD_DEVICE_JOYSTICK_AXIS;
}
SDL_JoystickID FD_AnalogInput::getJoystickID() const {
	return id;
}

// Key Input Member Functions

FD_KeyInput::FD_KeyInput(std::string s) : FD_Input(FD_KEYBOARD), key{ getKey(s) } {}
FD_KeyInput::FD_KeyInput(SDL_Keycode key) : FD_Input(FD_KEYBOARD), key{ key } {}
FD_KeyInput::~FD_KeyInput() { }

SDL_Keycode FD_KeyInput::getKey(std::string s) {
	return static_cast<SDL_Keycode>(FD_Serialisation::trimInteger(s));
}

bool FD_KeyInput::verify(const FD_Input& input) const {
	return input.verify(static_cast<FD_KeyInput>(*this));
}
bool FD_KeyInput::verify(const FD_KeyInput& input) const {
	return (this->key == input.getKey());
}

SDL_Keycode FD_KeyInput::getKey() const { return key; }

std::string FD_KeyInput::getInfo() const {
	return std::to_string(key);
}
FD_Device FD_KeyInput::getDevice() const {
	return FD_DEVICE_KEYBOARD;
}
SDL_JoystickID FD_KeyInput::getJoystickID() const {
	return -1;
}

// Mouse Button Input Member Functions

FD_MouseButtonInput::FD_MouseButtonInput(std::string s)
	: FD_Input(FD_MOUSE_BUTTON), mouse_button{ getButton(s) } {}
FD_MouseButtonInput::FD_MouseButtonInput(Uint8 mouse_button)
	: FD_Input(FD_MOUSE_BUTTON), mouse_button{ mouse_button } {}
FD_MouseButtonInput::~FD_MouseButtonInput() { }

Uint8 FD_MouseButtonInput::getButton(std::string s) {
	return static_cast<Uint8>(FD_Serialisation::trimInteger(s));
}

bool FD_MouseButtonInput::verify(const FD_Input& input) const {
	return input.verify(static_cast<FD_MouseButtonInput>(*this));
}
bool FD_MouseButtonInput::verify(const FD_MouseButtonInput& input) const {
	return (this->mouse_button == input.getButton());
}

Uint8 FD_MouseButtonInput::getButton() const { return mouse_button; }

std::string FD_MouseButtonInput::getInfo() const {
	return std::to_string(mouse_button);
}
FD_Device FD_MouseButtonInput::getDevice() const {
	return FD_DEVICE_MOUSE;
}
SDL_JoystickID FD_MouseButtonInput::getJoystickID() const {
	return -1;
}

// Mouse Wheel Input Member Functions

FD_MouseWheelInput::FD_MouseWheelInput(std::string s)
	: FD_Input(FD_MOUSE_WHEEL), scroll_direction{ getDirection(s) } {}
FD_MouseWheelInput::FD_MouseWheelInput(FD_ScrollDirection scroll_direction)
	: FD_Input(FD_MOUSE_WHEEL), scroll_direction{ scroll_direction } {}
FD_MouseWheelInput::~FD_MouseWheelInput() { }

FD_ScrollDirection FD_MouseWheelInput::getDirection(std::string s) {
	return static_cast<FD_ScrollDirection>(FD_Serialisation::trimInteger(s));
}

bool FD_MouseWheelInput::verify(const FD_Input& input) const {
	return input.verify(static_cast<FD_MouseWheelInput>(*this));
}
bool FD_MouseWheelInput::verify(const FD_MouseWheelInput& input) const {
	return (this->scroll_direction == input.getDirection());
}

FD_ScrollDirection FD_MouseWheelInput::getDirection() const { return scroll_direction; }

std::string FD_MouseWheelInput::getInfo() const {
	return std::to_string(scroll_direction);
}
FD_Device FD_MouseWheelInput::getDevice() const {
	return FD_DEVICE_MOUSE;
}
SDL_JoystickID FD_MouseWheelInput::getJoystickID() const {
	return -1;
}

// Controller Button Input Member Functions

FD_JoystickButtonInput::FD_JoystickButtonInput(std::string s)
	: FD_Input(FD_JOYSTICK_BUTTON), id{ getID(s) }, button{ getButton(s) } {}
FD_JoystickButtonInput::FD_JoystickButtonInput(SDL_JoystickID id, Sint16 button)
	: FD_Input(FD_JOYSTICK_BUTTON), id{ id }, button{ button } {}
FD_JoystickButtonInput::~FD_JoystickButtonInput() { }

SDL_JoystickID FD_JoystickButtonInput::getID(std::string s) {
	return static_cast<SDL_JoystickID>(FD_Serialisation::trimInteger(s));
}
Sint16 FD_JoystickButtonInput::getButton(std::string s) {
	std::string t{ FD_Serialisation::trim(s) };
	return FD_Serialisation::trimInteger(s);
}

bool FD_JoystickButtonInput::verify(const FD_Input& input) const {
	return input.verify(static_cast<FD_JoystickButtonInput>(*this));
}
bool FD_JoystickButtonInput::verify(const FD_JoystickButtonInput& input) const {
	bool ids = (id == -1) || (input.getID() == -1) || (id == input.getID());
	bool buttons = button < 0 || input.getButton() < 0 || (this->button == input.getButton());
	return ids && buttons;
}

SDL_JoystickID FD_JoystickButtonInput::getID() const { return id; }
Sint16 FD_JoystickButtonInput::getButton() const { return button; }

std::string FD_JoystickButtonInput::getInfo() const {
	std::string s{ std::to_string(id) };
	FD_Serialisation::append(s, button);
	return s;
}
FD_Device FD_JoystickButtonInput::getDevice() const {
	return FD_DEVICE_JOYSTICK_BUTTON;
}
SDL_JoystickID FD_JoystickButtonInput::getJoystickID() const {
	return id;
}

// Controller Dpad Input Member Functions

FD_JoystickDpadInput::FD_JoystickDpadInput(std::string s)
	: FD_Input(FD_JOYSTICK_DPAD), id{ getID(s) }, button{ getButton(s) } {}
FD_JoystickDpadInput::FD_JoystickDpadInput(SDL_JoystickID id, Sint16 button)
	: FD_Input(FD_JOYSTICK_DPAD), id{ id }, button{ button } {}
FD_JoystickDpadInput::~FD_JoystickDpadInput() { }

SDL_JoystickID FD_JoystickDpadInput::getID(std::string s) {
	return static_cast<SDL_JoystickID>(FD_Serialisation::trimInteger(s));
}
Sint16 FD_JoystickDpadInput::getButton(std::string s) {
	std::string t{ FD_Serialisation::trim(s) };
	return FD_Serialisation::trimInteger(s);
}

bool FD_JoystickDpadInput::verify(const FD_Input& input) const {
	return input.verify(static_cast<FD_JoystickDpadInput>(*this));
}
bool FD_JoystickDpadInput::verify(const FD_JoystickDpadInput& input) const {
	bool ids = (id == -1) || (input.getID() == -1) || (id == input.getID());
	bool dir = (button < 0) || (input.getDirection() < 0) || (this->button == input.getDirection());
	return ids && dir;
}

SDL_JoystickID FD_JoystickDpadInput::getID() const { return id; }
Sint16 FD_JoystickDpadInput::getDirection() const { return button; }

std::string FD_JoystickDpadInput::getInfo() const {
	std::string s{ std::to_string(id) };
	FD_Serialisation::append(s, button);
	return s;
}
FD_Device FD_JoystickDpadInput::getDevice() const {
	return FD_DEVICE_JOYSTICK_BUTTON;
}
SDL_JoystickID FD_JoystickDpadInput::getJoystickID() const {
	return id;
}