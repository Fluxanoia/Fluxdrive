#ifndef FD_PATHS_H_
#define FD_PATHS_H_

#include <string>

#include <SDL_filesystem.h>

/*!
	@file
	@brief The file containing the FD_Paths namespace.
*/

//! The FD_Paths namespace containing function useful for file paths.
namespace FD_Paths {
	//! Replaces all backslashes in a string with forward slashes.
	/*!
		\param string The array of strings to modify.
	*/
	inline void REPLACE_BACKSLASHES(std::string* string) {
		for (int i = 0; (*string)[i] != '\0'; i++) {
			if ((*string)[i] == '\\') (*string)[i] = '/';
		}
	}
	//! Returns the SDL base path attached to the input path.
	/*!
		\param path The path to modify.
	*/
	inline void ADD_BASE_PATH(std::string &path) {
		path = SDL_GetBasePath() + path;
		REPLACE_BACKSLASHES(&path);
	}
	//! Returns the SDL preference path attached to the input path.
	/*!
		\param path The path to modify.
		\param dir  The preference folder name.
	*/
	inline void ADD_PREF_PATH(std::string &path, std::string dir) {
		path = SDL_GetPrefPath("Fluxanoia", dir.c_str()) + path;
		REPLACE_BACKSLASHES(&path);
	}
}

#endif
