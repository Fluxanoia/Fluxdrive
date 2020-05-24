#ifndef FD_HANDLING_H_
#define FD_HANDLING_H_

#include <memory>
#include <stdlib.h>
#include <iostream>
#include <exception>

#include "SDL_ttf.h"
#include "SDL_image.h"

/*! 
	@file
	@brief A file containing resources relating to error handling and debugging.
*/

//! This is a general exception called by the functions in the FD_Handling namespace.
/*! Its use should be avoided as to not confuse Fluxdrive errors with other errors. */

class FD_HandlingException : public std::exception {

	//! The what() method overriden from std::exception.
	/*! 
		\return A generic exception message.
	*/
	virtual const char* what() const throw() {
		return "A handling exception occured.";
	}
};

//! The namespace containing debugging and error handling helper functions.
/*!
	This is used by virtually all Fluxdrive classes and its use is strongly encouraged.
*/
namespace FD_Handling {

	//! Throws a FD_HandlingException and terminante execution with respect to the parameters.
	/*!
		When both parameters are false, this function does nothing.
		The function will always attempt to throw an exception
		before it attempts to terminate execution with respect
		to the parameters.
	
		\param to_throw Whether the function should throw a FD_HandlingException.
		\param to_exit  Whether the function should terminate execution.
		
		\throw FD_HandlingException Thrown when to_throw is true.
	*/
	void throw_and_exit(bool to_throw, bool to_exit);
	
	//! Prints an error message and; can throw FD_HandlingException and terminate execution.
	/*!
		This prints an error message to std::cerr and if fatal is true,
		FD_HandlingException is thrown and the execution is terminated.

		\param s     The error message to be printed.
		\param fatal Whether the function should throw FD_HandlingException and terminate execution.

		\throw FD_HandlingException Thrown when fatal is true.

		\sa throw_and_exit
		\sa errorSDL
		\sa errorIMG
		\sa errorTTF
	*/
	void error(const std::string s = "An error occured.", bool fatal = false);

	//! Prints an error message and; the current SDL error and; can throw FD_HandlingException and terminate execution.
	/*!
		This prints an error message and then the current SDL error to std::cerr and if fatal is true,
		FD_HandlingException is thrown and the execution is terminated.

		\param s     The error message to be printed.
		\param fatal Whether the function should throw FD_HandlingException and terminate execution.

		\throw FD_HandlingException Thrown when fatal is true.

		\sa throw_and_exit
		\sa error
		\sa errorIMG
		\sa errorTTF
	*/
	void errorSDL(const std::string s = "An SDL error occured.", bool fatal = false);

	//! Prints an error message and; the current SDL_Image error and; can throw FD_HandlingException and terminate execution.
	/*!
		This prints an error message and then the current SDL_Image error to std::cerr and if fatal is true,
		FD_HandlingException is thrown and the execution is terminated.

		\param s     The error message to be printed.
		\param fatal Whether the function should throw FD_HandlingException and terminate execution.

		\throw FD_HandlingException Thrown when fatal is true.

		\sa throw_and_exit
		\sa error
		\sa errorSDL
		\sa errorTTF
	*/
	void errorIMG(const std::string s = "An SDL_image error occured.", bool fatal = false);

	//! Prints an error message and; the current SDL_TTF error and; can throw FD_HandlingException and terminate execution.
	/*!
		This prints an error message and then the current SDL_TTF error to std::cerr and if fatal is true,
		FD_HandlingException is thrown and the execution is terminated.

		\param s     The error message to be printed.
		\param fatal Whether the function should throw FD_HandlingException and terminate execution.

		\throw FD_HandlingException Thrown when fatal is true.

		\sa throw_and_exit
		\sa error
		\sa errorSDL
		\sa errorIMG
	*/
	void errorTTF(const std::string s = "An SDL_ttf error occured.", bool fatal = false);
	
	//! Locks a weak pointer into a given shared pointer
	/*!
		This locks a weak pointer into a given shared pointer. If this fails
		then false is returned and; an error message is printed, an FD_HandlingException
		is thrown and, execution is halted with respect to the parameters.
		To be specific, error is called with a custom message and fatal as its
		parameters.

		\param w_ptr     The weak pointer to be locked.
		\param s_ptr     The shared pointer for the weak pointer to be locked into.
		\param print_err Whether an error message should be printed.
		\param fatal     Whether the program should throw a FD_HandlingException and terminate execution.

		\throw FD_HandlingException Thrown when fatal is true.
		
		\sa error
	*/
	template <class T>
	inline bool lock(std::weak_ptr<T> w_ptr, std::shared_ptr<T>& s_ptr,
		bool print_err = true, bool fatal = true) {
		if (auto temp_s_ptr{ w_ptr.lock() }) {
			s_ptr = temp_s_ptr;
			return true;
		}
		if (print_err) {
			FD_Handling::error("The pointer could not be locked.", fatal);
		}
		return false;
	}
	
	
	//! Prints a debug message if FD_DEBUG is defined
	/*
		Prints a debug message to std::cout if FD_DEBUG is defined using #define.

		\param s A debug message.
	*/
	void debug(const std::string s = "");

	//! Prints a large debug message if FD_DEBUG is defined
	/*
		Prints a large debug message to std::cout if FD_DEBUG is defined using #define.
		This is for situations where a debug message could easily get lost in the output.
	*/
	void debug_alert();
}

#endif