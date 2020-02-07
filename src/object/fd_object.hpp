#ifndef FD_OBJECT_H_
#define FD_OBJECT_H_

#include <vector>
#include <memory>

#include "fd_objectGroup.hpp"
#include "../maths/fd_tween.hpp"
#include "../main/fd_handling.hpp"
#include "../input/fd_imageManager.hpp"

/*!
	@file
	@brief The file containing the FD_Object class, and some derived geometry elements.
*/

//! The enumeration containing interpretations of coordinates.
enum FD_DrawStyle {
	//! The object is drawn with its coordinate in the top left.
	FD_TOP_LEFT, 
	//! The object is drawn with its coordinate in the top right.
	FD_TOP_RIGHT,
	//! The object is drawn with its coordinate in the center.
	FD_CENTERED,
	//! The object is drawn with its coordinate in the bottom left.
	FD_BOTTOM_LEFT,
	//! The object is drawn with its coordinate in the bottom right.
	FD_BOTTOM_RIGHT
};

//! The FD_Line class, the basic line object.
class FD_Line : public FD_Layered, public std::enable_shared_from_this<FD_Line> {
private:

private:

	SDL_Colour colour;
	FD_Tween* x1;
	FD_Tween* y1;
	FD_Tween* x2;
	FD_Tween* y2;
	FD_Tween* opacity;

	bool visible{ true };

	SDL_Point* p1{ nullptr };
	SDL_Point* p2{ nullptr };

public:

	//! Constructs a FD_Line.
	/*!
		\param x1           The x-coordinate of the first point of the line.
		\param y1           The y-coordinate of the first point of the line.
		\param x2           The x-coordinate of the second point of the line.
		\param y2           The y-coordinate of the second point of the line.
		\param z            The layer of the line.
		\param camera_bound Whether the object is drawn relative to the camera.
		\param colour       The colour of the line.
	*/
	FD_Line(double x1, double y1, double x2, double y2, int z,
		bool camera_bound,
		SDL_Colour colour = { 255, 255, 255, 255 });
	//! Destroys the FD_Line.
	~FD_Line();

	//! Supplies pointers to points to the line.
	/*!
		The line will use the value of the supplied points whenever
		possible. Otherwise, it will fallback to the last given value.
		The supplied points can be wiped with removePoints.

		\sa removePoints
	*/
	void supplyPoints(SDL_Point* p1, SDL_Point* p2);
	//! Wipes the supplied points.
	/*!
		\sa supplyPoints
	*/
	void removePoints();

	//! Renders the FD_Line.
	/*!
		\param renderer The renderer to be used.
		\param alpha    The overall opacity, usually given by an object group - applied multiplicatively to the FD_Line opacity.
		\param camera   The camera to use for camera bound operations. 
	*/
	void render(SDL_Renderer* renderer, const Uint8 alpha,
		const std::shared_ptr<const FD_Camera> camera) const override;

	//! Sets the colour of the line.
	/*!
		\param colour The colour to be changed to.
	*/
	void setColour(SDL_Colour colour);
	//! Set the visibility of the line.
	/*!
		\param visible Whether the line should be visible or not.
	*/
	void setVisible(bool visible);

	//! Returns the colour of the line.
	/*!
		\return The colour of the line.
	*/
	SDL_Colour getColour() const;
	//! Returns whether the line is visible or not.
	/*!
		\return Whether the line is visible or not.
	*/
	bool isVisible() const;

	//! Returns the x-coordinate of the first point.
	/*!
		Note that this method does not access the value of any
		supplied pointers.

		\return The x-coordinate of the first point.
	*/
	double getX1() const;
	//! Returns the y-coordinate of the first point.
    /*!
		Note that this method does not access the value of any
		supplied pointers.

		\return The y-coordinate of the first point.
	*/
	double getY1() const;
	//! Returns the x-coordinate of the second point.
    /*!
		Note that this method does not access the value of any
		supplied pointers.

		\return The x-coordinate of the second point.
	*/
	double getX2() const;
	//! Returns the y-coordinate of the second point.
	/*!
		Note that this method does not access the value of any
		supplied pointers.

		\return The y-coordinate of the second point.
	*/
	double getY2() const;
	//! Returns the opacity of the line.
	/*!
		\return The opacity of the line.
	*/
	Uint8 getOpacity() const;

	//! Sets the values of the coordinates of the line.
	/*!
		Note, this sets the 'fallback' value of the coordinates.
		In other words, it will not change the value of points
		given to the line in the supplyPoints method.

		\sa supplyPoints
	*/
	void updatePoints(double x1, double y1, double x2, double y2);
	//! Returns the tween of the x-coordinate of the first point.
	/*!
		\return The tween of the x-coordinate of the first point.
	*/
	FD_Tween* getTweenX1();
	//! Returns the tween of the y-coordinate of the first point.
	/*!
		\return The tween of the y-coordinate of the first point.
	*/
	FD_Tween* getTweenY1();
	//! Returns the tween of the x-coordinate of the second point.
	/*!
		\return The tween of the x-coordinate of the second point.
	*/
	FD_Tween* getTweenX2();
	//! Returns the tween of the y-coordinate of the second point.
	/*!
		\return The tween of the y-coordinate of the second point.
	*/
	FD_Tween* getTweenY2();
	//! Returns the tween of the opacity.
	/*!
		\return The tween of the opacity.
	*/
	FD_Tween* getTweenOpacity();

	//! Gets the line to register itself with the given object group.
	/*!
		\param group The object group for the line to register with.	
	*/
	void assimilate(std::shared_ptr<FD_ObjectGroup> group);

};

//! The FD_Box class, the basic rectangle object.
class FD_Box : public FD_Layered, public std::enable_shared_from_this<FD_Box> {
private:

	SDL_Colour underlay_colour{ 0, 0, 0, 0 };
	SDL_Colour colour;
	SDL_Colour overlay_colour{ 0, 0, 0, 0 };
	FD_Tween* x;
	FD_Tween* y;
	FD_Tween* w;
	FD_Tween* h;
	FD_Tween* opacity;

	bool visible{ true };

	SDL_Rect* rect{ nullptr };

public:

	//! Constructs a FD_Line.
	/*!
		\param x            The x-coordinate of the rectangle.
		\param y            The y-coordinate of the rectangle.
		\param w            The width of the rectangle.
		\param h            The height of the rectangle.
		\param z            The layer of the line.
		\param camera_bound Whether the object is drawn relative to the camera.
		\param colour       The colour of the line.
	*/
	FD_Box(double x, double y, double w, double h, int z,
		bool camera_bound,
		SDL_Colour colour = { 255, 255, 255, 255 });
	//! Destroys the FD_Box.
	~FD_Box();

	//! Supplies a pointer to a rectangle to the box.
	/*!
		The box will use the values of the supplied rectangle whenever
		possible. Otherwise, it will fallback to the last given values.
		The supplied rectangle can be wiped with removeRect.

		\sa removeRect
	*/
	void supplyRect(SDL_Rect* rect);
	//! Wipes the supplied rectangle.
	/*!
		\sa supplyRect
	*/
	void removeRect();

	//! Renders the FD_Box.
	/*!
		\param renderer The renderer to be used.
		\param alpha    The overall opacity, usually given by an object group - applied multiplicatively to the FD_Box opacity.
		\param camera   The camera to use for camera bound operations. 
	*/
	void render(SDL_Renderer* renderer, const Uint8 alpha,
		const std::shared_ptr<const FD_Camera> camera) const override;

	//! Sets the colour of the outline of the box.
	/*!
		This is rendered after the underlay and before the overlay.

		\param colour The colour for the outline of the box.

		\sa setUnderlayColour
		\sa setOverlayColour
	*/
	void setColour(SDL_Colour colour);
	//! Sets the colour of the underlay of the box.
	/*!
		This is rendered before the outline and overlay.

		\param colour The colour for the underlay of the box.

		\sa setColour
		\sa setOverlayColour
	*/
	void setUnderlayColour(SDL_Colour colour);
	//! Sets the colour of the overlay of the box.
	/*!
		This is rendered after the outline and underlay.

		\param colour The colour for the overlay of the box.

		\sa setColour
		\sa setUnderlayColour
	*/
	void setOverlayColour(SDL_Colour colour);
	//! Set the visibility of the line.
	/*!
		\param visible Whether the line should be visible or not.
	*/
	void setVisible(bool visible);

	//! Returns the colour of the underlay of the box.
	/*!
		\return The colour of the underlay of the box.
	*/
	SDL_Colour getUnderlayColour() const;
	//! Returns the colour of the outline of the box.
	/*!
		\return The colour of the outline of the box.
	*/
	SDL_Colour getColour() const;
	//! Returns the colour of the overlay of the box.
	/*!
		\return The colour of the overlay of the box.
	*/
	SDL_Colour getOverlayColour() const;
	//! Returns whether the box is visible or not.
	/*!
		\return Whether the box is visible or not.
	*/
	bool isVisible() const;

	//! Returns the x-coordinate of the box.
	/*!
		Note that this method does not access the value of the
		supplied pointer.

		\return The x-coordinate of the box.
	*/
	double getX() const;
	//! Returns the y-coordinate of the box.
	/*!
		Note that this method does not access the value of the
		supplied pointer.

		\return The y-coordinate of the box.
	*/
	double getY() const;
	//! Returns the width of the box.
	/*!
		Note that this method does not access the value of the
		supplied pointer.

		\return The width of the box.
	*/
	double getWidth() const;
	//! Returns the height of the box.
	/*!
		Note that this method does not access the value of the
		supplied pointer.

		\return The height of the box.
	*/
	double getHeight() const;
	//! Returns the opacity of the box.
	/*!
		\return The opacity of the box.
	*/
	Uint8 getOpacity() const;

	//! Sets the dimensions of the box.
	/*!
		Note, this sets the 'fallback' values of the rectangle.
		In other words, it will not change the values of the
		rectangle given to the class in the supplyRect method.

		\sa supplyRect
	*/
	void updateRect(double x, double y, double w, double h);
	//! Returns the tween of the x-coordinate of the box.
	/*!
		\return The tween of the x-coordinate of the box.
	*/
	FD_Tween* getTweenX();
	//! Returns the tween of the y-coordinate of the box.
	/*!
		\return The tween of the y-coordinate of the box.
	*/
	FD_Tween* getTweenY();
	//! Returns the tween of the width of the box.
	/*!
		\return The tween of the width of the box.
	*/
	FD_Tween* getTweenWidth();
	//! Returns the tween of the height of the box.
	/*!
		\return The tween of the height of the box.
	*/
	FD_Tween* getTweenHeight();
	//! Returns the tween of the opacity of the box.
	/*!
		\return The tween of the opacity of the box.
	*/
	FD_Tween* getTweenOpacity();

	//! Gets the box to register itself with the given object group.
	/*!
		\param group The object group for the box to register with.
	*/
	void assimilate(std::shared_ptr<FD_ObjectGroup> group);

};

//! The virtual FD_Object class, the framework for the individual elements drawn by the engine.
class FD_Object : public FD_Layered {
protected:

	//! The tween corresponding to the x-coordinate.
	FD_Tween* x;
	//! The tween corresponding to the y-coordinate.
	FD_Tween* y;
	//! The tween corresponding to the width.
	FD_Tween* w;
	//! The tween corresponding to the height.
	FD_Tween* h;
	//! The tween corresponding to the angle.
	FD_Tween* angle;
	//! The tween corresponding to the horizontal scale.
	FD_Tween* scale_w;
	//! The tween corresponding to the vertical scale.
	FD_Tween* scale_h;
	//! The tween corresponding to the opacity.
	FD_Tween* opacity;
	//! Whether the object is visible or not.
	bool visible{ true };

	//! The source rectangle of the object.
	SDL_Rect* srcrect{ nullptr };
	//! The destination rectangle of the object.
	/*!
		It is advised that you use updateBounds to some capacity
		to assign this variable.
	*/
	SDL_Rect* dstrect{ nullptr };
	//! The tween corresponding to the x-coordinate of the rotational center.
	/*!
		This value is relative to the x-coordinate and is in units of the width of
		the object.
	*/
	FD_Tween* center_x;
	//! The tween corresponding to the y-coordinate of the rotational center.
	/*!
		This value is relative to the y-coordinate and is in units of the height of
		the object.
	*/
	FD_Tween* center_y;

	//! The draw style of the object.
	FD_DrawStyle draw_style{ FD_CENTERED };
	//! The flip flags of the object.
	SDL_RendererFlip flip_flags{ SDL_FLIP_NONE };

	//! This updates the destination rectangle of the object.
	/*!
		\param rect  The pointer to the rectangle to update.
		\param x     The x-coordinate of the rectangle.
		\param y     The y-coordinate of the rectangle.
		\param w     The width of the rectangle.
		\param h     The height of the rectangle.
		\param style The drawing style of the destination rectangle.
	*/
	void updateBounds(SDL_Rect* rect, double x, double y,
		double w, double h, FD_DrawStyle style);

public:

	//! Constructs a FD_Object.
	/*!
		Note that this class is simply a framework, and whilst some of these
		variables may not make sense for certain implementations of
		FD_Object - their core meaning should be maintained where possible.
		
		For example, there's no general notion of the 'width' of a line,
		but the FD_Line class maintains the width as the absolute distance
		between its points in the x-axis. Similarly, the angle is maintained
		at zero.

		\param z            The layer of the object.
		\param camera_bound Whether the object is drawn relative to the camera.
		\param x            The x-coordinate of the object.
		\param y            The y-coordinate of the object.
		\param w            The width of the object.
		\param h            The height of the object.
		\param angle        The angle of the object.
		\param scale_w      The horizontal scale of the object.
		\param scale_h      The vertical scale of the object.
		\param opacity      The opacity of the object.
		\param visible      Whether the object is visible or not.
	*/
	FD_Object(int z, bool camera_bound,
		double x = 0, double y = 0, double w = 0, double h = 0,
		double angle = 0, double scale_w = 1, double scale_h = 1,
		Uint8 opacity = 255, bool visible = true);
	~FD_Object();

	//! Renders the object.
	/*!
		\param renderer The renderer to be used.
		\param alpha    The overall opacity, usually given by an object group - applied multiplicatively to the FD_Box opacity.
		\param camera   The camera to use for camera bound operations.
	*/
	void render(SDL_Renderer* renderer, const Uint8 alpha,
		const std::shared_ptr<const FD_Camera> camera) const override;

	//! Returns the x-coordinate of the object.
	/*!
		\return The x-coordinate of the object.
	*/
	virtual double getX() const;
	//! Returns the y-coordinate of the object.
	/*!
		\return The y-coordinate of the object.
	*/
	virtual double getY() const;
	//! Returns the width of the object.
	/*!
		\return The width of the object.
	*/
	virtual double getWidth() const;
	//! Returns the height of the object.
	/*!
		\return The height of the object.
	*/
	virtual double getHeight() const;
	//! Returns the horizontal scale of the object.
	/*!
		\return The horizontal scale of the object.
	*/
	virtual double getWidthScale() const;
	//! Returns the vertical scale of the object.
	/*!
		\return The vertical scale of the object.
	*/
	virtual double getHeightScale() const;
	//! Returns the visibility of the object.
	/*!
		\return The visibility of the object.
	*/
	virtual bool isVisible() const;

	//! Returns the FD_Image associated with the object.
	/*!
		There is no default for this method, it is the sole
		method that makes the object abstract.

		\return The image of the object.
	*/
	virtual std::shared_ptr<FD_Image> getImage() const = 0;
	//! Returns the source rectangle of the object.
	/*!
		\return The source rectangle of the object.
	*/
	virtual SDL_Rect* getSourceRect() const;
	//! Returns the destination rectangle of the object.
	/*!
		\return The destination rectangle of the object.
	*/
	virtual SDL_Rect* getDestinationRect() const;
	//! Returns the angle of the object.
	/*!
		\return The angle of the object.
	*/
	virtual double getAngle() const;
	//! Returns the center x-coordinate of the object.
	/*!
		This is used for rotations.

		\return The center x-coordinate of the object.
	*/
	virtual double getCenterX() const;
	//! Returns the center y-coordinate of the object.
	/*!
		This is used for rotations.

		\return The center y-coordinate of the object.
	*/
	virtual double getCenterY() const;
	//! Returns the rendering flipping flags of the object.
	/*!
		\return The rendering flipping flags of the object.
	*/
	virtual SDL_RendererFlip getFlipFlags() const;
	//! Returns the opacity of the object.
	/*!
		\return The opacity of the object.
	*/
	virtual Uint8 getOpacity() const;

};

#endif
