#ifndef FD_INPUT_MANAGER_H_
#define FD_INPUT_MANAGER_H_

#include <memory>
#include <vector>
#include <unordered_map>

#include <SDL_events.h>
#include <SDL_haptic.h>

#include "fd_input.hpp"

/*!
	@file
	@brief The file containing input managing classes and structures.
*/

//! The FD_InputEvent struct, allows the data corresponding to an input event to be grouped.
typedef struct FD_InputEvent_ {
	//! The corresponding code to the input.
	int code;
	//! The corresponding the device to the input.
	FD_Device device;
	//! The corresponding joystick (if any) to the input.
	SDL_JoystickID joystick_id;
} FD_InputEvent;

//! The FD_InputSet class, creates an individual interface to input for classes.
class FD_InputSet {
private:

	//! The FD_InputMap class, represents a map from some input to a code.
	class FD_InputMap {
	private:

		const Uint16 pause;
		Uint16 wait{ 0 };

		const FD_MapType type;
		const std::shared_ptr<FD_Input> input;
		const int map_code;

	public:

		//! Constructs a FD_InputMap.
		/*!
			\param t        The type of map this is.
			\param input    The input the map reacts to.
			\param map_code The code corresponding to the map.
			\param pause    The length of time the map needs to wait before it can be used again in ms.
		*/
		FD_InputMap(const FD_MapType t, const std::shared_ptr<FD_Input> input,
			const int map_code, const Uint16 pause);
		//! Destroys the FD_InputType.
		~FD_InputMap();

		//! Updates the map.
		void update();
		//! Resets the map.
		void reset();

		//! Calls the map to compare its type and input to the incoming input.
		/*!
			\param t     The type of the incoming input.
			\param input The FD_Input instance of the incoming input.

			\return Whether this map matches the incoming input.
		*/
		bool call(const FD_MapType t, const std::shared_ptr<FD_Input> input);
		
		//! Returns the map type of this map.
		/*!
			\return The map type of this map.
		*/
		FD_MapType getType();
		//! Returns the input of this map.
		/*!
			\return The input of this map.
		*/
		std::shared_ptr<FD_Input> getInput();
		//! Returns the input event corresponding to this map.
		/*!
			\return The input event corresponding to this map.
		*/
		FD_InputEvent getEvent();

	};

	const int id;
	int shared_ids{ 0 };
	std::vector<FD_InputEvent> event_queue{  };
	std::vector<std::unique_ptr<FD_InputMap>> maps{  };

	int mouse_x{};
	int mouse_y{};
	std::unordered_map<SDL_JoystickID, std::unordered_map<FD_ControllerAxis, Uint16>> axis_values{ };

	bool mouse_moved{ false };
	std::unordered_map<SDL_JoystickID, std::unordered_map<FD_ControllerAxis, bool>> axis_moved{ };

	FD_Device lastDevice{ FD_DEVICE_NONE };
	SDL_JoystickID lastJoystick{};

	std::vector<std::shared_ptr<FD_InputSet>> shared_sets;

	void addMap(const FD_MapType t, const std::shared_ptr<FD_Input> input,
		const int map_code, const Uint16 pause);

public:

	//! Constructs a FD_InputSet.
	/*!
		\param id The identifier of the input list this set is attached to.
	*/
	FD_InputSet(int id);
	//! Destroys the FD_InputSet.
	~FD_InputSet();

	//! Updates the input set, its shared sets, and its maps.
	void update();
	//! Clears all maps and pending events from the input set.
	void clear();
	//! Resets the input set, its shared sets, and its maps.
	void reset();
	//! Clears all pending events from the input set.
	void empty();

	//! Calls the input set with a map type and input.
	/*!
		This function checks the parameters against each map contained
		within it. If there is a match, it is added to the event queue.

		\param t     The type to check against.
		\param input The input to check against.
	*/
	void call(const FD_MapType t, const std::shared_ptr<FD_Input> input);
	//! Returns a input event by reference.
	/*!
		\param code The parameter to write the pending event to.

		\return Whether an event has been returned via the parameter.
	*/
	bool getEvent(FD_InputEvent &code);

	//! Update the set with the new mouse position.
	/*!
		\param mouse_x The new x-coordinate of the mouse.
		\param mouse_y The new y-coordinate of the mouse.
	*/
	void updateMouse(int mouse_x, int mouse_y);
	//! Update an axis with the new position.
	/*!
		\param id    The ID of the joystick where the axis has changed.
		\param axis  The axis that has changed.
		\param value The new value of the axis.
	*/
	void updateAxis(SDL_JoystickID id, FD_ControllerAxis axis, Uint16 value);
	//! Update the last device.
	/*!
		\param device The new device.
		\param id     The ID of the joystick corresponding to the device (if any).
	*/
	void updateDevice(FD_Device device, SDL_JoystickID id = 0);

	//! Adds a joystick axis map to the set.
	/*!
		\param id		The ID of the controller.
		\param axis     The axis of the map.
		\param map_code The code corresponding to the map.
		\param pause    The length of time the map needs to wait before it can be used again in ms.
	*/
	void addJoystickAxisMap(const SDL_JoystickID id, const FD_ControllerAxis axis,
		const int map_code, const Uint16 pause = 0);
	//! Adds a key map to the set.
	/*!
		\param t		The type of map.
		\param k        The code for the key.
		\param map_code The code corresponding to the map.
		\param pause    The length of time the map needs to wait before it can be used again in ms.
	*/
	void addKeyMap(const FD_MapType t,
		const SDL_Keycode k, const int map_code, const Uint16 pause = 0);
	//! Adds a mouse button map to the set.
	/*!
		\param t		The type of map.
		\param b        The code for the mouse button.
		\param map_code The code corresponding to the map.
		\param pause    The length of time the map needs to wait before it can be used again in ms.
	*/
	void addMouseButtonMap(const FD_MapType t,
		const Uint8 b, const int map_code, const Uint16 pause = 0);
	//! Adds a mouse wheel to the set.
	/*!
		\param d		The direction of the scroll wheel.
		\param map_code The code corresponding to the map.
		\param pause    The length of time the map needs to wait before it can be used again in ms.
	*/
	void addMouseWheelMap(const FD_ScrollDirection d, const int map_code,
		const Uint16 pause = 0);
	//! Adds a joystick button map to the set.
	/*!
		\param t		The type of map.
		\param id       The ID of the controller.
		\param b        The code for the joystick button.
		\param map_code The code corresponding to the map.
		\param pause    The length of time the map needs to wait before it can be used again in ms.
	*/
	void addJoystickButtonMap(const FD_MapType t, const SDL_JoystickID id,
		const Uint8 b, const int map_code, const Uint16 pause = 0);
	//! Adds a joystick D-pad map to the set.
	/*!
		\param t		The type of map.
		\param id       The ID of the controller.
		\param b        The code for the D-pad button.
		\param map_code The code corresponding to the map.
		\param pause    The length of time the map needs to wait before it can be used again in ms.
	*/
	void addJoystickDpadMap(const FD_MapType t, const SDL_JoystickID id,
		const Uint8 b, const int map_code, const Uint16 pause = 0);

	//! Removes the map corresponding to the parameters.
	/*!
		\param t     The map type to check.
		\param input The input to check against.

		\return If a map was deleted.
	*/
	bool removeMap(const FD_MapType t, std::shared_ptr<FD_Input> input);

	//! Creates a shared set from this set and returns it.
	/*!
		This is used to create seperate interfaces for the same input set,
		which is useful for the FD_ButtonManager among other things.

		The ID of the shared set is independent of the input list ID of the
		original set.

		\return The new shared set.
	*/
	std::shared_ptr<FD_InputSet> generateSharedSet();
	//! Removes a shared set from the set.
	/*!
		\param id The ID of the shared set to remove.
		
		\return Whether a set was removed or not.
	*/
	bool removeSharedSet(const int id);

	//! Returns the ID corresponding to this set.
	/*!
		\return The ID corresponding to this set.
	*/
	int getID();

	//! Returns the mouse's x-coordinate relative to the window.
	/*!
		\return The mouse's x-coordinate relative to the window.
	*/
	int getMouseX() const;
	//! Returns the mouse's y-coordinate relative to the window.
	/*!
		\return The mouse's y-coordinate relative to the window.
	*/
	int getMouseY() const;
	//! Returns an axis' position.
	/*!
		\param id The ID of the controller the axis is on.
		\param a  The axis to query.

		\return The desired axis' position.
	*/
	double getAxisValue(SDL_JoystickID id, FD_ControllerAxis a) const;
	//! Returns the last device used.
	/*!
		\return The last device used.
	*/
	FD_Device getLastDevice() const;
	//! Returns the last joystick used.
	/*!
		\return The last joystick used.
	*/
	SDL_JoystickID getLastJoystick() const;

	//! Returns the angle the mouse position makes with the given point in degrees.
	/*!
		\param x The x-coordinate to query.
		\param y The y-coordinate to query.

		\return The angle the mouse position makes with the given point in degrees.
	*/
	double getAngle(int x, int y) const;
	//! Returns the angle the mouse position makes with the given point in degrees.
	/*!
		\param x The x-coordinate to query.
		\param y The y-coordinate to query.

		\return The angle the mouse position makes with the given point in degrees.
	*/
	double getAngle(double x, double y) const;
	//! Returns the angle of the axis.
	/*!
		\param id   The ID of the controller to query.
		\param axis The axis to query.

		\return The angle of the axis.
	*/
	double getAngle(SDL_JoystickID id, FD_ControllerAxis axis) const;

	//! Returns whether the mouse has moved since it was last checked.
	/*!
		\returns Whether the mouse has moved since it was last checked.

		\warning Note that this method is not const, thus calling it may affect subsequent calls.
	*/
	bool mouseMoved();
	//! Returns whether an axis has moved since it was last checked.
	/*!
		\param id The ID of the controller to check.
		\param a  The axis to check.

		\returns Whether the axis has moved since it was last checked.

		\warning Note that this method is not const, thus calling it may affect subsequent calls.
	*/
	bool axisMoved(SDL_JoystickID id, FD_ControllerAxis a);

};

//! The FD_InputManager class, manages all input devices.
class FD_InputManager {
private:

	enum Axis {
		AXIS_LEFT_X,
		AXIS_LEFT_Y,
		AXIS_LEFT_TRIGGER,
		AXIS_RIGHT_X,
		AXIS_RIGHT_Y,
		AXIS_RIGHT_TRIGGER,
	};

	int mouse_x{};
	int mouse_y{};

	std::unordered_map<int, SDL_Haptic*> haptics{ };
	std::unordered_map<int, SDL_Joystick*> joysticks{ };
	std::unordered_map<SDL_JoystickID, std::unordered_map<FD_ControllerAxis, Uint16>> axis_values{ };

	FD_Device lastDevice{ FD_DEVICE_NONE };
	SDL_JoystickID lastJoystick{};

	std::vector<std::shared_ptr<FD_Input>> pressed{ };
	std::vector<std::shared_ptr<FD_Input>> held{ };
	std::vector<std::shared_ptr<FD_Input>> released{ };
	std::vector<std::shared_ptr<FD_Input>> analog{ };
	std::vector<std::shared_ptr<FD_Input>> other{ };

	int idCount{ 0 };
	int currentInputSet{ 0 };
	std::vector<std::shared_ptr<FD_InputSet>> maps{  };

	bool isHeld(const std::shared_ptr<FD_Input> input);
	void addHeldInput(std::shared_ptr<FD_Input> input);
	void removeHeldInput(std::shared_ptr<FD_Input> input);
	void removeHeldDpadInput(SDL_JoystickID id);
	void removeHeldAxisInput(SDL_JoystickID id, FD_ControllerAxis axis);

	void updateDevice(FD_Device device, SDL_JoystickID id = 0);

	// Filler function for the dead zone
	static inline void DEAD_ZONE(Uint16 &dead_zone) { dead_zone = 6000; }

public:

	//! Constructs a FD_InputManager.
	FD_InputManager();
	//! Destroys the FD_InputManager.
	~FD_InputManager();
	//! Updates the input manager.
	void update();

	//! Processes a keyboard event.
	/*!
		\param e The event.
	*/
	void pushKeyboardEvent(const SDL_KeyboardEvent* e);
	//! Processes a mouse button event.
	/*!
		\param e The event.
	*/
	void pushMouseButtonEvent(const SDL_MouseButtonEvent* e);
	//! Processes a mouse motion event.
	/*!
		\param e The event.
	*/
	void pushMouseMotionEvent(const SDL_MouseMotionEvent* e);
	//! Processes a mouse wheel event.
	/*!
		\param e The event.
	*/
	void pushMouseWheelEvent(const SDL_MouseWheelEvent* e);
	//! Processes a joystick button event.
	/*!
		\param e The event.
	*/
	void pushJoyButtonEvent(const SDL_JoyButtonEvent* e);
	//! Processes a joystick axis event.
	/*!
		\param e The event.
	*/
	void pushJoyAxisEvent(const SDL_JoyAxisEvent* e);
	//! Processes a joystick D-pad event.
	/*!
		\param e The event.
	*/
	void pushJoyDpadEvent(const SDL_JoyHatEvent* e);
	//! Processes a joystick device event.
	/*!
		\param e The event.
	*/
	void pushJoyDeviceEvent(const SDL_JoyDeviceEvent* e);

	//! Generates a new input set and returns the corresponding ID.
	/*!
		\return The ID of the new input set.
	*/
	int generateSet();
	//! Sets the current input set.
	/*!
		\param id The ID of the input set to change to.
	*/
	void setInputSet(const int id);
	
	//! Returns the current input set.
	/*!
		\return The current input set.
	*/
	std::weak_ptr<FD_InputSet> getInputSet() const;
	//! Returns the input set corresponding to the given ID.
	/*!
		\param id The ID of the desired input set.

		\return The input set corresponding to the given ID.
	*/
	std::weak_ptr<FD_InputSet> getInputSet(const int id) const;

	
	//! Returns the mouse's x-coordinate relative to the window.
	/*!
		\return The mouse's x-coordinate relative to the window.
	*/
	int getMouseX() const;
	//! Returns the mouse's y-coordinate relative to the window.
	/*!
		\return The mouse's y-coordinate relative to the window.
	*/
	int getMouseY() const;
	//! Returns an axis' position.
	/*!
		\param id The ID of the controller the axis is on.
		\param a  The axis to query.

		\return The desired axis' position.
	*/
	double getAxisValue(SDL_JoystickID id, FD_ControllerAxis a) const;
	//! Returns the last device used.
	/*!
		\return The last device used.
	*/
	FD_Device getLastDevice() const;
	//! Returns the last joystick used.
	/*!
		\return The last joystick used.
	*/
	SDL_JoystickID getLastJoystick() const;

	//! Causes haptic feedback on a joystick.
	/*!
		\param id       The ID of the controller to affect.
		\param power    The strength of the feedback.
		\param duration The length of time of the feedback in ms.
	*/
	void hapticFeedback(SDL_JoystickID id, float power, Uint32 duration);

};

#endif