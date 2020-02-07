#ifndef FD_RESIZABLE_H_
#define FD_RESIZABLE_H_

/*!
	@file
	@brief The file containing the FD_Resizable class.
*/

//! The class that allows other classes to be notified of changes in resolution.
class FD_Resizable {
public:
	//! Updates the class with a change in dimensions.
	/*!
		\param width  The new width.
		\param height The new height.
	*/
	virtual void resized(int width, int height) = 0;
};

#endif
