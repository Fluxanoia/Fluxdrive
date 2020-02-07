#ifndef FD_ELEMENT_H_
#define FD_ELEMENT_H_

#include "fd_object.hpp"
#include "../maths/fd_tween.hpp"
#include "../input/fd_imageManager.hpp"

/*!
	@file
	@brief The file containing FD_Element class, a basic image object.
*/

//! The FD_Element class, allowing the manipulation of an image object.
class FD_Element : public FD_Object, public std::enable_shared_from_this<FD_Element> {
private:

	std::weak_ptr<FD_Image> image;

	void updateBounds();

public:
	
	//! Constructs a FD_Element.
	/*!
		Note, if the scale_y is less than zero, then it will use the
		scale parameter as the vertical and horizontal scale.
		By default, scale_y is less than zero.

		\param image        The image to be used.
		\param x            The x-coordinate of the element.
		\param y            The y-coordinate of the element.
		\param angle        The angle of the element.
		\param z            The layer of the element.
		\param scale        The horizontal scale for the element.
		\param scale_y      The vertical scale for the element.
		\param camera_bound Whether the element should be drawn relative to the camera. 
		\param style        The drawing style for the element (see FD_DrawStyle).
	*/
	FD_Element(std::weak_ptr<FD_Image> image, double x, double y,
		double angle, int z, double scale = 1, double scale_y = -1, 
		bool camera_bound = false, FD_DrawStyle style = FD_CENTERED);
	//! Destroys the FD_Element.
	~FD_Element();

	//! Updates the element.
	void update();

	//! Returns the image.
	/*!
		\return image The element's image.
	*/
	std::shared_ptr<FD_Image> getImage() const override;
	//! Returns the width of the element.
	/*!
		Note that this is the drawn width of the element
		(so after source rect and scaling calculations).

		\return The width of the element.
	*/
	double getWidth() const override;
	//! Returns the height of the element.
	/*!
		Note that this is the drawn height of the element
		(so after source rect and scaling calculations).

		\return The height of the element.
	*/
	double getHeight() const override;

	//! Sets the width of the element.
	/*!
		Note that this sets the drawn width, and does so by
		altering the scale of the element.
		
		\param width The new width of the element.
	*/
	void setWidth(int width);
	//! Sets the height of the element.
	/*!
		Note that this sets the drawn height, and does so by
		altering the scale of the element.

		\param height The new height of the element.
	*/
	void setHeight(int height);
	//! Sets whether the element is visible or not.
	/*!
		\param visible Whether the element is visible or not.
	*/
	void setVisible(bool visible);
	//! Sets the overlay colour of the element.
	/*!
		The overlay colour is drawn over the image.
		
		\param colour The new overlay colour.
	*/
	void setOverlayColour(SDL_Colour colour);
	//! Sets the underlay colour of the element.
	/*!
		The underlay colour is drawn under the image.

		\param colour The new underlay colour.
	*/
	void setUnderlayColour(SDL_Colour colour);
	//! Sets the image of the element.
	/*!
		\param image The new image.
	*/
	void setImage(std::weak_ptr<FD_Image> image);
	//! Sets the source rectangle.
	/*!
		The source rectangle is the section of the given
		image that is drawn. It is nullptr initially 
		causing the whole image to be drawn. It can be
		returned to nullptr by removeSourceRect.

		\param rect The new source rectangle.

		\sa removeSourceRect
	*/
	void setSourceRect(SDL_Rect rect);
	//! Sets the flipping flags.
	/*!
		\param f The new flipping flags.
	*/
	void setFlipFlags(SDL_RendererFlip f);

	//! Removes the given source rectangle.
	/*!
		\sa setSourceRect
	*/
	void removeSourceRect();

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
	//! Returns the tween for the angle.
	/*!
		\return The tween for the angle.
	*/
	FD_Tween* getTweenAngle();
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
	//! Returns the tween for the center x-coordinate.
	/*!
		\return The tween for the center x-coordinate.
	*/
	FD_Tween* getTweenCenterX();
	//! Returns the tween for the center y-coordinate.
	/*!
		\return The tween for the center y-coordinate.
	*/
	FD_Tween* getTweenCenterY();

	//! Adds the element to the object group.
	/*!
		\param group The object group to add to.
	*/
	void assimilate(std::shared_ptr<FD_ObjectGroup> group) override;

};

#endif
