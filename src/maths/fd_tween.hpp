#ifndef FD_TWEEN_H_
#define FD_TWEEN_H_

#include <functional>

#include <SDL_stdinc.h>

/*!
	@file
	@brief The file containing the FD_Tween class and related structures.
*/

//! The enumeration containing the different accepted types of tween.
enum FD_TweenType {
	//! A linear tween, moves proportionally to time passed.
	FD_TWEEN_LINEAR,
	//! An ease in tween.
	FD_TWEEN_EASE_IN,
	//! An ease out tween.
	FD_TWEEN_EASE_OUT,
	//! An elastic tween, do not use if you are wary of overshooting (albeit temporarily) the destination.
	FD_TWEEN_ELASTIC
};

//! The struct detailing a type of tween 'action' (movement).
/*
	\sa apply
*/
typedef struct FD_TweenAction_ {
	//! The type of tween to be started.
	FD_TweenType type{ FD_TWEEN_LINEAR };
	//! The end point of the tween.
	double destination{ 0 };
	//! The length of time the tween will last in milliseconds.
	Uint32 duration{ 0 };
	//! The length of time the tween will pause before initiating in milliseconds.
	Uint32 pause{ 0 };

	//! The value by which the destination increments when this action is used.
	double value_increment{ 0 };
	//! The value by which the pause increments when this action is used.
	Uint32 pause_increment{ 0 };
	//! The current number of uses this action has experienced.
	int uses{ 0 };
} FD_TweenAction;

//! The class allowing smooth interpolation between numerical values.
class FD_Tween {
private:

	// The type of tween
	FD_TweenType tweenType{ FD_TWEEN_LINEAR };
	// The starting time and the duration
	Uint32 start_time{ 0 }, duration{ 0 };
	// The source and destination values
	double start_value{ 0 }, end_value{ 0 };
	// Variables for detecting whether the tween 
	// has moved since it was last checked
	Uint32 last_checked{ 0 };
	bool force_moved{ true };
	// Whether we have declared that the tween has ended
	bool has_finished{ false };

	Uint32 getProgress();

	static double linearTween(double s, double e, Uint32 t, Uint32 d);
	static double easeInTween(double s, double e, Uint32 t, Uint32 d);
	static double easeOutTween(double s, double e, Uint32 t, Uint32 d);
	static double elasticTween(double s, double e, Uint32 t, Uint32 d);

public:

	//! Constructs a FD_Tween.
	/*!
		\param value The starting value of the tween.
	*/
	FD_Tween(double value = 0);
	//! Destroys the FD_Tween.
	~FD_Tween();
	
	//! Sets the value of the tween.
	/*!
		\param value The value to be set to.

		\sa add
		\sa move
		\sa apply
	*/
	void set(double value);
	//! Adds to the value of the tween.
	/*!
		\param value The value to be added to the tween's current value.

		\sa set
		\sa move
		\sa apply
	*/
	void add(double value);
	//! Applies a tween.
	/*!
		\param tweenType   The type of tween.
		\param destination The end point of the tween.
		\param duration    The length of time the tween will last in milliseconds.
		\param pause       The length of time the tween will pause before initiating in milliseconds.

		\sa set
		\sa add
		\sa apply
	*/
	void move(FD_TweenType tweenType, double destination, Uint32 duration, Uint32 pause = 0);
	//! Applies a tween.
	/*!
		\param action The FD_TweenAction to be applied.

		\sa set
		\sa add
		\sa move
	*/
	void apply(FD_TweenAction &action);

	//! Returns the value of the current destination of the tween.
	/*!
		\return The value of the current destination of the tween.
	*/
	double destination();
	//! Returns the current value of the tween.
	/*!
		\return The current value of the tween.
	*/
	double value();

	//! Returns whether the tween has moved since it was lasted checked.
	/*!
		\return Whether the tween has moved since it was lasted checked.

		\warning This is not a const operation.
	*/
	bool moved();
	//! Returns whether the destination of the tween is equal to the given value.
	/*!
		\param value The destination to check.

		\return Whether the destination of the tween is equal to the given value.
	*/
	bool going(double value) const;
	//! Returns whether the tween has stopped moving.
	/*!
		\return Whether the tween has stopped moving.
	*/
	bool done() const;
	//! Returns whether the tween has finished since it was lasted checked.
	/*!
		\return Whether the tween has finished since it was lasted checked.

		\warning This is not a const operation.
	*/
	bool finished();

};

#endif
