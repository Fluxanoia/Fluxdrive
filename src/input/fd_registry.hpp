#ifndef FD_REGISTRY_H_
#define FD_REGISTRY_H_

#include <memory>
#include <string>
#include <unordered_map>

/*!
	@file
	@brief The file containing the classes that allow indexing of strings (paths).
*/

//! The FD_Registry class, allows indexing of strings (paths).
class FD_Registry {
private:

	std::unordered_map<int, std::string> maps{};

public:

	//! Constructs a FD_Registry.
	FD_Registry();
	//! Destroys the FD_Registry.
	~FD_Registry();

	//! Adds the value with the corresponding ID to the list.
	/*!
		\param id    The identifier for the information.
		\param value The information.
	*/
	void log(const int id, const std::string value);
	//! Returns the value corresponding to the given ID.
	/*!
		\param id    The identifier of the string wanted.
		\param value The reference to write the value to.

		\return Whether there was anything at the given ID.
	*/
	bool get(const int id, std::string& value) const;

};

//! The FD_Registered class, allows a FD_Registry to be associated with a class.
class FD_Registered {
protected:

	//! The registry of the class.
	std::weak_ptr<FD_Registry> registry{ std::weak_ptr<FD_Registry>() };

public:

	//! Constructs a FD_Registered.
	FD_Registered() = default;
	//! Destroys the FD_Registered.
	~FD_Registered() = default;

	//! Sets the registry of the class.
	/*!
		\param registry The registry to set to.
	*/
	void setRegistry(std::weak_ptr<FD_Registry> registry);

};

#endif
