#include "fd_scene.hpp"

FD_Scene::FD_Scene(std::shared_ptr<FD_Window> window,
	std::string settings_path) {
	this->use_settings = true;
	this->win = window;
	this->settings_path = settings_path;
	io = std::make_shared<FD_IOManager>(window->getRenderer());
	readDisplaySettings();
}

FD_Scene::FD_Scene(std::shared_ptr<FD_Window> window,
	bool fullscreen, int window_width, int window_height) {
	this->use_settings = false;
	this->win = window;
	io = std::make_shared<FD_IOManager>(window->getRenderer());
	if (fullscreen) {
		win->setFullscreen();
	} else {
		win->setWindowed();
		win->setResolution(window_width, window_height);
	}
}

FD_Scene::~FD_Scene() {
	groups.clear();
	FD_Handling::debug("FD_Scene destroyed.");
}

void FD_Scene::readDisplaySettings() {
	if (!use_settings) return;
	// Create the settings file if needed
	std::string temp_path{ settings_path };
	FD_Paths::ADD_BASE_PATH(temp_path);
	settings = io->getFileManager()->loadFile(temp_path);
	if (!settings->exists()) {
		if (!settings->create()) {
			FD_Handling::error("Settings file could not be created.", true);
		}
	}
	// Get the settings values
	std::string def_res_width = getDefault(FD_DEF_RESOLUTION_WIDTH);
	std::string def_res_height = getDefault(FD_DEF_RESOLUTION_HEIGHT);
	std::string def_fullscreen = getDefault(FD_DEF_FULLSCREEN);
	bool fullscreen;
	int window_width, window_height;
	if (!settings->getIntegerValue(
		getKey(FD_KEY_RESOLUTION_WIDTH), window_width, def_res_width)) {
		FD_Handling::error("Settings file could not be read.", true);
	}
	if (!settings->getIntegerValue(
		getKey(FD_KEY_RESOLUTION_HEIGHT), window_height, def_res_height)) {
		FD_Handling::error("Settings file could not be read.", true);
	}
	if (!settings->getBoolValue(
		getKey(FD_KEY_FULLSCREEN), fullscreen, def_fullscreen)) {
		FD_Handling::error("Settings file could not be read.", true);
	}
	win->setResolution(window_width, window_height);
	if (fullscreen) {
		win->setFullscreen();
	} else {
		win->setWindowed();
	}
}
void FD_Scene::writeDisplaySettings() {
	if (!use_settings) return;
	if (!settings->setValue(
		getKey(FD_KEY_FULLSCREEN), std::to_string(win->isFullscreen()))) {
		FD_Handling::error("Settings file could not be read.", true);
		return;
	}
	if (!settings->setValue(
		getKey(FD_KEY_RESOLUTION_WIDTH), 
		std::to_string(win->getWindowedWidth()))) {
		FD_Handling::error("Settings file could not be read.", true);
		return;
	}
	if (!settings->setValue(
		getKey(FD_KEY_RESOLUTION_HEIGHT), 
		std::to_string(win->getWindowedHeight()))) {
		FD_Handling::error("Settings file could not be read.", true);
		return;
	}
}

void FD_Scene::update() {
	if (win->hasUpdated()) writeDisplaySettings();
	if (groups.find(currentID) != groups.end()) {
		for (auto og : groups.at(currentID)) {
			og->update();
		}
	}
	io->update();
}

void FD_Scene::prepareRenderProgress() {
	render_progress.clear();
	render_completion.clear();
	render_minimals.clear();
	if (groups.find(currentID) != groups.end()) {
		for (auto og : groups.at(currentID)) {
			render_progress.push_back(0);
			render_completion.push_back(og->getSize() == 0);
		}
	}
}
bool FD_Scene::renderCompleted() {
	for (bool b : render_completion) {
		if (!b) return false;
	}
	return true;
}
void FD_Scene::render() {
	SDL_RenderClear(win->getRenderer());
	// Render the current object list
	if (groups.find(currentID) != groups.end()) {
		// Prepare the camera
		for (auto og : groups.at(currentID)) {
			og->pre_render();
		}
		// Render the objects
		prepareRenderProgress();
		bool first_inspection{};
		size_t index{};
		int temp_minimal{}, minimal{};
		while (!renderCompleted()) {
			// index        : the index of the group we are looking at
			// temp_minimal : the lowest layer of the current group
			// Reset the index
			index = 0;
			first_inspection = true;
			// Clear the 'groups with the minimal layer'
			render_minimals.clear();
			// Iterate through the groups...
			for (auto og : groups.at(currentID)) {
				// If the group is complete, ignore
				if (render_completion.at(index)) {
					index++;
					continue;
				}
				// Get the minimal layer not get drawn
				og->getLayer(render_progress.at(index), temp_minimal);
				// Check if it's the smallest seen
				if (first_inspection) {
					// If it's the first group, it's always the smallest seen
					minimal = temp_minimal;
					render_minimals.push_back(index);
					first_inspection = false;
				} else {
					if (temp_minimal < minimal) {
						// This is a new low, reset the minimals
						render_minimals.clear();
						// Set the minimal and add this group to the minimals
						minimal = temp_minimal;
						render_minimals.push_back(index);
					} else if (temp_minimal == minimal) {
						// This is a shared low, add to the minimals
						render_minimals.push_back(index);
					}
				}
				index++;
			}
			// min_index    : the index of the group with the minimal
			// index        : the index of the object we are looking to draw
			// temp_minimal : the layer of the next object in this groups list
			for (size_t min_index : render_minimals) {
				// Get the object index
				index = render_progress.at(min_index);
				// The layer of the next object to be drawn is by
				// definition, the minimal
				temp_minimal = minimal;
				// If we haven't completed this list and the next object is also minimal...
				while (!render_completion.at(min_index) && temp_minimal == minimal) {
					// Render the object
					groups.at(currentID).at(min_index)->render(index, win->getRenderer());
					// Increment our progress and our index
					index++;
					render_progress.at(min_index) = index;
					// If our progress has reached the end of the list, we are complete
					if (render_progress.at(min_index) ==
						groups.at(currentID).at(min_index)->getSize()) {
						render_completion.at(min_index) = true;
					} else {
						// Get the minimal of the next element for checking
						groups.at(currentID).at(min_index)->getLayer(index, temp_minimal);
					}
				}
			}
		}
	}
	SDL_RenderPresent(win->getRenderer());
}

void FD_Scene::pushEvent(const SDL_Event* e) {
	if (e->type == SDL_WINDOWEVENT) win->pushEvent(e->window);
	io->pushEvent(e);
}

FD_ObjListIndex FD_Scene::generateObjectList() {
	groups[++objectIDCount]
		= std::vector<std::shared_ptr<FD_ObjectGroup>>();
	return objectIDCount;
}
void FD_Scene::setObjectList(const FD_ObjListIndex id) {
	currentID = id;
}

void FD_Scene::addObjectGroup(const std::shared_ptr<FD_ObjectGroup> og) {
	if (groups.find(og->getID()) != groups.end()) {
		groups.at(og->getID()).push_back(og);
	}
}
void FD_Scene::removeObjectGroup(const std::shared_ptr<FD_ObjectGroup> og) {
	if (groups.find(og->getID()) != groups.end()) {
		auto v{ groups.at(og->getID()) };
		if (std::find(v.begin(), v.end(), og) != v.end()) {
			v.erase(std::find(v.begin(), v.end(), og));
		}
	}
}

std::shared_ptr<FD_IOManager> FD_Scene::getIOManager() const {
	return io;
}
std::shared_ptr<FD_AudioManager> FD_Scene::getAudioManager() const {
	return io->getAudioManager();
}
std::shared_ptr<FD_FileManager> FD_Scene::getFileManager() const {
	return io->getFileManager();
}
std::shared_ptr<FD_ImageManager> FD_Scene::getImageManager() const {
	return io->getImageManager();
}
std::shared_ptr<FD_InputManager> FD_Scene::getInputManager() const {
	return io->getInputManager();
}
std::shared_ptr<FD_Window> FD_Scene::getWindow() const {
	return win;
}

std::string FD_Scene::getKey(DisplayKeys key) {
	switch (key) {
	case FD_KEY_RESOLUTION_WIDTH: return "resolution_width";
	case FD_KEY_RESOLUTION_HEIGHT: return "resolution_height";
	case FD_KEY_FULLSCREEN: return "fullscreen";
	}
	FD_Handling::error("Unknown display key.", true);
	return "Unknown display key.";
}
std::string FD_Scene::getDefault(DisplayDefaults def) {
	switch (def) {
	case FD_DEF_RESOLUTION_WIDTH: return std::to_string(1280);
	case FD_DEF_RESOLUTION_HEIGHT: return std::to_string(720);
	case FD_DEF_FULLSCREEN: return std::to_string(false);
	}
	FD_Handling::error("Unknown display key.", true);
	return "Unknown display key.";
}