#ifndef FD_LOOPER_
#define FD_LOOPER_

#include <chrono>
#include <memory>
#include <string>

#include "SDL_stdinc.h"
#include "SDL_events.h"

#include "fd_handling.hpp"

/*!
	@file
	@brief A file containing classes allowing for updating and rendering loops to be made.
*/


//! The class that can be controller by FD_Looper.
/*!
	This class should be inherited by other classes to allow them to be used within the FD_Looper class.
	FD_Looper calls update at a fixed, given rate, render as much as possible and, pushEvent as events
	occur.
	This class is of little use otherwise.
*/
class FD_Loopable {
protected:

	//! Whether the FD_Loopable should terminate its loop.
	bool closed{ false };

public:

	//! Constructs a FD_Loopable.
	FD_Loopable() = default;
	//! Destroys the FD_Loopable.
	~FD_Loopable() = default;

	//! The update method to be ran by FD_Looper.
	virtual void update();
	//! The render method to be ran by FD_Looper.
	virtual void render();
	//! The event handling method to be ran by FD_Looper.
	virtual void pushEvent(const SDL_Event* e);
	//! The method allowing the FD_Loopable to be closed by force.
	virtual void forceClose();
	//! The getter function for closed.
	bool hasClosed();

};

//! The class that runs the methods of FD_Loopable with respect to the rate(s) given.
/*!
	This class takes a FD_Loopable and a fixed update rate in hertz (per second).
*/
class FD_Looper {
private:

	//! The rate at which FD_Looper updates the given FD_Loopable in hertz (per second).
	Uint32 ups;
	//! The FD_Loopable being controller by FD_Looper.
	std::shared_ptr<FD_Loopable> loopable;

public:

	//! Constructs a FD_Looper
	/*!
		Constructs an FD_Looper.

		\param loopable The FD_Loopable to be controller by FD_Looper.
		\param ups      The rate at which the FD_Looper will update loopable in hertz (per second).

		\throw FD_HandlingException Thrown when loopable is nullptr.
	*/
	FD_Looper(std::shared_ptr<FD_Loopable> loopable, Uint32 ups = 60);
	//! Destroys the FD_Looper.
	~FD_Looper();

	//! Initialises the loop, updating, rendering and, pushing events to the given FD_Loopable.
	/*!
		This loop can be terminated by the FD_Loopable closing itself.

		\sa FD_Loopable
	*/
	void loop();

};

#endif
