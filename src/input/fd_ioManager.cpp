#include "fd_ioManager.hpp"

#include "../main/fd_handling.hpp"

FD_IOManager::FD_IOManager(SDL_Renderer* renderer) {
	audio = std::make_shared<FD_AudioManager>();
	files = std::make_shared<FD_FileManager>();
	images = std::make_shared<FD_ImageManager>(renderer);
	input = std::make_shared<FD_InputManager>();
}

FD_IOManager::~FD_IOManager() {
	FD_Handling::debug("FD_IOManager destroyed.");
}

void FD_IOManager::update() { 
	audio->update();
	input->update(); 
}

void FD_IOManager::pushEvent(const SDL_Event* e) {
	switch (e->type) {
	case SDL_JOYHATMOTION:
		input->pushJoyDpadEvent(&e->jhat);
		break;
	case SDL_JOYDEVICEADDED:
	case SDL_JOYDEVICEREMOVED:
		input->pushJoyDeviceEvent(&e->jdevice);
		break;
	case SDL_JOYBUTTONUP:
	case SDL_JOYBUTTONDOWN:
		input->pushJoyButtonEvent(&e->jbutton);
		break;
	case SDL_JOYAXISMOTION:
		input->pushJoyAxisEvent(&e->jaxis);
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		input->pushKeyboardEvent(&e->key);
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		input->pushMouseButtonEvent(&e->button);
		break;
	case SDL_MOUSEMOTION:
		input->pushMouseMotionEvent(&e->motion);
		break;
	case SDL_MOUSEWHEEL:
		input->pushMouseWheelEvent(&e->wheel);
		break;
	case SDL_TEXTINPUT:
		input->pushTextInputEvent(&e->text);
		break;
	case SDL_TEXTEDITING:
		input->pushTextEditingEvent(&e->edit);
		break;
	}
}

std::shared_ptr<FD_AudioManager> FD_IOManager::getAudioManager() { return audio; }
std::shared_ptr<FD_FileManager> FD_IOManager::getFileManager() { return files; }
std::shared_ptr<FD_ImageManager> FD_IOManager::getImageManager() { return images; }
std::shared_ptr<FD_InputManager> FD_IOManager::getInputManager() { return input; }
