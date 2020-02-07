#ifndef FD_SERIALISATION_H_
#define FD_SERIALISATION_H_

#include <string>
#include <sstream>

#include <SDL_rect.h>

/*!
	@file
	@brief The file containing the FD_Serialisation namespace.
*/

//! The FD_Serialisation namespace pertaining to writing to files and parsing files.
namespace FD_Serialisation {
	
	//! Returns the content of the string up to the next delimeter and removes it.
	/*!
		\param info The reference to the string.

		\return The content of the string up to the next delimeter.
	*/
	std::string trim(std::string& info);
	//! Returns and casts to bool the content of the string up to the next delimeter and removes it.
	/*!
		\param info The reference to the string.

		\return The content of the string up to the next delimeter casted to bool.
	*/
	bool trimBool(std::string& info);
	//! Returns and casts to double the content of the string up to the next delimeter and removes it.
	/*!
		\param info The reference to the string.

		\return The content of the string up to the next delimeter casted to double.
	*/
	double trimDouble(std::string& info);
	//! Returns and casts to int the content of the string up to the next delimeter and removes it.
	/*!
		\param info The reference to the string.

		\return The content of the string up to the next delimeter casted to int.
	*/
	int trimInteger(std::string& info);
	//! Returns and casts to SDL_Rect the content of the string up to the next delimeter and removes it.
	/*!
		\param info The reference to the string.
		\param rect The reference of the rectangle to write to.
	*/
	void trimRect(std::string& info, SDL_Rect& rect);
	//! Returns and casts to SDL_Point the content of the string up to the next delimeter and removes it.
	/*!
		\param info The reference to the string.
		\param p The reference of the point to write to.
	*/
	void trimPoint(std::string& info, SDL_Point& p);

	//! Identical to trim but does not take the reference of the string.
	/*!
		As a reference is not taken, this does not mutate the original string.

		\param info The string to inspect.

		\sa trim
	*/
	std::string inspect(std::string info);
	//! Identical to trimBool but does not take the reference of the string.
	/*!
		As a reference is not taken, this does not mutate the original string.

		\param info The string to inspect.

		\sa trimBool
	*/
	bool inspectBool(std::string info);
	//! Identical to trimDouble but does not take the reference of the string.
	/*!
		As a reference is not taken, this does not mutate the original string.

		\param info The string to inspect.

		\sa trimDouble
	*/
	double inspectDouble(std::string info);
	//! Identical to trimInteger but does not take the reference of the string.
	/*!
		As a reference is not taken, this does not mutate the original string.

		\param info The string to inspect.

		\sa trimInteger
	*/
	int inspectInteger(std::string info);
	//! Identical to trimRect but does not take the reference of the string.
	/*!
		As a reference is not taken, this does not mutate the original string.

		\param info The string to inspect.
		\param rect The rectangle to write to.

		\sa trimRect
	*/
	void inspectRect(std::string info, SDL_Rect& rect);
	//! Identical to trimPoint but does not take the reference of the string.
	/*!
		As a reference is not taken, this does not mutate the original string.

		\param info The string to inspect.
		\param p    The point to write to.

		\sa trimPoint
	*/
	void inspectPoint(std::string info, SDL_Point& p);

	//! Appends a string onto an existing string using the FD_Serialisation delim.
	/*!
		\param info   The string to edit.
		\param append The information to append.
	*/
	void append(std::string& info, const std::string append);
	//! Appends a boolean onto an existing string using the FD_Serialisation delim.
	/*!
		\param info   The string to edit.
		\param append The information to append.
	*/
	void append(std::string& info, const bool append);
	//! Appends a double onto an existing string using the FD_Serialisation delim.
	/*!
		\param info   The string to edit.
		\param append The information to append.
	*/
	void append(std::string& info, const double append);
	//! Appends an int onto an existing string using the FD_Serialisation delim.
	/*!
		\param info   The string to edit.
		\param append The information to append.
	*/
	void append(std::string& info, const int append);
	//! Appends an SDL_Rect onto an existing string using the FD_Serialisation delim.
	/*!
		\param info   The string to edit.
		\param append The information to append.
	*/
	void append(std::string& info, const SDL_Rect append);

	//! Returns the delimiter used by the FD_Serialisation namespace. 
	/*!
		\return The delimiter used by the FD_Serialisation namespace. 
	*/
	char delim();

};

#endif
