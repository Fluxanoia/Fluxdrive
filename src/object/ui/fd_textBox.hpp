#ifndef FD_TEXTBOX_HPP_
#define FD_TEXTBOX_HPP_

#include "../fd_object.hpp"
#include "../../maths/fd_timer.hpp"
#include "../../main/fd_handling.hpp"
#include "../../display/fd_scene.hpp"

/*!
	@file
	@brief The file containing the class allowing large text display.
*/

//! The struct containing text formatting information.
typedef struct FD_TextTemplate_ {
	//! The font used for the text.
	std::shared_ptr<FD_Font> font;
	//! The colour of the font.
	SDL_Colour font_colour;
	//! The colour behind of selected text.
	SDL_Colour selection_colour;
	//! The colour of selected text.
	SDL_Colour selection_text_colour;
	//! The width of the text texture.
	Uint32 box_width;
	//! The height of the text texture.
	Uint32 box_height;
	//! Whether the box should scroll horizontally, rather than vertically.
	bool horz_scroll{ true };
	//! The spacing between lines.
	Uint32 line_spacing{ 4 };
	//! The time in ms between caret blinks.
	Uint32 caret_blink_delay{ 500 };
} FD_TextTemplate;

//! The struct containing text information.
typedef struct FD_TextInfo_ {
	//! The text.
	std::string text{ "" };
	//! The caret position.
	size_t caret_pos{ 0 };
	//! The index where a selection begins.
	size_t selection_start{ 0 };
	//! The index where the selection ends.
	size_t selection_end{ 0 };
} FD_TextInfo;

//! The class allowing for larger text container display and editing.
class FD_TextBox : public FD_Object, public std::enable_shared_from_this<FD_TextBox>  {
private:

	typedef struct LineSection_ {
		std::string text;
		Uint32 x, y;
		int w, h;
		bool sel;
		size_t index_bound;
	} LineSection;

	std::weak_ptr<FD_Scene> scene{};
	std::weak_ptr<FD_ObjectGroup> object_group{};

	std::shared_ptr<FD_PureImage> pure_image;
	std::vector<FD_PureElement*> pure_elements{};
	std::vector<std::shared_ptr<FD_TextImage>> text_images{ };
	std::vector<std::shared_ptr<FD_Box>> boxes{ };

	int caret_x{ 0 }, caret_y{ 0 };
	SDL_Rect* caret_rect;
	std::shared_ptr<FD_Box> caret;

	FD_Timer* caret_timer;
	FD_Tween* line_offset;
	std::vector<LineSection> lines{ };
	const FD_TextTemplate type_temp;

	FD_TextInfo text_info;
	bool editing{ false };

	void updateHorizontalImage();
	void updateVerticalImage();

	void clearPureElements();
	void prepareRender();
	void updateBoxes();

public:

	//! Constructs a FD_TextBox.
	/*!
		\param s            The scene used to create the image.
		\param type_temp    The template for the text.
		\param x            The x-coordinate of the box.
		\param y            The y-coordinate of the box.
		\param z            The layer of the box.
		\param camera_bound Whether the box is camera_bound or not.
		\param style        The drawing style of the box. 
	*/
	FD_TextBox(std::weak_ptr<FD_Scene> s,
		const FD_TextTemplate& type_temp,
		int x, int y, int z, 
		bool camera_bound, FD_DrawStyle style);
	//! Destroys the FD_TextBox.
	~FD_TextBox();

	//! Wakes the text box.
	void wake();

	//! Updates the text box.
	void update();

	//! Changes the box's text.
	/*!
		\param text The new text information.
	*/
	void changeText(const std::string text);
	//! Changes the box's text.
	/*!
		\param info The new text information.
	*/
	void changeText(const FD_TextInfo info);
	//! Updates the caret's position.
	void updateCaret();
	//! Changes the caret's position.
	/*!
		\param pos The new caret position.
	*/
	void updateCaret(size_t pos);

	//! Sets whether the box is being edited or not.
	/*!
		When in 'edit' mode, the box displays selections
		and the caret.

		\param edit Whether the box is being edited or not.
	*/
	void setEditing(bool edit);

	//! Returns the FD_Image associated with the object.
	/*!
		\return The image of the object.
	*/
	std::shared_ptr<FD_Image> getImage() const override;
	//! Returns the current text information.
	/*!
		\return The current text information.
	*/
	FD_TextInfo getTextInfo() const;
	//! Returns whether the box is being edited or not.
	/*!
		When in 'edit' mode, the box displays selections
		and the caret.

		\return Whether the box is being edited or not.
	*/
	bool isEditing() const;

	//! Returns the tween corresponding to the x coordinate.
	/*!
		\return The tween corresponding to the x coordinate.
	*/
	FD_Tween* getTweenX();
	//! Returns the tween corresponding to the y coordinate.
	/*!
		\return The tween corresponding to the y coordinate.
	*/
	FD_Tween* getTweenY();

	//! Adds the box to the object group.
	/*!
		\param group The object group to add to.
	*/
	void assimilate(std::shared_ptr<FD_ObjectGroup> group) override;

};

#endif
