#ifndef FD_OBJECT_GROUP_H_
#define FD_OBJECT_GROUP_H_

#include <memory>
#include <vector>
#include <unordered_map>

#include <SDL_render.h>

#include "../maths/fd_tween.hpp"
#include "../display/fd_cameraSet.hpp"

/*!
	@file
	@brief The file containing the FD_Layered and FD_ObjectGroup classes pertaining to object layering.
*/

class FD_Scene;
class FD_Camera;
class FD_ObjectGroup;

//! The FD_Layered class, adds layering functionality to classes.
class FD_Layered {
private:

	int z;
	bool camera_bound;

	bool re_layered{ false };

public:

	//! Constructs a FD_Layered.
	/*!
		\param z            The initial layer.
		\param camera_bound Whether the object's coordinates are relative to the camera.
	*/
	FD_Layered(int z, bool camera_bound);
	//! Destroys the FD_Layered.
	~FD_Layered();

	//! Renders the object.
	/*!
		\param renderer The renderer used to draw the object.
		\param alpha    The overall opacity, usually of the overarching object group.
		\param camera   The camera to be used for relative coordinates.
	*/
	virtual void render(SDL_Renderer* renderer, const Uint8 alpha,
		const std::shared_ptr<const FD_Camera> camera) const = 0;

	//! Changes the layer of the object.
	/*!
		\param z The new layer value.
	*/
	void setLayer(int z);
	//! Sets whether the objects coordinates are relative to the camera.
	/*!
		\param camera_bound Whether the objects coordinates are relative to the camera.
	*/
	void setCameraBound(bool camera_bound);

	//! Returns the current layer.
	/*!
		\return The current layer.
	*/
	int getLayer() const;
	//! Returns whether the object is drawn relative to the camera.
	/*!
		\return Whether the object is drawn relative to the camera.
	*/
	bool isCameraBound() const;
	//! Returns whether the object has changed its layer since it was last checked.
	/*!
		\return Whether the object has changed its layer since it was last checked.

		\warning This method is not const, using it may affect the function of other classes if used improperly.
	*/
	bool hasChangedLayer();

	//! Registers the object with an object group.
	/*!
		\param group The object group to register the object to.
	*/
	virtual void assimilate(std::shared_ptr<FD_ObjectGroup> group) = 0;

};

//! The FD_ObjectGroup class, groups objects for blanket visibility and opacity variability.
class FD_ObjectGroup : public std::enable_shared_from_this<FD_ObjectGroup> {
private:

	const int id;
	std::vector<std::shared_ptr<FD_Layered>> list{};

	std::weak_ptr<FD_CameraSet> cameras{ std::weak_ptr<FD_CameraSet>() };
	
	FD_Tween* opacity;
	bool visible{ true };

public:

	//! Constructs a FD_ObjectGroup.
	/*!
		\param id The associated object ID (normally given by FD_Scene).
	*/
	FD_ObjectGroup(const int id);
	//! Destroys the FD_ObjectGroup.
	~FD_ObjectGroup();

	//! Updates the group, relayering objects and updating cameras.
	void update();

	//! Prepares the group for a rendering cycle.
	void pre_render();
	//! Renders a specific object.
	/*!
		\param index    The index of the object to draw.
		\param renderer The renderer to use to render the object.
	*/
	void render(const size_t index, SDL_Renderer* renderer) const;
	//! Renders all objects.
	/*!
		\param renderer The renderer to use to render the objects.
	*/
	void render_all(SDL_Renderer* renderer) const;

	//! Adds an object.
	/*!
		\param o The object to add.
	*/
	void addObject(const std::shared_ptr<FD_Layered> o);
	//! Adds a list of objects.
	/*!
		\param os The objects to add.
	*/
	void addObjects(const std::vector<std::shared_ptr<FD_Layered>> os);

	//! Sets the set of cameras that the group will use.
	/*!
		\param set The set of cameras to use.

		\warning Without a camera set, the group will NOT render.
	*/
	void setCameraSet(std::weak_ptr<FD_CameraSet> set);
	//! Returns the current camera set.
	std::weak_ptr<FD_CameraSet> getCameraSet() const;

	//! Returns the ID of the group.
	/*!
		\return The ID of the group.
	*/
	int getID() const;
	//! Returns whether the group is visible or not.
	/*!
		\return Whether the group is visible or not.
	*/
	bool isVisible() const;
	//! Returns the opacity of the group.
	/*!
		\return The opacity of the group.
	*/
	Uint8 getOpacity() const;
	//! Returns the number of objects in the group.
	/*!
		\return The number of objects in the group.
	*/
	size_t getSize() const;
	//! Returns the layer of a specific object by reference.
	/*!
		\param index The index of the object being referred to.
		\param layer The reference to be edited.

		\return Whether a layer relating to the given index could be retrieved.
	*/
	bool getLayer(size_t index, int& layer) const;
	//! Returns the tween for the opacity.
	/*!
		\return The tween for the opacity.
	*/
	FD_Tween* getTweenOpacity();

	//! Sets whether the group is visible or not.
	/*!
		\param visible Whether the group should be visible or not.
	*/
	void setVisible(bool visible);

};

#endif
