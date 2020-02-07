#ifndef FD_VARIABLE_TEXT_H_
#define FD_VARIABLE_TEXT_H_

#include <memory>
#include <string>

#include "../fd_object.hpp"
#include "../../maths/fd_tween.hpp"
#include "../../display/fd_scene.hpp"
#include "../../input/fd_imageManager.hpp"

/*!
	@file
	@brief The file containing the FD_Text class.
*/

//! The FD_Text class, allowing for text image manipulation.
class FD_Text : public FD_Object, public std::enable_shared_from_this<FD_Object> {
private:

	SDL_Renderer* renderer;
	std::shared_ptr<FD_TextImage> image;

	void updateBounds();

public:

	//! Constructs a FD_Text.
	/*!
		This class consists of three text elements, the prefix, main text and, suffix.
		The prefix and suffix are input on construction and never change. 
		The main text is input on construction but can be changed at anytime with
		changeText - reproducing the texture again with the new main body.

		This system is useful for displaying scores, the prefix could be "You have "
		and the suffix could be " points.".

		\param renderer     The renderer used to produce the text texture.
		\param font         The font used by the text.
		\param prefix       The (fixed) prefix of the text.
		\param text         The variable main text.
		\param suffix       The (fixed) suffix of the text.
		\param colour       The colour of the text.
		\param x            The x-coordinate of the text.
		\param y            The y-coordinate of the text.
		\param style        The drawing style of the text, see FD_DrawStyle.
		\param z            The layer of the text.
		\param camera_bound Whether the text's coordinates are relative to the camera.

		\sa changeText
	*/
	FD_Text(SDL_Renderer* renderer, std::shared_ptr<FD_Font> font,
		std::string prefix,
		std::string text,
		std::string suffix,
		SDL_Colour colour,
		double x, double y,
		FD_DrawStyle style,
		int z, bool camera_bound);
	//! Destroys the FD_Text.
	~FD_Text();

	//! Updates the class.
	void update();
	//! Changes the main text of the class, and re-renders the texture.
	/*!
		\param text The new main text for the class.
	*/
	void changeText(std::string text);

	//! Sets whether the text is visible or not.
	/*!
		\param visible Whether the text should be visible or not.
	*/
	void setVisible(bool visible);
	//! Sets the overlay colour of the text.
	/*!
		The overlay colour is drawn after the underlay colour and text.

		\param colour The new overlay colour of the text.
	*/
	void setOverlayColour(SDL_Colour colour);
	//! Sets the underlay colour of the text.
	/*!
		The underlay colour is drawn before the text and overlay colour.

		\param colour The new underlay colour of the text.
	*/
	void setUnderlayColour(SDL_Colour colour);

	//! Returns the text image.
	/*
		\return The text image.	
	*/
	std::shared_ptr<FD_Image> getImage() const override;

	//! Returns the tween for the x-coordinate.
	/*!
		\return The tween for the x-coordinate.
	*/
	FD_Tween* getTweenX();
	//! Returns the tween for the y-coordinate.
	/*!
		\return The tween for the y-coordinate.
	*/
	FD_Tween* getTweenY();
	//! Returns the tween for the horizontal scale.
	/*!
		\return The tween for the horizontal scale.
	*/
	FD_Tween* getTweenScaleX();
	//! Returns the tween for the vertical scale.
	/*!
		\return The tween for the vertical scale.
	*/
	FD_Tween* getTweenScaleY();
	//! Returns the tween for the opacity.
	/*!
		\return The tween for the opacity.
	*/
	FD_Tween* getTweenOpacity();

	//! Adds the text to the object group.
	/*!
		\param group The object group to add to.
	*/
	void assimilate(std::shared_ptr<FD_ObjectGroup> group) override;

};

#endif
