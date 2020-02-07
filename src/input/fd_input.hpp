#ifndef FD_INPUT_H_
#define FD_INPUT_H_

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include <SDL_joystick.h>
#include <SDL_keycode.h>

/*!
	@file
	@brief The file containing the classes relating to input categorisation.
*/

//! The definition of the value that refers to all loaded joysticks.
#define FD_ALL_JOYSTICKS -1
//! The definition of the value that refers to all joystick buttons.
#define FD_ALL_JOYSTICK_BUTTONS -1

//! The enumeration containing the type of input device.
enum FD_Device {
	//! The value corresponding to the keyboard.
	FD_DEVICE_KEYBOARD,
	//! The value corresponding to the mouse.
	FD_DEVICE_MOUSE,
	//! The value corresponding to the joystick analog sticks and triggers.
	FD_DEVICE_JOYSTICK_AXIS,
	//! The value corresponding to the joysticks buttons.
	FD_DEVICE_JOYSTICK_BUTTON,
	//! The value corresponding to no device.
	FD_DEVICE_NONE
};

//! The enumeration containing types of maps.
enum FD_MapType {
	//! The value corresponding to a button being pressed.
	FD_MAP_PRESSED,
	//! The value corresponding to a button being held.
	FD_MAP_HELD,
	//! The value corresponding to a button being released.
	FD_MAP_RELEASED,
	//! The value corresponding to an analog map.
	FD_MAP_ANALOG,
	//! The value corresponding to some other type, like the mouse scroll wheel.
	FD_MAP_OTHER
};

//! The enumeration containing types of inputs.
enum FD_InputType {
	//! The value corresponding to the joystick analog sticks and triggers. 
	FD_JOYSTICK_AXIS,
	//! The value corresponding to the keyboard.
	FD_KEYBOARD,
	//! The value corresponding to mouse buttons.
	FD_MOUSE_BUTTON,
	//! The value corresponding to the mouse wheel.
	FD_MOUSE_WHEEL,
	//! The value corresponding to joystick buttons.
	FD_JOYSTICK_BUTTON,
	//! The value corresponding to the joystick D-pad.
	FD_JOYSTICK_DPAD
};

//! The enumeration containing the scroll directions.
enum FD_ScrollDirection {
	//! The value corresponding to scrolling up.
	FD_SCROLL_UP,
	//! The value corresponding to scrolling down.
	FD_SCROLL_DOWN
};

//! The enumeration containing the types of controller axis.
enum FD_ControllerAxis {
	//! The value corresponding to all axes.
	FD_ALL_AXES = -1,

	//! The value corresponding to the whole left analog stick.
	FD_AXIS_LEFT,
	//! The value corresponding to the left analog sticks' left movement.
	FD_LEFT_X_LEFT,
	//! The value corresponding to the left analog sticks' right movement.
	FD_LEFT_X_RIGHT,
	//! The value corresponding to the left analog sticks' upward movement.
	FD_LEFT_Y_UP,
	//! The value corresponding to the left analog sticks' downward movement.
	FD_LEFT_Y_DOWN,
	//! The value corresponding to the left trigger.
	FD_LEFT_TRIGGER,
	//! The value corresponding to the whole right analog stick.
	FD_AXIS_RIGHT,
	//! The value corresponding to the right analog sticks' left movement.
	FD_RIGHT_X_LEFT,
	//! The value corresponding to the right analog sticks' right movement.
	FD_RIGHT_X_RIGHT,
	//! The value corresponding to the right analog sticks' upward movement.
	FD_RIGHT_Y_UP,
	//! The value corresponding to the right analog sticks' downward movement.
	FD_RIGHT_Y_DOWN,
	//! The value corresponding to the right trigger.
	FD_RIGHT_TRIGGER,

	//! The value corresponding to the number of axes in this enum.
	FD_AXIS_COUNT
};

class FD_KeyInput;
class FD_MouseButtonInput;
class FD_MouseWheelInput;
class FD_JoystickButtonInput;
class FD_JoystickDpadInput;
class FD_AnalogInput;

//! The FD_Input class, allows for verification and comparision of different input types.
class FD_Input {
private:

	const FD_InputType type;

public:

	//! Constructs the FD_Input.
	/*!
		\param type The type of input.
	*/
	FD_Input(const FD_InputType type);
	//! Destroys the FD_Input.
	virtual ~FD_Input();

	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	virtual bool verify(const FD_Input &input) const = 0;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	virtual bool verify(const FD_KeyInput &input) const;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	virtual bool verify(const FD_MouseButtonInput &input) const;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	virtual bool verify(const FD_MouseWheelInput &input) const;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	virtual bool verify(const FD_JoystickButtonInput &input) const;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	virtual bool verify(const FD_JoystickDpadInput &input) const;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	virtual bool verify(const FD_AnalogInput &input) const;
	
	//! Partially serialises the input.
	/*!
		This allows the FD_Input base class to serialise the input
		by returning the serialisation of just the specific
		information relevant to the input.

		Call serialise for a full serialisation.

		\return Returns the partial serialisation of the input.
	
		\sa serialise
	*/
	virtual std::string getInfo() const = 0;
	//! Returns the device corresponding to the input.
	/*!
		\return The device corresponding to the input.
	*/
	virtual FD_Device getDevice() const = 0;
	//! Returns the joystick ID corresponding to the input.
	virtual SDL_JoystickID getJoystickID() const = 0;

	//! Serialises the input.
	/*!
		\return The serialised input.
	*/
	std::string serialise() const;
	//! Returns the input type.
	/*!
		\return The input type.
	*/
	const FD_InputType getType() const;

	//! Creates a FD_Input instance from a serialised input.
	/*!
		\param s The serialised input.

		\return The new FD_Input instance.
	*/
	static std::shared_ptr<FD_Input> deserialise(const std::string s);

};

//! The FD_AnalogInput class, allows for analog inputs.
class FD_AnalogInput : public virtual FD_Input {
private:

	const SDL_JoystickID id;
	const FD_ControllerAxis axis;

	SDL_JoystickID getID(std::string s);
	FD_ControllerAxis getAxis(std::string s);

public:

	//! Constructs a FD_AnalogInput from a serialised input string.
	/*!
		\param s The serialised input string.
	*/
	FD_AnalogInput(std::string s);
	//! Constructs a FD_AnalogInput from values.
	/*!
		\param id   The ID of the corresponding joystick.
		\param axis The corresponding axis.
	*/
	FD_AnalogInput(const SDL_JoystickID id, const FD_ControllerAxis axis);
	//! Destroys the FD_AnalogInput.
	~FD_AnalogInput();

	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_Input& input) const override;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_AnalogInput& input) const;

	//! Returns the input's joystick ID.
	/*!
		\return The input's joystick ID.
	*/
	SDL_JoystickID getID() const;
	//! Returns the input's axis.
	/*!
		\return The input's axis.
	*/
	FD_ControllerAxis getAxis() const;

	//! Partially serialises the input.
	/*!
		This allows the FD_Input base class to serialise the input
		by returning the serialisation of just the specific
		information relevant to the input.

		Call serialise for a full serialisation.

		\return Returns the partial serialisation of the input.

		\sa serialise
	*/
	std::string getInfo() const override;
	//! Returns the device corresponding to the input.
	/*!
		\return The device corresponding to the input.
	*/
	FD_Device getDevice() const override;
	//! Returns the joystick ID corresponding to the input.
	SDL_JoystickID getJoystickID() const override;

};

//! The FD_KeyInput class, allows for keyboard inputs.
class FD_KeyInput : virtual public FD_Input {
private:

	const SDL_Keycode key{};

	SDL_Keycode getKey(std::string s);

public:

	//! Constructs a FD_KeyInput from a serialised input string.
	/*!
		\param s The serialised input string.
	*/
	FD_KeyInput(std::string s);
	//! Constructs a FD_KeyInput from values.
	/*!
		\param key The corresponding key.
	*/
	FD_KeyInput(SDL_Keycode key);
	//! Destroys the FD_KeyInput.
	~FD_KeyInput();

	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_Input &input) const override;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_KeyInput &input) const;

	//! Returns the input's key.
	/*!
		\return The input's key.
	*/
	SDL_Keycode getKey() const;

	//! Partially serialises the input.
	/*!
		This allows the FD_Input base class to serialise the input
		by returning the serialisation of just the specific
		information relevant to the input.

		Call serialise for a full serialisation.

		\return Returns the partial serialisation of the input.

		\sa serialise
	*/
	std::string getInfo() const override;
	//! Returns the device corresponding to the input.
	/*!
		\return The device corresponding to the input.
	*/
	FD_Device getDevice() const override;
	//! Returns the joystick ID corresponding to the input.
	/*!
		This will always return -1.
	*/
	SDL_JoystickID getJoystickID() const override;

};

//! The FD_MouseButtonInput class, allows for mouse button inputs.
class FD_MouseButtonInput : virtual public FD_Input {
private:

	const Uint8 mouse_button{};

	Uint8 getButton(std::string s);

public:

	//! Constructs a FD_MouseButtonInput from a serialised input string.
	/*!
		\param s The serialised input string.
	*/
	FD_MouseButtonInput(std::string s);
	//! Constructs a FD_MouseButtonInput from values.
	/*!
		\param mouse_button The corresponding button.
	*/
	FD_MouseButtonInput(Uint8 mouse_button);
	//! Destroys the FD_MouseButtonInput.
	~FD_MouseButtonInput();

	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_Input &input) const override;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_MouseButtonInput &input) const;

	//! Returns the input's button.
	/*!
		\return The input's button.
	*/
	Uint8 getButton() const;

	//! Partially serialises the input.
	/*!
		This allows the FD_Input base class to serialise the input
		by returning the serialisation of just the specific
		information relevant to the input.

		Call serialise for a full serialisation.

		\return Returns the partial serialisation of the input.

		\sa serialise
	*/
	std::string getInfo() const override;
	//! Returns the device corresponding to the input.
	/*!
		\return The device corresponding to the input.
	*/
	FD_Device getDevice() const override;
	//! Returns the joystick ID corresponding to the input.
	/*!
		This will always return -1.
	*/
	SDL_JoystickID getJoystickID() const override;

};

//! The FD_MouseWheelInput class, allows for mouse wheel inputs.
class FD_MouseWheelInput : virtual public FD_Input {
private:

	const FD_ScrollDirection scroll_direction{};

	FD_ScrollDirection getDirection(std::string s);

public:

	//! Constructs a FD_MouseWheelInput from a serialised input string.
	/*!
		\param s The serialised input string.
	*/
	FD_MouseWheelInput(std::string s);
	//! Constructs a FD_MouseWheelInput from values.
	/*!
		\param scroll_direction The corresponding scroll direction.
	*/
	FD_MouseWheelInput(FD_ScrollDirection scroll_direction);
	//! Destroys the FD_MouseWheelInput.
	~FD_MouseWheelInput();

	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_Input &input) const override;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_MouseWheelInput &input) const;

	//! Returns the input's scroll direction.
	/*!
		\return The input's scroll direction.
	*/
	FD_ScrollDirection getDirection() const;

	//! Partially serialises the input.
	/*!
		This allows the FD_Input base class to serialise the input
		by returning the serialisation of just the specific
		information relevant to the input.

		Call serialise for a full serialisation.

		\return Returns the partial serialisation of the input.

		\sa serialise
	*/
	std::string getInfo() const override;
	//! Returns the device corresponding to the input.
	/*!
		\return The device corresponding to the input.
	*/
	FD_Device getDevice() const override;
	//! Returns the joystick ID corresponding to the input.
	/*!
		This will always return -1.
	*/
	SDL_JoystickID getJoystickID() const override;

};

//! The FD_JoystickButtonInput class, allows for joystick button inputs.
class FD_JoystickButtonInput : virtual public FD_Input {
private:

	const SDL_JoystickID id;
	const Sint16 button{};

	SDL_JoystickID getID(std::string s);
	Sint16 getButton(std::string s);

public:

	//! Constructs a FD_JoystickButtonInput from a serialised input string.
	/*!
		\param s The serialised input string.
	*/
	FD_JoystickButtonInput(std::string s);
	//! Constructs a FD_JoystickButtonInput from values.
	/*!
		\param id     The ID of the corresponding joystick.
		\param button The corresponding controller button.
	*/
	FD_JoystickButtonInput(SDL_JoystickID id, Sint16 button);
	//! Destroys the FD_JoystickButtonInput.
	~FD_JoystickButtonInput();
	
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_Input& input) const override;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_JoystickButtonInput& input) const;

	//! Returns the input's joystick ID.
	/*!
		\return The input's joystick ID.
	*/
	SDL_JoystickID getID() const;
	//! Returns the input's button.
	/*!
		\return The input's button.
	*/
	Sint16 getButton() const;

	//! Partially serialises the input.
	/*!
		This allows the FD_Input base class to serialise the input
		by returning the serialisation of just the specific
		information relevant to the input.

		Call serialise for a full serialisation.

		\return Returns the partial serialisation of the input.

		\sa serialise
	*/
	std::string getInfo() const override;
	//! Returns the device corresponding to the input.
	/*!
		\return The device corresponding to the input.
	*/
	FD_Device getDevice() const override;
	//! Returns the joystick ID corresponding to the input.
	SDL_JoystickID getJoystickID() const override;

};

//! The FD_JoystickDpadInput class, allows for joystick D-pad inputs.
class FD_JoystickDpadInput : virtual public FD_Input {
private:

	const SDL_JoystickID id;
	const Sint16 button{};

	SDL_JoystickID getID(std::string s);
	Sint16 getButton(std::string s);

public:

	//! Constructs a FD_JoystickDpadInput from a serialised input string.
	/*!
		\param s The serialised input string.
	*/
	FD_JoystickDpadInput(std::string s);
	//! Constructs a FD_JoystickDpadInput from values.
	/*!
		\param id     The ID of the corresponding joystick.
		\param button The corresponding D-pad button.
	*/
	FD_JoystickDpadInput(SDL_JoystickID id, Sint16 button);
	//! Destroys the FD_JoystickDpadInput.
	~FD_JoystickDpadInput();

	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_Input& input) const override;
	//! Checks whether the given input corresponds to this input.
	/*! \param input The given input.
		\return Whether the given input corresponds to this input.
	*/
	bool verify(const FD_JoystickDpadInput& input) const;

	//! Returns the input's joystick ID.
	/*!
		\return The input's joystick ID.
	*/
	SDL_JoystickID getID() const;
	//! Returns the input's D-pad direction.
	/*!
		\return The input's D-pad direction.
	*/
	Sint16 getDirection() const;

	//! Partially serialises the input.
	/*!
		This allows the FD_Input base class to serialise the input
		by returning the serialisation of just the specific
		information relevant to the input.

		Call serialise for a full serialisation.

		\return Returns the partial serialisation of the input.

		\sa serialise
	*/
	std::string getInfo() const override;
	//! Returns the device corresponding to the input.
	/*!
		\return The device corresponding to the input.
	*/
	FD_Device getDevice() const override;
	//! Returns the joystick ID corresponding to the input.
	SDL_JoystickID getJoystickID() const override;

};

#endif
