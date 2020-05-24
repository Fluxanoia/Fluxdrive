#ifndef FD_BUTTON_H_
#define FD_BUTTON_H_

#include "../fd_object.hpp"
#include "../fd_element.hpp"
#include "../../maths/fd_tween.hpp"
#include "../../maths/fd_maths.hpp"
#include "../../display/fd_scene.hpp"
#include "../../input/fd_imageManager.hpp"

/*!
	@file
	@brief The file containing the FD_Button, FD_BasicButton and, FD_DropdownButton class alongside other button structures.
*/

//! The structure for supplying a summary of the state of input to a button.
typedef struct FD_ButtonActivity_ {
	
	//! Whether the mouse is activated.
	bool mouse{ false };
	//! The absolute x-coordinate of the mouse.
	int x;
	//! The absolute y-coordinate of the mouse.
	int y;
	//! The relative x-coordinate of the mouse.
	int camera_x; 
	//! The relative y-coordinate of the mouse.
	int camera_y;

	//! Whether this button is selected or not.
	bool selected{ false };

} FD_ButtonActivity;

//! The structure allowing for easier button creation.
/*!
	Usually, you want to create many buttons with the same or
	similar properties. By creating a template, you only have
	to set these properties once and reuse the template.
*/
typedef struct FD_ButtonTemplate_ {

	//! The scene used to create buttons.
	std::weak_ptr<FD_Scene> scene{ std::weak_ptr<FD_Scene>() };
	//! The object group to add buttons to.
	std::weak_ptr<FD_ObjectGroup> group;
	//! The layer of the buttons.
	int z{};
	//! Whether the camera's coordinates are relative to the camera.
	bool camera_bound{ true };

	//! The background image of the button.
	std::weak_ptr<FD_Image> background{ std::weak_ptr<FD_Image>() };
	//! The width override of the button.
	/*!
		Leave this as zero if you want the width to be
		automatically calculated. This is an override.
	*/
	Uint32 width{ 0 };
	//! The height override of the button.
	/*!
		Leave this as zero if you want the height to be
		automatically calculated. This is an override.
	*/
	Uint32 height{ 0 };
	//! Whether the background image should be stretched to fit the content.
	bool stretch_bg{ true };
	//! When stretched, this variable determines how much wider the button is than the content.
	int stretch_buffer_w{ 20 };
	//! When stretched, this variable determines how much taller the button is than the content.
	int stretch_buffer_h{ 20 };
	//! This is the x-coordinate that the x-coordinate of the button constructor is relative to.
	int origin_x{ 0 };
	//! This is the y-coordinate that the y-coordinate of the button constructor is relative to.
	int origin_y{ 0 };

	//! The sound effect that plays when the button is hovered.
	std::weak_ptr<FD_SFX> sfx_hover{};
	//! The sound effect that plays when the button is pressed.
	std::weak_ptr<FD_SFX> sfx_press{};
	//! The sound effect that plays when the button is released.
	std::weak_ptr<FD_SFX> sfx_release{};

} FD_ButtonTemplate;

//! The FD_Button class, the framework for all buttons.
class FD_Button : public FD_Object {
protected:

	//! The scene used by the button.
	std::weak_ptr<FD_Scene> scene;
	//! The image of the button.
	std::weak_ptr<FD_Image> image;
	//! The SFX played when the button is hovered over.
	std::weak_ptr<FD_SFX> sfx_hover{ std::weak_ptr<FD_SFX>() };
	//! The SFX played when the button is pressed.
	std::weak_ptr<FD_SFX> sfx_press{ std::weak_ptr<FD_SFX>() };
	//! The SFX played when the button is released.
	std::weak_ptr<FD_SFX> sfx_release{ std::weak_ptr<FD_SFX>() };

	//! The code of the button.
	int return_code;
	//! Whether the button is selected (hovered over) or not.
	bool selected{ false };
	//! Whether the button is held down or not.
	bool held{ false };
	//! Whether the button has been entered or not.
	/*!
		\sa FD_BUTTON_ENTER
	*/
	bool entered{ false };

	//! Updates the overlay colour of the button.
	virtual void updateColour();

public:

	//! Constructs a FD_Button.
	/*!
		\param temp The button template to create the button with.
	*/
	FD_Button(const FD_ButtonTemplate temp);
	//! Destroys the FD_Button.
	~FD_Button();

	//! Updates the button.
	/*!
		\param activity The current input state.
	*/
	virtual void update(FD_ButtonActivity activity) = 0;
	//! Checks whether the button is selected by the mouse.
	/*!
		\param a The input state to check under.

		\return Whether the button is selected by the mouse.
	*/
	virtual bool mouseSelected(FD_ButtonActivity a) const = 0;

	//! Performs a press action on the button.
	virtual void press();
	//! Performs a release action on the button.
	virtual bool release() = 0;
	//! Resets the state of the button (whether it's selected or held).
	virtual void reset();

	//! Performs an enter action on the button.
	/*!
		\sa FD_BUTTON_ENTER
	*/
	virtual void enter();
	//! Performs an up action on the button when it is entered.
	/*!
		This allows the button to override the upward action whilst entered.
	*/
	virtual void buttonUp();
	//! Performs a down action on the button when it is entered.
	/*!
		This allows the button to override the downward action whilst entered.
	*/
	virtual void buttonDown();
	//! Performs an exit action on the button.
	/*!
		\sa FD_BUTTON_EXIT
	*/
	virtual void exit();

	//! Returns the code associated with this button.
	/*!
		\return The code associated with this button.
	*/
	int getCode() const;
	//! Returns the image associated with this button.
	/*!
		\return The image associated with this button.
	*/
	std::shared_ptr<FD_Image> getImage() const;
	//! Returns whether the button is selected or not.
	/*!
		\return Whether the button is selected or not.
	*/
	bool isSelected() const;
	//! Returns whether the button is entered or not.
	/*!
		\return Whether the button is entered or not.
	*/
	bool isEntered() const;
	//! Returns the tween relating to the button's x-ccordinate.
	/*!
		\return The tween relating to the button's x-ccordinate.
	*/
	FD_Tween* getTweenX();
	//! Returns the tween relating to the button's y-ccordinate.
	/*!
		\return The tween relating to the button's y-ccordinate.
	*/
	FD_Tween* getTweenY();
	//! Returns the tween relating to the button's width.
	/*!
		\return The tween relating to the button's width.
	*/
	FD_Tween* getTweenWidth();
	//! Returns the tween relating to the button's height.
	/*!
		\return The tween relating to the button's height.
	*/
	FD_Tween* getTweenHeight();
	//! Returns the tween relating to the button's opacity.
	/*!
		\return The tween relating to the button's opacity.
	*/
	FD_Tween* getTweenOpacity();

};

//! The FD_BasicButton, a simple button class.
class FD_BasicButton : public FD_Button, public std::enable_shared_from_this<FD_BasicButton> {
private:

	FD_PureElement* pe_fore;
	FD_PureElement* pe_back;
	std::shared_ptr<FD_PureImage> pure_image;

	SDL_Rect* back_dstrect{ nullptr };
	SDL_Rect* fore_dstrect{ nullptr };

	void updateColour() override;

public:

	//! Constructs a FD_BasicButton.
	/*!
		\param temp    The template for this button.
		\param x       The x-coordinate of the button.
		\param y       The y-coordinate of this button.
		\param code    The code corresponding to this button. 
		\param overlay The image that overlays the button background.
	*/
	FD_BasicButton(const FD_ButtonTemplate& temp,
		const int x, const int y,
		const int code,
		const std::weak_ptr<FD_Image> overlay);
	//! Destroys the FD_BasicButton.
	~FD_BasicButton();

	//! Performs a release action on the button.
	bool release() override;
	//! Resets the state of the button (whether it's selected or held).
	void reset() override;
	//! Updates the button.
	/*!
		\param activity The current input state.
	*/
	void update(FD_ButtonActivity activity) override;
	//! Checks whether the button is selected by the mouse.
	/*!
		\param a The input state to check under.

		\return Whether the button is selected by the mouse.
	*/
	bool mouseSelected(FD_ButtonActivity a) const override;

	//! Adds the button to the object group.
	/*!
		\param group The object group to add to.
	*/
	void assimilate(std::shared_ptr<FD_ObjectGroup> group) override;

};

//! The FD_DropdownButton, a button class allowing for dropdown selection.
class FD_DropdownButton : public FD_Button, public std::enable_shared_from_this<FD_DropdownButton> {
private:

	size_t option{ 0 };
	std::vector<int> codes;

	std::vector<std::shared_ptr<FD_Image>> option_images;
	std::vector<std::shared_ptr<FD_Element>> option_elements;

	std::shared_ptr<FD_Box> backing;
	std::shared_ptr<FD_Box> selection;

	bool has_background{ false };
	std::shared_ptr<FD_Element> bg;
	FD_PureElement* bg_elem;
	std::shared_ptr<FD_PureImage> bg_cpy;

	int drop_selection{ -1 };
	bool dropped{ false };
	Uint32 unhovered_ticks{ 0 };
	const Uint32 unhovered_cap{ 30 };
	void configureDropSelection(int dms);

	void updateElements();
	void updateColour() override;

public:

	//! Constructs a FD_DropdownButton.
	/*!
		\param temp         The template for this button.
		\param x            The x-coordinate of the button.
		\param y            The y-coordinate of this button.
		\param codes        The code corresponding to this button.
		\param overlays     The image that overlays the button background.
		\param preselection The dropdown element that is selected upon initialisation.
	*/
	FD_DropdownButton(const FD_ButtonTemplate& temp,
		const int x, const int y,
		const std::vector<int> codes,
		const std::vector<std::weak_ptr<FD_Image>> overlays,
		const size_t preselection);
	//! Destroys the FD_DropdownButton.
	~FD_DropdownButton();
	
	//! Performs a release action on the button.
	bool release() override;
	//! Resets the state of the button (whether it's selected or held).
	void reset() override;
	//! Updates the button.
	/*!
		\param activity The current input state.
	*/
	void update(FD_ButtonActivity activity) override;
	
	//! Performs an enter action on the button.
	/*!
		\sa FD_BUTTON_ENTER
	*/
	void enter() override;
	//! Performs an up action on the button when it is entered.
	/*!
		This allows the button to override the upward action whilst entered.
	*/
	void buttonUp() override;
	//! Performs a down action on the button when it is entered.
	/*!
		This allows the button to override the downward action whilst entered.
	*/
	void buttonDown() override;
	//! Performs an exit action on the button.
	/*!
		\sa FD_BUTTON_EXIT
	*/
	void exit() override;

	//! Checks whether the button is selected by the mouse.
	/*!
		\param a The input state to check under.

		\return Whether the button is selected by the mouse.
	*/
	bool mouseSelected(FD_ButtonActivity a) const override;
	//! Checks which dropdown button is selected by the mouse.
	/*!
		\param a The input state to check under.

		\return The index of the button selected by the mouse, -1 if none are selected.
	*/
	int dropMouseSelected(FD_ButtonActivity a) const;

	//! Adds the button to the object group.
	/*!
		\param group The object group to add to.
	*/
	void assimilate(std::shared_ptr<FD_ObjectGroup> group) override;


};

#endif