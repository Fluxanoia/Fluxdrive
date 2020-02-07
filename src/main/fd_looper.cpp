#include "fd_looper.hpp"

// Loopable Member Functions

void FD_Loopable::update() {}
void FD_Loopable::render() {}
void FD_Loopable::pushEvent(const SDL_Event* e) {}
void FD_Loopable::forceClose() { closed = true; }
bool FD_Loopable::hasClosed() { return closed; }

// Looper Member Functions

FD_Looper::FD_Looper(std::shared_ptr<FD_Loopable> loopable, Uint32 ups) {
	if (loopable == nullptr) FD_Handling::error("The given loopable was nullptr.", true);
	this->loopable = loopable;
	this->ups = ups;
}
FD_Looper::~FD_Looper() {}

void FD_Looper::loop() {
	// Run the loop
	FD_Handling::debug("Running the game loop...\n");
	SDL_Event e;
	// Prepare chrono variables
	using namespace std::chrono_literals;
	std::chrono::nanoseconds lag{ 0ns }, delta{ 0ns };
	auto now{ std::chrono::high_resolution_clock::now() };
	auto last{ std::chrono::high_resolution_clock::now() };
	const std::chrono::nanoseconds second{ static_cast<Uint32>(pow(10, 9)) };
	const std::chrono::nanoseconds timestep{ static_cast<Uint32>(pow(10, 9) / 
		static_cast<double>(ups)) };
	// Initialise debug tracking variables
	auto last_debug{ std::chrono::high_resolution_clock::now() };
	int frames{ 0 };
	int ticks{ 0 };
	while (!loopable->hasClosed()) {
		// Poll events
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				loopable->forceClose();
				continue;
			default:
				loopable->pushEvent(&e);
				continue;
			}
		}
		// Update where needed
		now = std::chrono::high_resolution_clock::now();
		delta = now - last;
		last = now;
		lag += delta;
		while (lag >= timestep) {
			// Update
			ticks++;
			loopable->update();
			lag -= timestep;
		}
		// Draw when possible
		frames++;
		loopable->render();
		// Print debug
		while (now - last_debug >= second) {
			last_debug += second;
			std::string debug{ 
				"Frames: " + std::to_string(frames) +
				"  | Updates: " + std::to_string(ticks) };
			FD_Handling::debug(debug.c_str());
			frames = ticks = 0;
		}
	}
}