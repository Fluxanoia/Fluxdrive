#ifndef FD_RANDOM_GEN_H_
#define FD_RANDOM_GEN_H_

#include <random>

#include <SDL_timer.h>

/*!
	@file
	@brief The file containing the FD_RandomGenerator class.
*/

//! The class that handles random number generation.
class FD_RandomGenerator {
private:

	// For random generation
	std::random_device device;
	std::mt19937 generator{ device() };

	// The last recorded tick
	Uint32 last_ticks;
	// The ticks between endpoints
	Uint32 noise_delta;
	// The noise endpoints
	double noise_1, noise_2;

	// Gets the smoothed interpolation value
	double getInterpolatedValue(double interpolation);

public:

	//! Constructs the FD_RandomGenerator.
	/*!
		\param noise_delta The number of updates between noise generation endpoints.

		\sa getNoise
	*/
	FD_RandomGenerator(Uint32 noise_delta = 10);
	//! Destroys the FD_RandomGenerator.
	~FD_RandomGenerator();

	//! Generates random integers between v1 (inclusive) and v2 (exclusive).
	/*!
		\param v1 The lower bound for the number to be generated.
		\param v2 The upper bound for the number to be generated.

		\return The random integer.
	*/
	int randomInteger(int v1, int v2);
	//! Generates random doubles between v1 (inclusive) and v2 (exclusive).
	/*!
		\param v1 The lower bound for the number to be generated.
		\param v2 The upper bound for the number to be generated.

		\return The random double.
	*/
	double randomDouble(double v1 = 0, double v2 = 1);

	//! Returns the current noise value.
	/*!
		This function generates pseudo-Perlin noise and is based
		off of the SDL_Ticks function provided by SDL.

		\returns The current noise value.
	*/
	double getNoise();

};

#endif
