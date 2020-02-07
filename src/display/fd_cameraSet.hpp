#ifndef FD_CAMERA_SET_H_
#define FD_CAMERA_SET_H_

#include <unordered_map>

#include "fd_window.hpp"
#include "fd_camera.hpp"

/*!
	@file
	@brief This file contains code to allow the management of multiple FD_Camera instances.
*/

//! The default culling radius for instances of FD_Camera made by the FD_CameraSet.
#define FD_CAMERA_SET_DEFAULT_CULL 16

//! Defines a type for the indices of FD_Camera.
typedef int FD_CameraIndex;

//! Defines the types of camera transitions.
enum FD_CameraTransition {
	//! Instantly switches focus to the next FD_Camera
	FD_CAMERA_TRAN_INSTANT,
	/*!
		Tweens the current FD_Camera onto the next
		one then switches focus to the next one. The original
		FD_Camera will have the dimensions and position of
		the new one.
	*/
	FD_CAMERA_TRAN_SMOOTH,
	/*!
		Tweens a temporary intermidiary FD_Camera onto
		the next one then switches focus to the next one.
		The original FD_Camera will not have its dimensions changed.
	*/
	FD_CAMERA_TRAN_SMOOTH_PRESERVED
};

//! The class that allows the management of multiple instances of FD_Camera.
class FD_CameraSet : public FD_Resizable {
private:

	std::weak_ptr<FD_Window> window;

	size_t camera_id_count{ 0 };
	FD_CameraTransition cam_tran{ FD_CAMERA_TRAN_INSTANT };
	FD_TweenType cam_tran_type{ FD_TWEEN_LINEAR };
	Uint32 cam_tran_duration{ 1000 };

	FD_CameraIndex current_camera{ 0 };
	std::unordered_map<FD_CameraIndex, 
		std::shared_ptr<FD_Camera>> cameras{};
	
	bool temping{ false };
	FD_CameraIndex next_camera;
	std::shared_ptr<FD_Camera> temp_camera;

	void transition(std::shared_ptr<FD_Camera> from,
		std::shared_ptr<FD_Camera> to);

public:

	//! Constructs an FD_CameraSet.
	/*!
		\param window The FD_Window that all cameras in this set will be associated with.
	*/
	FD_CameraSet(std::weak_ptr<FD_Window> window);
	//! Destroys the FD_CameraSet.
	~FD_CameraSet();

	//! Updates the FD_CameraSet.
	/*!
		This should be called frequently (usually on every update cycle).
	*/
	void update();
	//! Prepares all instances of FD_Camera for the manipulation associated with rendering.
	void pre_render();

	//! Transitions from the current FD_Camera to a different FD_Camera.
	/*!
		This functions based on the current camera transition and its type and duration.

		\param index The index of the FD_Camera to switch to.

		\sa setCameraTransition
		\sa setCameraTransitionType
		\sa setCameraTransitionDuration
	*/
	void transitionCamera(FD_CameraIndex index);
	//! Sets the type of the transitions.
	/*!
		\param tran The new transition type.

		\sa FD_CameraTransition
	*/
	void setCameraTransition(FD_CameraTransition tran);
	//! Sets the type of tween ran by transitions.
	/*!
		\param type The new tween type.

		\sa FD_TweenType
	*/
	void setCameraTransitionType(FD_TweenType type);
	//! Sets the duration of transitions.
	/*!
		\param duration The new duration.
	*/
	void setCameraTransitionDuration(Uint32 duration);

	//! Sets the current FD_Camera to the one associated with the given FD_CameraIndex.
	/*
		\param index The index of the FD_Camera to switch to.
	*/
	void setCamera(FD_CameraIndex index);
	//! Returns the FD_CameraIndex of the current camera.
	/*!
		\return The FD_CameraIndex of the current camera.
	*/
	FD_CameraIndex getCurrentCameraID() const;
	//! Adds an FD_Camera to the set.
	/*!
		\param width          The width of the new FD_Camera.
		\param culling_radius The culling radius of the new FD_Camera.

		\return The FD_CameraIndex of the new camera.

		\sa FD_Camera
	*/
	FD_CameraIndex addCamera(int width, 
		int culling_radius = FD_CAMERA_SET_DEFAULT_CULL);
	//! Returns a weak pointer to the FD_Camera associated to the given FD_CameraIndex.
	/*!
		\return A weak pointer to the FD_Camera associated to the given FD_CameraIndex.
	*/
	std::weak_ptr<FD_Camera> getCamera(FD_CameraIndex index) const;
	//! Returns a weak pointer to the current FD_Camera.
	/*!
		\return A weak pointer to the current FD_Camera.
	*/
	std::weak_ptr<FD_Camera> getCurrentCamera() const;

	//! Updates all instances of FD_Camera with a change in its associated FD_Window's dimensions.
	/*!
		\param width  The new width of the FD_Window.
		\param height The new height of the FD_Window.
	*/
	void resized(int width, int height);

};

#endif
