#ifndef FD_BUTTON_MANAGER_H_
#define FD_BUTTON_MANAGER_H_

#include <vector>
#include <memory>

#include "fd_button.hpp"
#include "../../display/fd_scene.hpp"

/*!
	@file
	@brief This file contains the FD_ButtonManager class.
*/

//! The enum representing the codes used by the input set of the button manager.
enum FD_ButtonResponses {
	//! The code corresponding to moving upward through the buttons.
	FD_BUTTON_UP,
	//! The code corresponding to moving downward through the buttons.
	FD_BUTTON_DOWN,
	//! The code corresponding to a button press with the mouse.
	FD_BUTTON_MOUSE_PRESS,
	//! The code corresponding to a button release with the mouse.
	FD_BUTTON_MOUSE_RELEASE,
	//! The code corresponding to a button press with anything but the mouse.
	FD_BUTTON_OTHER_PRESS,
	//! The code corresponding to a button release with anything but the mouse.
	FD_BUTTON_OTHER_RELEASE,
	//! The code corresponding to the 'enter' command.
	/*!
		For example, when used on a dropdown buttons, this accesses the dropdown elements.
	*/
	FD_BUTTON_ENTER,
	//! The code corresponding to the 'exit' command.
	/*!
		For example, when used when within a dropdown button, this exits the dropdown elements.
	*/
	FD_BUTTON_EXIT
};

//! The FD_ButtonManager class, allowing for easy button creation and management.
class FD_ButtonManager {
private:

	enum ControlType {
		MOUSE,
		DIRECTIONAL
	};
	ControlType controlMethod{ MOUSE };
	bool mouse_activated{ false };
	bool active{ true };
	size_t selected{ 0 };

	std::weak_ptr<FD_Scene> scene;
	std::weak_ptr<FD_CameraSet> cameras;
	std::weak_ptr<FD_InputSet> inputSet;

	std::vector<int> events{};
	std::vector<std::shared_ptr<FD_Button>> buttons{};

	FD_ButtonActivity prepareActivity() const;

public:

	//! Constructs a FD_ButtonManager.
	/*!
		\param scene     The scene to create the buttons with.
		\param cameras   The set of cameras for the buttons to use.
		\param inputList The ID of the input set for the button manager to use.
	*/
	FD_ButtonManager(std::weak_ptr<FD_Scene> scene, 
		std::weak_ptr<FD_CameraSet> cameras,
		int inputList);
	//! Destroys the FD_ButtonManager.
	~FD_ButtonManager();

	//! Updates the buttons.
	void update();
	//! Resets the buttons.
	void reset();

	//! Returns whether an event has occured as well as the assigned code.
	/*!
		\param code The reference of the value used to return a code.

		\return Whether an event has occured.
	*/
	bool getEvent(int& code);

	//! Adds a provided button to the button manager.
	/*!
		\param group  The object group to add the button to.
		\param button The button to add to the manager.
	*/
	void addButton(std::shared_ptr<FD_ObjectGroup> group,
		std::shared_ptr<FD_Button> button);
	//! Creates a basic button and adds it to the button manager.
	/*!
		\param temp   The template for the button.
		\param x	  The x-coordinate of the button.
		\param y      The y-coordinate of the button.
		\param code   The code the button will return when pressed.
		\param text   The text of the button.
		\param font   The font for the text of the button.
		\param colour The colour of the text of the button.
	*/
	void addBasicButton(const FD_ButtonTemplate& temp,
		const int x, const int y,
		const int code,
		const std::string text,
		const std::shared_ptr<FD_Font> font,
		const SDL_Colour colour);
	//! Creates a dropdown button and adds it to the button manager.
	/*!
		\param temp			The template for the button.
		\param x			The x-coordinate of the button.
		\param y			The y-coordinate of the button.
		\param codes		The codes the button will return when each dropdown element is pressed.
		\param texts		The texts of the dropdown elements.
		\param font			The font for the text of the button.
		\param colour		The colour of the text of the button.
		\param preselection The element which is selected on initialisation.
	*/
	void addDropdownButton(const FD_ButtonTemplate& temp,
		const int x, const int y,
		const std::vector<int> codes,
		const std::vector<std::string> texts,
		const std::shared_ptr<FD_Font> font,
		const SDL_Colour colour,
		const size_t preselection = 0);

	//! Returns whether any button is being hovered over by the mouse.
	/*!
		\return Whether any button is being hovered over by the mouse.
	*/
	bool mouseSelected() const;

	//! Sets the activity of the button manager.
	/*!
		An inactive button manager will not react to input of any kind, it
		is 'turned off'. An active one acts as you would expect.

		\param active Whether the button manager should be active or not.
	*/
	void setActive(bool active);
	//! Sets whether the buttons can be activated by the mouse.
	/*!
		\param mouse_only Whether the buttons can be activated by the mouse.
	*/
	void setMouseActivated(bool mouse_only);

	//! Returns the input set being used by the button manager.
	/*!
		\return The input set being used by the button manager.
	*/
	std::shared_ptr<FD_InputSet> getInputSet();
	//! Adds the default maps for the button manager.
	/*
		Calls addDefaultMouseMaps, 
		addDefaultKeyboardMaps and, 
		addDefaultJoystickMaps.

		\sa addDefaultMouseMaps
		\sa addDefaultKeyboardMaps
		\sa addDefaultJoystickMaps
	*/
	void addDefaultMaps();
	//! Adds the default mouse mappings.
	/*!
		Specifically, this makes the left mouse button capable of pressing buttons.
	*/
	void addDefaultMouseMaps();
	//! Adds the default keyboard mappings.
	/*!
		Specifically, this makes the WASD and arrow keys work directionally
		with the enter key for pressing buttons.
	*/
	void addDefaultKeyboardMaps();
	//! Adds the default joystick (controller) maps.
	/*!
		Specifically, this makes the analog sticks and DPad work directionally
		with the A button for pressing buttons.
	*/
	void addDefaultJoystickMaps();

	//! Applies a tween action to each button's x-coordinate.
	/*!
		\param action The action to apply.
	*/
	void applyTweenX(FD_TweenAction& action);
	//! Applies a tween action to each button's y-coordinate.
	/*!
		\param action The action to apply.
	*/
	void applyTweenY(FD_TweenAction& action);
	//! Applies a tween action to each button's width.
	/*!
		\param action The action to apply.
	*/
	void applyTweenWidth(FD_TweenAction& action);
	//! Applies a tween action to each button's height.
	/*!
		\param action The action to apply.
	*/
	void applyTweenHeight(FD_TweenAction& action);
	//! Applies a tween action to each button's opacity.
	/*!
		\param action The action to apply.
	*/
	void applyTweenOpacity(FD_TweenAction& action);

};

#endif
