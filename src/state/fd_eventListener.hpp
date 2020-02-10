#ifndef FD_EVENT_LISTENER_H_
#define FD_EVENT_LISTENER_H_

#include <vector>

#include <SDL_events.h>

/*!
	@file
	@brief The file containing the FD_EventListener.
*/

//! The FD_EventListener, allows any class to listen to all SDL_Events.
class FD_EventListener {
private:

	bool accepting{ true };
	std::vector<SDL_Event> queue{};

public:

	//! Constructs a FD_EventListener.
	FD_EventListener();
	//! Destroys the FD_EventListener.
	~FD_EventListener();

	//! Provides the listener with an event to process.
	/*!
		\param event The event to process.
	*/
	virtual void pushEvent(const SDL_Event* event);

	//! Takes an event from the listener by reference.
	/*!
		\param e The event to be written to.
		
		\return Whether an event has been written to the input.
	*/
	virtual bool pullEvent(SDL_Event& e);

	//! Sets whether the listener is currently accepting events.
	/*!
		\param a Whether the listener should accept events.
	*/
	virtual void setAccepting(bool a);

	//! Clears the event queue.
	virtual void clear();

};

#endif
