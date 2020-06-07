#ifndef FD_STATE_MANAGER_H_
#define FD_STATE_MANAGER_H_

#include <memory>

#include "fd_state.hpp"
#include "fd_eventListener.hpp"
#include "../main/fd_looper.hpp"
#include "../object/fd_element.hpp"
#include "../display/fd_scene.hpp"

/*!
	@file
	@brief The file containing the FD_StateManager.
*/

//! The FD_StateManager class, manages instances of the FD_State class.
class FD_StateManager : public FD_Loopable {
private:

	std::weak_ptr<FD_Scene> scene;

	int currentState{ FD_State::INVALID_STATE };
	std::vector<std::weak_ptr<FD_State>> states{};
	std::vector<std::weak_ptr<FD_EventListener>> event_list{};

public:

	//! Constructs a FD_StateManager.
	/*!
		\param scene The scene for the manager to use.
	*/
	FD_StateManager(std::weak_ptr<FD_Scene> scene);
	//! Destroys the FD_StateManager.
	~FD_StateManager();

	//! Adds a state to the manager.
	void logState(std::weak_ptr<FD_State> state);
	//! Adds an event listener to the manager.
	void logEventListener(std::weak_ptr<FD_EventListener> el);

	//! Updates the states and the scene.
	void update() override;
	//! Renders the scene.
	void render() override;
	//! Provides an event to the manager to process.
	/*!
		\param e The event to process.
	*/
	void pushEvent(const SDL_Event* e) override;

	//! Changes the state of the manager.
	/*!
		\param id The ID of the state to switch to.
	*/
	void setState(int id);

};

#endif