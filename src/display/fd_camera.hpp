#ifndef FD_CAMERA_H_
#define FD_CAMERA_H_

#include <cmath>

#include "fd_window.hpp"
#include "fd_resizable.hpp"
#include "../main/fd_handling.hpp"
#include "../maths/fd_tween.hpp"
#include "../maths/fd_maths.hpp"
#include "../maths/fd_randomGenerator.hpp"

/*!
	@file
	@brief A file containing the FD_Camera class.
*/

//! The FD_Camera class, allowing for abstract coordinate systems.
/*!
	The FD_Camera class works with FD_ObjectGroup and FD_Scene to
	translate and render images.
	FD_Camera does not draw itself, it manipulates destination rectangles
	to allow objects to be places appropriately.
*/
class FD_Camera : public FD_Resizable, 
	public std::enable_shared_from_this<FD_Camera> {
private:

	FD_Tween* x;
	FD_Tween* y;
	FD_Tween* w;
	FD_Tween* scale;
	FD_Tween* angle;

	int resolution_width;
	int resolution_height;
	double aspect_ratio;
	double drawing_scale;

	FD_RandomGenerator* random_x;
	FD_RandomGenerator* random_y;
	FD_RandomGenerator* random_angle;

	double shake_amount{ 0 };
	double max_x_shake{ 0 };
	double max_y_shake{ 0 };
	double max_angle_shake{ 10 };
	std::function<double(double)> shake_decrement{
		[](double s){ return s * 0.9; }
	};

	int culling_radius;
	SDL_Rect* culling_rect;
	SDL_Rect* bounds;
	SDL_Rect* bounding_rect;
	SDL_Point* bounds_center;

	int getRotatedWidth() const;
	int getRotatedHeight() const;

public:

	//! Constructs a FD_Camera.
	/*!
		\param camera_width   The width of the FD_Camera.
		\param culling_radius The extra length around the FD_Camera's bounds where objects should still be drawn.
	*/
	FD_Camera(int camera_width, int culling_radius);
	//! Destroys the FD_Camera.
	~FD_Camera();
	//! Associates the FD_Camera to the given FD_Window.
	/*!
		This should be performed after a FD_Camera is constructed to ensure it functions as intended,
		especially in regards to aspect ratio.
		
		\param window The FD_Window to associate the FD_Camera to.
	*/
	void associate(std::weak_ptr<FD_Window> window);

	//! Prepares the FD_Camera for the manipulation associated with rendering.
	void pre_render();
	//! Manipulates a given destination rectangle from arbitrary to window coordinates.
	/*!
		The given rectangle is changed by reference.

		\param dstrect The destination rectangle.
		\param angle   The angle of the destination rectangle.

		\return Returns whether the object should be drawn or not (if the destination intersects with the camera bounds).
	*/
	bool manipulate(SDL_Rect& dstrect, double& angle) const;

	//! Updates the FD_Camera.
	/*!
		This should be called frequently (usually on every update cycle).

		\param force Whether FD_Camera should update its bounds regardless of whether its tweens have changed.
	*/
	void update(bool force = false);

	//! Updates the FD_Camera with a change in its associated FD_Window's dimensions.
	/*!
		\param width  The new width of the FD_Window.
		\param height The new height of the FD_Window.
	*/
	void resized(int width, int height);

	//! Stills the FD_Camera, stopping all shaking.
	void still();
	//! Shakes the FD_Camera.
	/*!
		The amount is arbitrary and should be experimented with.
		Note that the input is added onto the current amount of shake.

		\param amount How much more the FD_Camera should shake.
	*/
	void shake(double amount);

	//! Converts the given point from window coordinates to the arbitrary coordinate system.
	/*!
		The given coordinates are changed by reference.

		\param x The x coordinate of the input point.
		\param y The y coordinate of the input point.
	*/
	void toCameraCoordinates(int& x, int& y) const;
	//! Converts the given point from the arbitrary coordinate system to screen coordinates.
	/*!
		The given coordinates are changed by reference.

		\param x The x coordinate of the input point.
		\param y The y coordinate of the input point.
	*/
	void toScreenCoordinates(int& x, int& y) const;
	//! Converts the given point from window coordinates to the arbitrary coordinate system.
	/*!
		The given coordinates are changed by reference.

		\param x The x coordinate of the input point.
		\param y The y coordinate of the input point.
	*/
	void toCameraCoordinates(double& x, double& y) const;
	//! Converts the given point from the arbitrary coordinate system to screen coordinates.
	/*!
		The given coordinates are changed by reference.

		\param x The x coordinate of the input point.
		\param y The y coordinate of the input point.
	*/
	void toScreenCoordinates(double& x, double& y) const;

	//! Returns the width of the FD_Camera.
	/*!
		\return The width of the FD_Camera after scaling and rounded.
	*/
	int getWidth() const;
	//! Returns the height of the FD_Camera.
	/*!
		\return The height of the FD_Camera after scaling and rounded.
	*/
	int getHeight() const;
	//! Returns the culling radius of the FD_Camera.
	/*!
		\return The culling radius of the FD_Camera.
	*/
	int getCullingRadius() const;
	//! Returns the max angle that the FD_Camera can rotate by whilst shaking.
	/*
		\return The max angle that the FD_Camera can rotate by whilst shaking.
	*/
	const double getMaxAngularShake() const;
	//! Returns the function that represents how the amount of shake changes over time.
	/*
		\return The function that represents how the amount of shake changes over time.
	*/
	const std::function<double(double)> getShakeDecrementer() const;

	//! Sets the width of the FD_Camera before scaling.
	/*!
		\param width The new width of the FD_Camera.
	*/
	void setWidth(double width);
	//! Sets the height of the FD_Camera before scaling.
	/*!
		\param height The new height of the FD_Camera.
	*/
	void setHeight(double height);
	//! Sets the culling radius of the FD_Camera.
	/*!
		\param radius The new culling radius of the FD_Camera.
	*/
	void setCullingRadius(int radius);
	//! Sets the max angle that the FD_Camera can rotate by whilst shaking.
	/*!
		\param angle The new max angle that the FD_Camera can rotate by whilst shaking.
	*/
	void setMaxAngularShake(double angle);
	//! Sets the function that represents how the amount of shake changes over time.
	/*!
		\param f The new function that represents how the amount of shake changes over time.
	*/
	void setShakeDecrementer(std::function<double(double)> f);

	//! Returns the current rectangle representing the area of the FD_Camera.
	/*!
		\return The current rectangle representing the area of the FD_Camera.
	*/
	const SDL_Rect* getBounds() const;
	//! Returns the current bounds of the FD_Camera padded with the culling radius.
	/*!
		\return The current bounds of the FD_Camera padded with the culling radius.
	*/
	const SDL_Rect* getCullingBounds() const;

	//! Returns the FD_Tween representing the x coordinate of the FD_Camera.
	/*!
		The value of this tween is the x coordinate of the centre of the FD_Camera.
		
		\returns The FD_Tween representing the x coordinate of the FD_Camera.

		\warning Do not delete this pointer.
	*/
	FD_Tween* getTweenX();
	//! Returns the FD_Tween representing the y coordinate of the FD_Camera.
	/*!
		The value of this tween is the y coordinate of the centre of the FD_Camera.

		\returns The FD_Tween representing the y coordinate of the FD_Camera.

		\warning Do not delete this pointer.
	*/
	FD_Tween* getTweenY();
	//! Returns the FD_Tween representing the width of the FD_Camera before scaling.
	/*!
		\returns The FD_Tween representing the width of the FD_Camera.

		\warning Do not delete this pointer.
	*/
	FD_Tween* getTweenWidth();
	//! Returns the FD_Tween representing the scale of the FD_Camera.
	/*!
		\returns The FD_Tween representing the scale of the FD_Camera.

		\warning Do not delete this pointer.
	*/
	FD_Tween* getTweenScale();
	//! Returns the FD_Tween representing the angle of the FD_Camera before scaling.
	/*!
		\returns The FD_Tween representing the angle of the FD_Camera.

		\warning Do not delete this pointer.
	*/
	FD_Tween* getTweenAngle();

	//! Returns whether any of the associated FD_Tweens of the FD_Camera are in motion.
	/*!
		The associated FD_Tweens correspond to the x, y, width, scale and, angle of the FD_Camera.

		\returns Whether any of the associated FD_Tweens of the FD_Camera are in motion.
	*/
	bool isMoving();

};

#endif