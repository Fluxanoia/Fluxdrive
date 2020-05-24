#ifndef FD_WINDOW_H_
#define FD_WINDOW_H_

#include <vector>
#include <memory>
#include <string>

#include <SDL_hints.h>
#include <SDL_events.h>
#include <SDL_render.h>

#include "fd_resizable.hpp"
#include "../main/fd_handling.hpp"

/*!
	@file
	@brief The file containing the FD_Window class.
*/

//! The Fluxdrive wrapper for SDL_Window.
class FD_Window {
private:

	bool updated{ true };
	bool fullscreen{};
	int window_width{}, window_height{};

	SDL_Window* window;
	SDL_Renderer* renderer;

	std::vector<std::shared_ptr<FD_Resizable>> resizeables{};

	void apply();

public:
	
	//! Constructs an FD_Window.
	/*!
		\param title     The title for the window.
		\param width     The width of the window.
		\param height    The height of the window.
		\param win_flags The window flags, see the SDL documentation.
		\param ren_flags The rendering flags for the window, see the SDL documentation.
	*/
	FD_Window(std::string title, int width, int height, 
		Uint32 win_flags = 0,
		Uint32 ren_flags = SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
	//! Destroys the FD_Window.
	~FD_Window();
	
	//! Sets default SDL rendering hints.
	void setDefaultHints();

	// Updates the window.
	void update();

	//! Parses a given SDL_WindowEvent.
	/*!
		This is usually called by an associated FD_Scene.	

		\param e The event to parse.
	*/
	void pushEvent(SDL_WindowEvent e);

	//! Adds an FD_Resizable.
	/*!
		\param resizable The FD_Resizable to add.

		\sa FD_Resizable
	*/
	void addResizable(std::shared_ptr<FD_Resizable> resizable);

	//! Makes the window fullscreen.
	void setFullscreen();
	//! Makes the window windowed.
	void setWindowed();
	//! Changes the dimensions of the window.
	/*!
		\param window_w The new width of the window.
		\param window_h The new height of the window.
	*/
	void setResolution(int window_w, int window_h);
	//! Sets the position of the window on the screen.
	/*!
		\param x The new x coordinate.
		\param y The new y coordinate.
	*/
	void setPosition(int x, int y);

	//! Returns whether the window has changed in some form since it was last checked.
	/*!
		\return Whether the window has changed in some form since it was last checked.

		\warning Calling this function changes when it was last checked so can break some
		of the automatic settings functionality of FD_Scene so take care with use or monitor 
		when you change the window.
	*/
	bool hasUpdated();

	//! Returns the renderer.
	/*!
		\return The renderer.
	*/
	SDL_Renderer* getRenderer() const;
	//! Returns whether the window is fullscreen or not.
	/*!
		\return Whether the window is fullscreen or not.
	*/
	bool isFullscreen() const;
	//! Returns the width of the window when it's not fullscreen.
	/*!
		\return The width of the window when it's not fullscreen.
	*/
	int getWindowedWidth() const;
	//! Returns the height of the window when it's not fullscreen.
	/*!
		\return The height of the window when it's not fullscreen.
	*/
	int getWindowedHeight() const;
	//! Returns the current width of the window.
	/*!
		If the window is fullscreen, this returns the screen width.
		
		\return The width of the window.
	*/
	int getWidth() const;
	//! Returns the current height of the window.
	/*!
		If the window is fullscreen, this returns the screen height.

		\return The height of the window.
	*/
	int getHeight() const;
	//! Returns the width of the screen.
	/*!
		\return The width of the screen.
	*/
	int getScreenWidth() const;
	//! Returns the height of the screen.
	/*!
		\return The height of the screen.
	*/
	int getScreenHeight() const;

};

#endif