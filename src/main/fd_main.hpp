#ifndef FD_MAIN_
#define FD_MAIN_

/*!
	@file
	@brief A file containing the namespace FD_Main, relating to initialisation and closing of Fluxdrive.
*/

//! The namespace relating to initialisation and closing of Fluxdrive.
namespace FD_Main {
	//! Initialises SDL and Fluxdrive.
	/*!
		This should be called before any Fluxdrive related classes are constructed.
		If called, close should be called before execution ends.
		
		\sa close
	*/
	bool initialise();
	//! Runs the Fluxdrive test suite.
	void test();
	//! Closes Fluxdrive and SDL. 
	/*!
		If initialise is called, close should also be called before the execution ends.
	*/
	void close();
}

#endif