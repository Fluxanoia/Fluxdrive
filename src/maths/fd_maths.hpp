#ifndef FD_CONSTANTS_H_
#define FD_CONSTANTS_H_

#include <SDL_rect.h>

/*!
	@file
	@brief A file containing the FD_Maths namespace.
*/

//! The defined value of PI used by Fluxdrive.
#define FD_PI 3.14159

//! The structure defining a Fluxdrive point.
typedef struct FD_Point_ {
	//! The x-coordinate of the point.
	double x;
	//! The y-coordinate of the point.
	double y;
} FD_Point;

//! The structure defining a Fluxdrive rectangle.
typedef struct FD_Rect_ {
	//! The x-coordinate of the rectangle.
	double x;
	//! The y-coordinate of the rectangle.
	double y;
	//! The width of the rectangle.
	double w;
	//! The height of the rectangle.
	double h;
} FD_Rect;

//! The namespace containing generic mathematical operations.
namespace FD_Maths {

	//! Extrudes a rectangle.
	/*! Increases the width and height by double the given extrusion value,
		centered about the centre of the rectangle.
		\param rect The rectangle to extrude.
		\param size The amount to extrude.
		\return The extruded rectangle.
		\sa extrusion */
	SDL_Rect extrude(const SDL_Rect rect, const int size);
	//! Extrudes a rectangle.
	/*! Increases the width and height by double the given extrusion value,
		centered about the centre of the rectangle.
		\param rect The rectangle to extrude.
		\param size The amount to extrude.
		\return The extruded rectangle.
		\sa extrusion */
	FD_Rect extrude(const FD_Rect rect, const double size);

	//! Checks whether a point is in a rectangle or not.
	/*! This operation is inclusive of the edges of the rectangle.
		\param x The x coordinate of the point in question.
		\param y The y coordinate of the point in question.
		\param r The rectangle in question.
		\return Returns whether the given point is in the rectangle or not.
		\sa pointInRect */
	bool pointInRect(const int x, const int y, const SDL_Rect* r);
	//! Checks whether a point is in a rectangle or not.
	/*! This operation is inclusive of the edges of the rectangle.
		\param x The x coordinate of the point in question.
		\param y The y coordinate of the point in question.
		\param r The rectangle in question.
		\return Returns whether the given point is in the rectangle or not.
		\sa pointInRect */
	bool pointInRect(const double x, const double y, const FD_Rect* r);
	//! Checks whether a point is in a rectangle or not.
	/*! This operation is inclusive of the edges of the rectangle.
		\param p The point in question.
		\param r The rectangle in question.
		\return Returns whether the given point is in the rectangle or not.
		\sa pointInRect */
	bool pointInRect(const SDL_Point* p, const SDL_Rect* r);
	//! Checks whether a point is in a rectangle or not.
	/*! This operation is inclusive of the edges of the rectangle.
		\param p The point in question.
		\param r The rectangle in question.
		\return Returns whether the given point is in the rectangle or not.
		\sa pointInRect */
	bool pointInRect(const FD_Point* p, const FD_Rect* r);
	//! Checks whether two rectangles intersect.
	/*! This operation is inclusive of the edges of the rectangles.
		\param a The first rectangle in question.
		\param b The second rectangle in question.
		\return Returns whether the rectangle intersect.
		\sa intersection */
	bool hasIntersection(const SDL_Rect* a, const SDL_Rect* b);
	//! Checks whether two rectangles intersect.
	/*! This operation is inclusive of the edges of the rectangles.
		\param a The first rectangle in question.
		\param b The second rectangle in question.
		\return Returns whether the rectangle intersect.
		\sa intersection */
	bool hasIntersection(const FD_Rect* a, const FD_Rect* b);
	//! Calculates the intersection of two rectangles.
	/*! This operation is inclusive of the edges of the rectangles.
		The intersection is written to the intersection parameter.
		\param a            The first rectangle in question.
		\param b            The second rectangle in question.
		\param intersection The rectangle for the intersection to be written to.
		\return Returns whether the rectangle intersect.
		\sa hasIntersection */
	bool intersection(const SDL_Rect* a, const SDL_Rect* b, SDL_Rect* intersection);
	//! Calculates the intersection of two rectangles.
	/*! This operation is inclusive of the edges of the rectangles.
		The intersection is written to the intersection parameter.
		\param a            The first rectangle in question.
		\param b            The second rectangle in question.
		\param intersection The rectangle for the intersection to be written to.
		\return Returns whether the rectangle intersect.
		\sa hasIntersection */
	bool intersection(const FD_Rect* a, const FD_Rect* b, FD_Rect* intersection);
	
}

#endif
