#ifndef FD_INPUT_BOX_HPP_
#define FD_INPUT_BOX_HPP_

#include <memory>

#include "fd_button.hpp"
#include "fd_textBox.hpp"
#include "../../maths/fd_tween.hpp"

/*!
	@file
	@brief The file containing the class allowing text input.
*/

//! The FD_TextField, a text input box and a button.
class FD_TextField : public FD_Button, public std::enable_shared_from_this<FD_TextField> {
private:

	enum InputAction {
		CUT, COPY, PASTE, SELECT_ALL
	};

	typedef struct LineSection_ {
		std::string text;
		Uint32 x, y;
		int w, h;
		bool sel;
	} LineSection;

	std::vector<FD_PureElement*> elements;
	std::shared_ptr<FD_PureImage> pure_image;

	bool focus{ false };
	std::shared_ptr<FD_InputSet> input;
	std::shared_ptr<FD_TextBox> text_box;
	int padding_top, padding_left, padding_bottom, padding_right;

	void updateColour();

	void changeTextBox();

public:

	//! Constructs a FD_TextField.
	/*!
		\param temp             The template for this button.
		\param type_temp	    The template for the text.
		\param input_list       The ID of the input list for this box to use.
		\param x                The x-coordinate of this button.
		\param y                The y-coordinate of this button.
		\param code		        The code corresponding to this button.
		\param padding_left     The padding to the left of the text.
		\param padding_top      The padding above the text.
		\param padding_right    The padding to the right of the text.
		\param padding_bottom   The padding below the text.
	*/
	FD_TextField(const FD_ButtonTemplate& temp,
		const FD_TextTemplate& type_temp,
		int input_list, int x, int y, int code,
		int padding_left, int padding_top, 
		int padding_right, int padding_bottom);
	//! Destroys the FD_TextField.
	~FD_TextField();

	//! Performs a press action on the button.
	void press() override;
	//! Performs a release action on the button.
	bool release() override;
	//! Resets the state of the button (whether it's selected or held).
	void reset() override;
	
	//! Wakes the text field.
	void wake();
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

#endif
