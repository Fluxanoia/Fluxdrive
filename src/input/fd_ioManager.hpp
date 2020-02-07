#ifndef FD_IO_MANAGER_H_
#define FD_IO_MANAGER_H_

#include <memory>

#include "fd_audioManager.hpp"
#include "fd_fileManager.hpp"
#include "fd_imageManager.hpp"
#include "fd_inputManager.hpp"

#include <string>

/*!
	@file
	@brief The file containing the FD_IOManager.
*/

//! The FD_IOManager, pulling together all the Fluxdrive resource managers.
class FD_IOManager {
private:

	std::shared_ptr<FD_AudioManager> audio;
	std::shared_ptr<FD_FileManager>  files;
	std::shared_ptr<FD_ImageManager> images;
	std::shared_ptr<FD_InputManager> input;

public:

	//! Constructs the FD_IOManager.
	/*!
		\param renderer The renderer for the manager to use.
	*/
	FD_IOManager(SDL_Renderer* renderer);
	//! Destroys the FD_IOManager.
	~FD_IOManager();
	//! Updates the managers in this class.
	void update();
	//! Pushes events to the managers in this class.
	/*!
		\param e The event to process.
	*/
	void pushEvent(const SDL_Event* e);

	//! Returns the audio manager.
	/*!
		\return The audio manager.
	*/
	std::shared_ptr<FD_AudioManager> getAudioManager();
	//! Returns the file manager.
	/*!
		\return The file manager.
	*/
	std::shared_ptr<FD_FileManager> getFileManager();
	//! Returns the image manager.
	/*!
		\return The image manager.
	*/
	std::shared_ptr<FD_ImageManager> getImageManager();
	//! Returns the input manager.
	/*!
		\return The input manager.
	*/
	std::shared_ptr<FD_InputManager> getInputManager();

};

#endif