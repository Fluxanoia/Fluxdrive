#ifndef FD_STATE_H_
#define FD_STATE_H_

#include <memory>

#include "../main/fd_handling.hpp"
#include "../display/fd_scene.hpp"

/*!
	@file
	@brief The file containing the FD_State class.
*/

//! The FD_State class, for segmenting programs into distinct states.
class FD_State : public FD_Resizable {
protected:

	//! The unique identifier of the state.
	const int id;
	//! The ID for input used by the state.
	const int input_list;
	//! The ID for object registering used by the state.
	const int object_list;
	//! The ID of the state this state wants to switch to.
	/*!
		\sa getNextState
	*/
	int nextState{ FD_State::INVALID_STATE };
	//! Whether this state is requesting the program's closure.
	/*!
		\sa isClosed
	*/
	bool closed{ false };
	//! The scene used by this state.
	std::weak_ptr<FD_Scene> scene;

	//! Returns a new input list ID.
	int getInputList(std::weak_ptr<FD_Scene> scene);
	//! Returns a new object list ID.
	int getObjectList(std::weak_ptr<FD_Scene> scene);

public:

	//! The value corresponding to the invalid state ID.
	/*!
		\warning Do not create a state with this ID.
	*/
	static const int INVALID_STATE{ -1 };

	//! Constructs a FD_State.
	/*!
		\param id    The unique identifier for the state.
		\param scene The scene for the state to use.
	*/
	FD_State(int id, std::weak_ptr<FD_Scene> scene);
	//! Destroys the FD_State.
	virtual ~FD_State();
	//! Allows the state to prepare to resume running after being slept.
	/*!
		\sa sleep
	*/
	virtual void wake();
	//! Allows the state to prepare to pause running.
	/*!
		\sa wake
	*/
	virtual void sleep() = 0;
	//! Updates the state.
	virtual void update() = 0;
	//! Allows the state to resize its elements based on the new dimensions of the window.
	virtual void resized();

	//! Returns the state this state wants to switch to by reference.
	/*!
		\param id The reference to write the next state to.

		\return Whether the state wants to switch to a new state.
	*/
	bool getNextState(int& id);
	//! Returns whether the state is requesting the closure of the program. 
	/*!
		\return Whether the state is requesting the closure of the program.
	*/
	bool hasClosed();

	//! Returns the ID of the state.
	/*!
		\return The ID of the state.
	*/
	int getID() const;

};

#endif