#include "fd_handling.hpp"

namespace FD_Handling {

	void throw_and_exit(bool to_throw, bool to_exit) {
		if (to_throw) throw FD_HandlingException();
		if (to_exit) exit(EXIT_FAILURE);
	}

	void error(const char* s, bool fatal) {
		std::cerr << s << std::endl;
		throw_and_exit(fatal, fatal);
	}

	void errorSDL(const char* s, bool fatal) {
		std::cerr << s << std::endl;
		std::cerr << "SDL_Error: " << SDL_GetError() << std::endl;
		throw_and_exit(fatal, fatal);
	}

	void errorIMG(const char* s, bool fatal) {
		std::cerr << s << std::endl;
		std::cerr << "IMG_Error: " << IMG_GetError() << std::endl;
		throw_and_exit(fatal, fatal);
	}

	void errorTTF(const char* s, bool fatal) {
		std::cerr << s << std::endl;
		std::cerr << "TTF_Error: " << TTF_GetError() << std::endl;
		throw_and_exit(fatal, fatal);
	}

	void debug(const char* s) {
#ifdef FD_DEBUG
		std::cout << s << std::endl;
#endif
	}

	void debug_alert() {
#ifdef FD_DEBUG
		std::cout << "!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		std::cout << "!   FLUXDRIVE ALERT   !" << std::endl;
		std::cout << "!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
#endif
	}

}
