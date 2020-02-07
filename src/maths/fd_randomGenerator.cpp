#include "fd_randomGenerator.hpp"

#include <cmath>

#include "../maths/fd_maths.hpp"

FD_RandomGenerator::FD_RandomGenerator(Uint32 noise_delta) : noise_delta{ noise_delta } {
	noise_1 = randomDouble(-1, 1);
	noise_2 = randomDouble(-1, 1);
	last_ticks = SDL_GetTicks();
}
FD_RandomGenerator::~FD_RandomGenerator() { }

int FD_RandomGenerator::randomInteger(int v1, int v2) {
	return static_cast<int>(floor(randomDouble(v1, v2)));
}
double FD_RandomGenerator::randomDouble(double v1, double v2) {
	std::uniform_real_distribution<> urd(v1, v2);
	return urd(generator);
}

double FD_RandomGenerator::getNoise() {
	Uint32 ticks = SDL_GetTicks();
	Uint32 diff = ticks - last_ticks;
	if (diff > noise_delta) {
		if (diff > 2 * noise_delta) {
			noise_1 = randomDouble(-1, 1);
			noise_2 = randomDouble(-1, 1);
		} else {
			noise_1 = noise_2;
			noise_2 = randomDouble(-1, 1);
		}
		while (ticks - last_ticks > noise_delta) last_ticks += noise_delta;
	}
	double interpolation = static_cast<double>(ticks - last_ticks);
	interpolation /= static_cast<double>(noise_delta);
	return getInterpolatedValue(interpolation);
}

double FD_RandomGenerator::getInterpolatedValue(double interpolation) {
	double ease = 0.5 * (1 - cos(FD_PI * interpolation));
	return noise_1 + (noise_2 - noise_1) * ease;
}
