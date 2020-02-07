#ifndef FD_EVENT_LISTENER_H_
#define FD_EVENT_LISTENER_H_

#include <SDL_events.h>

/*!
	@file
	@brief The file containing the FD_EventListener.
*/

//! The FD_EventListener, allows any class to listen to SDL_Events.
class FD_EventListener {
public:

	//! Constructs a FD_EventListener.
	FD_EventListener();
	//! Destroys the FD_EventListener.
	~FD_EventListener();

	//! Provides the listener with an event to process.
	/*!
		\param event The event to process.
	*/
	virtual void pushEvent(const SDL_Event* event) = 0;

};

#endif
