#ifndef FD_TIMER_HPP_
#define FD_TIMER_HPP_

#include "SDL_timer.h"
#include "SDL_stdinc.h"

/*!
	@file
	@brief The file containing the class allowing for basic timing.
*/

//! The class for basic timing.
class FD_Timer {
private:

	Uint32 start_time{ 0 };
	Uint32 length{ 0 };

public:

	FD_Timer();
	~FD_Timer();

	//! Start the timer.
	/*!
		\param ms The length of the timer.
	*/
	void start(Uint32 ms);

	//! Returns whether the timer has finished or not.
	/*! 
		\return Whether the timer has finished or not.
	*/
	bool finished() const;

	//! Returns the progress of the timer, in ms.
	/*!
		\return The progress of the timer, in ms.
	*/
	Uint32 progress() const;

};

#endif
