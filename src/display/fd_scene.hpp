#ifndef FD_SCENE_H_
#define FD_SCENE_H_

#include <memory>
#include <vector>
#include <unordered_map>

#include <SDL_video.h>

#include "fd_camera.hpp"
#include "fd_window.hpp"
#include "../display/fd_resizable.hpp"
#include "../input/fd_ioManager.hpp"
#include "../input/fd_paths.hpp"
#include "../object/fd_objectGroup.hpp"
#include "../main/fd_handling.hpp"

/*!
	@file
	@brief The file containing the FD_Scene class.
*/

//! Defines a type for indices of object lists.
typedef int FD_ObjListIndex;

//! The class that manages rendering, FD_Window settings, FD_Object instances, and the FD_IOManager.
class FD_Scene {
private:

	enum DisplayKeys {
		FD_KEY_RESOLUTION_WIDTH,
		FD_KEY_RESOLUTION_HEIGHT,
		FD_KEY_FULLSCREEN,

		FD_KEY_COUNT
	};
	enum DisplayDefaults {
		FD_DEF_RESOLUTION_WIDTH,
		FD_DEF_RESOLUTION_HEIGHT,
		FD_DEF_FULLSCREEN,

		FD_DEFAULT_COUNT
	};
	std::string getKey(DisplayKeys key);
	std::string getDefault(DisplayDefaults def);
	bool use_settings;

	std::shared_ptr<FD_IOManager> io;
	std::shared_ptr<FD_Window> win;
	
	FD_ObjListIndex objectIDCount{ 0 }, currentID{ 0 };
	std::unordered_map<FD_ObjListIndex,
		std::vector<std::shared_ptr<FD_ObjectGroup>>> groups{};

	std::string settings_path;
	std::shared_ptr<FD_File> settings;
	void readDisplaySettings();
	void writeDisplaySettings();

	std::vector<size_t> render_progress{};
	std::vector<size_t> render_minimals{};
	std::vector<bool> render_completion{};
	void prepareRenderProgress();
	bool renderCompleted();

public:

	//! Constructs an FD_Scene.
	/*!
		Use this constructor if you want the FD_Scene to automatically
		use/read/write its settings to the settings_path.

		\param window        The FD_Window to associate the FD_Scene with.
		\param settings_path The path the FD_Scene should use to read/write its settings.
	*/
	FD_Scene(std::shared_ptr<FD_Window> window,
		std::string settings_path = "config/display.fdc");
	//! Constructs an FD_Scene.
	/*!
		Use this constructor if you manually change the settings of the
		FD_Scene.

		\param window        The FD_Window to associate the FD_Scene with.
		\param fullscreen    Whether the window should be fullscreen or not.
		\param window_width  The desired width of the window.
		\param window_height The desired height of the window.
	*/
	FD_Scene(std::shared_ptr<FD_Window> window, bool fullscreen, 
		int window_width, int window_height);
	//! Destorys the FD_Scene.
	~FD_Scene();

	//! Updates the FD_Scene.
	/*!
		This should be run every update cycle.
	*/
	void update();
	//! Renders the instances of FD_Object to the FD_Window.
	void render();
	//! Pushes events to the classes associated to the FD_Scene.
	/*!
		\param e The event to be parsed.
	*/
	void pushEvent(const SDL_Event* e);

	//! Generates a new index for registering instances of FD_ObjectGroup.
	/*!
		The FD_Scene can only draw a single object list at a time, meaning
		instances of FD_State (or anything really) shpuld have their own (or many)
		object lists that they will register their instances of FD_ObjectGroup to for drawing.

		\return The index for the new object list.
	*/
	FD_ObjListIndex generateObjectList();
	//! Changes the current object list being drawn.
	/*!
		\param id The index of the object list.

		\sa generateObjectList
	*/
	void setObjectList(const FD_ObjListIndex id);

	//! Adds an FD_ObjectGroup to its associated object list.
	/*!
		Note that each FD_ObjectGroup contains its associated object list index.

		\param og The FD_ObjectGroup to be added.
	*/
	void addObjectGroup(const std::shared_ptr<FD_ObjectGroup> og);
	//! Removes an FD_ObjectGroup to its associated object list.
	/*!
		Note that each FD_ObjectGroup contains its associated object list index.
		
		\param og The FD_ObjectGroup to be removed.
	*/
	void removeObjectGroup(const std::shared_ptr<FD_ObjectGroup> og);

	//! Returns the FD_IOManager.
	/*!
		\return The FD_IOManager.
	*/
	std::shared_ptr<FD_IOManager> getIOManager() const;
	//! Returns the FD_InputManager in FD_IOManager.
	/*!
		\return The FD_InputManager.
	*/
	std::shared_ptr<FD_InputManager> getInputManager() const;
	//! Returns the FD_AudioManager in FD_IOManager.
	/*!
		\return The FD_AudioManager.
	*/
	std::shared_ptr<FD_AudioManager> getAudioManager() const;
	//! Returns the FD_ImageManager in FD_IOManager.
	/*!
		\return The FD_ImageManager.
	*/
	std::shared_ptr<FD_ImageManager> getImageManager() const;
	//! Returns the FD_FileManager in FD_IOManager.
	/*!
		\return The FD_FileManager.
	*/
	std::shared_ptr<FD_FileManager> getFileManager() const;
	//! Returns the FD_Window.
	/*!
		\return The FD_Window.
	*/
	std::shared_ptr<FD_Window> getWindow() const;

};

#endif