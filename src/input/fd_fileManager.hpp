#ifndef FD_FILE_MANAGER_H_
#define FD_FILE_MANAGER_H_

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>
#include <functional>

#include "fd_paths.hpp"
#include "fd_registry.hpp"

/*!
	@file
	@brief The file containing the classes relating to file management.
*/

//! The data type of the file register value.
typedef int FD_FileRegister;

//! The FD_File class, managing a specific files data.
class FD_File {
private:

	const char delim{ ':' };

	std::string path{};
	bool existing { false };
	std::vector<std::string> data = {};

	void load();

public:

	//! Constructs a FD_File.
	/*!
		\param path   The path to the file.
		\param ending The ending to append to the file path.
	*/
	FD_File(const std::string path, const std::string ending = "");
	//! Destroys the FD_File.
	~FD_File();

	//! Creates the file this class links to if it doesn't exist already.
	/*!
		\return Whether the file was created or not.
	*/
	bool create();
	//! Appends lines to the end of the file.
	/*!
		\param lines The lines to append.

		\return Whether the lines were successfully appended.
	*/
	bool append(const std::vector<std::string> &lines);
	//! Overwrites the file with the given lines.
	/*!
		\param lines The new lines for the file.

		\return Whether the file was successfully overwritten.
	*/
	bool overwrite(const std::vector<std::string> &lines);

	//! Returns the value from a key-value pair in a file.
	/*!
		Key-value pairs are written as "key:value" in files
		and have distinct lines.

		\param key   The key to look for.
		\param value The string to write the value to.

		\return Whether a value was retrieved.
	*/
	bool getValue(const std::string key, std::string &value) const;
	//! Identical to getValue but casts the result to an int.
	/*!
		\param key   The key to look for.
		\param value The variable to write the value to.

		\return Whether a value was retrieved.

		\sa getValue
	*/
	bool getIntegerValue(const std::string key, int &value) const;
	//! Identical to getValue but casts the result to a double.
	/*!
		\param key   The key to look for.
		\param value The variable to write the value to.

		\return Whether a value was retrieved.

		\sa getValue
	*/
	bool getDoubleValue(const std::string key, double &value) const;
	//! Identical to getValue but casts the result to a boolean.
	/*!
		Values greater than zero are mapped to true, the rest to false.

		\param key   The key to look for.
		\param value The variable to write the value to.

		\return Whether a value was retrieved.

		\sa getValue
	*/
	bool getBoolValue(const std::string key, bool &value) const;
	
	//! Returns the value from a key-value pair in a file. If none is found, the default is written to the file and returned.
	/*!
		\param key   The key to look for.
		\param value The string to write the value to.
		\param def   The default to return.

		\return Whether a value was retrieved.

		\sa getValue
	*/
	bool getValue(const std::string key, std::string& value, const std::string &def);
	//! Identical to getValue (with the default) but casts the result to an int.
	/*!
		\param key   The key to look for.
		\param value The variable to write the value to.
		\param def   The default to return.

		\return Whether a value was retrieved.

		\sa getValue
	*/
	bool getIntegerValue(const std::string key, int& value, const std::string &def);
	//! Identical to getValue (with the default) but casts the result to a double.
	/*!
		\param key   The key to look for.
		\param value The variable to write the value to.
		\param def   The default to return.

		\return Whether a value was retrieved.

		\sa getValue
	*/
	bool getDoubleValue(const std::string key, double& value, const std::string &def);
	//! Identical to getValue (with the default) but casts the result to a boolean.
	/*!
		\param key   The key to look for.
		\param value The variable to write the value to.
		\param def   The default to return.

		\return Whether a value was retrieved.

		\sa getValue
	*/
	bool getBoolValue(const std::string key, bool& value, const std::string &def);

	//! Overwrites a value in a key-value pair. Does not create new pairs.
	/*!
		\param key   The key to look for.
		\param value The value to write.

		\return Whether a value was overwritten.
	*/
	bool setValue(const std::string &key, const std::string &value);

	//! Returns whether the file path is empty.
	/*!
		\return Whether the file path is empty.
	*/
	bool isPathEmpty() const;
	//! Returns whether the path of the file is identical to the parameters.
	/*!
		\param path   The path to check against.
		\param ending The file ending to check against.

		\return Whether the path given is identical to this file's path.
	*/
	bool hasPath(const std::string path, const std::string ending = "") const;

	//! Returns whether the file exists or not.
	/*!
		\return Whether the file exists or not.
	*/
	bool exists() const;
	//! Returns the file data.
	/*!
		\return The file data.
	*/
	std::vector<std::string> getData() const;

};

//! The FD_FileManager class, manages instances of FD_File.
class FD_FileManager : public FD_Registered {
private:

	std::vector<std::shared_ptr<FD_File>> files {  };

public:

	//! Constructs a FD_FileManager.
	FD_FileManager();
	//! Destroys the FD_FileManager.
	~FD_FileManager();

	//! Loads a file into the manager and returns it.
	/*!
		\param reg    The register of the path to add.
		\param ending The file ending to add.

		\return The file, which may or may not exist.
	*/
	std::shared_ptr<FD_File> loadFile(const FD_FileRegister reg, const std::string ending = "");
	//! Loads a file into the manager and returns it.
	/*!
		\param path   The path of the file to add.
		\param ending The file ending to add.

		\return The file, which may or may not exist.
	*/
	std::shared_ptr<FD_File> loadFile(const std::string path, const std::string ending = "");

	//! Deletes a file from the manager.
	/*!
		\param reg    The register of the path to delete.
		\param ending The file ending of the file to delete.

		\return Whether a file was deleted or not.
	*/
	bool deleteFile(const FD_FileRegister reg, const std::string ending = "");
	//! Deletes a file from the manager.
	/*!
		\param path   The path of the file to delete.
		\param ending The file ending of the file to delete.

		\return Whether a file was deleted or not.
	*/
	bool deleteFile(const std::string path, const std::string ending = "");

};

#endif