#ifndef FD_FACTORY_HPP_
#define FD_FACTORY_HPP_

#include <memory>

#include "../display/fd_scene.hpp"
#include "../object/fd_object.hpp"
#include "../object/fd_element.hpp"
#include "../object/ui/fd_text.hpp"
#include "../object/ui/fd_textField.hpp"
#include "../object/ui/fd_buttonManager.hpp"

/*!
	@file
	@brief The file containing the classes relating to audio.
*/

//! The class that allows automatic creation and configuring of FD_Object inheriting classes
class FD_Factory {
private:

	std::weak_ptr<FD_Scene> scene;
	std::weak_ptr<FD_ObjectGroup> group{ };

	std::shared_ptr<FD_Scene> lockScene();
	std::shared_ptr<FD_ObjectGroup> lockGroup();

public:

	//! Constructs an FD_Factory.
	/*!
		\param scene The scene for the factory to use.
	*/
	FD_Factory(std::weak_ptr<FD_Scene> scene);
	//! Destroys the FD_Factory.
	~FD_Factory();

	//! Generates a new FD_CameraSet.
	std::shared_ptr<FD_CameraSet> generateCameraSet();

	//! Generates a new FD_Registry.
	std::shared_ptr<FD_Registry> generateRegistry();

	//! Generates a new FD_Tween.
	/*!
		\param v The starting value for the tween.
	*/
	std::shared_ptr<FD_Tween> generateTween(double v);

	//! Generates a new FD_ButtonManager.
	/*!
		\param cameras    The set of cameras for the button manager to use.
		\param input_list The ID of the input list for this box to use.
	*/
	std::shared_ptr<FD_ButtonManager> generateButtonManager(
		std::weak_ptr<FD_CameraSet> cameras,
		int input_list);

	//! Generates a new FD_Text.
	/*!
		\param configure    Whether the factory should assimilate the object.
		\param font         The font used by the text.
		\param prefix       The (fixed) prefix of the text.
		\param text         The variable main text.
		\param suffix       The (fixed) suffix of the text.
		\param colour       The colour of the text.
		\param x            The x-coordinate of the text.
		\param y            The y-coordinate of the text.
		\param style        The drawing style of the text, see FD_DrawStyle.
		\param z            The layer of the text.
		\param camera_bound Whether the text's coordinates are relative to the camera.
	*/
	std::shared_ptr<FD_Text> generateText(
		bool configure,
		std::shared_ptr<FD_Font> font,
		std::string prefix,
		std::string text,
		std::string suffix,
		SDL_Colour colour,
		double x, double y,
		FD_DrawStyle style,
		int z, bool camera_bound);

	//! Generates a new FD_TextBox.
	/*!
		\param configure    Whether the factory should assimilate the object.
		\param s            The scene used to create the image.
		\param type_temp    The template for the text.
		\param x            The x-coordinate of the box.
		\param y            The y-coordinate of the box.
		\param z            The layer of the box.
		\param camera_bound Whether the box is camera_bound or not.
		\param style        The drawing style of the box. 
	*/
	std::shared_ptr<FD_TextBox> generateTextBox(bool configure,
		const FD_TextTemplate& type_temp,
		int x, int y, int z,
		bool camera_bound, FD_DrawStyle style);

	//! Generates a new FD_TextField.
	/*!
		\param configure        Whether the factory should assimilate the object.
		\param temp             The template for this button.
		\param type_temp	    The template for the text.
		\param input_list       The ID of the input list for this box to use.
		\param x                The x-coordinate of this button.
		\param y                The y-coordinate of this button.
		\param code		        The code corresponding to this button.
		\param padding_left     The padding to the left of the text.
		\param padding_top      The padding above the text.
		\param padding_right    The padding to the right of the text.
		\param padding_bottom   The padding below the text.
	*/
	std::shared_ptr<FD_TextField> generateTextField(bool configure,
		const FD_ButtonTemplate& temp,
		const FD_TextTemplate& type_temp,
		int input_list, int x, int y, int code,
		int padding_left, int padding_top,
		int padding_right, int padding_bottom);

	//! Generates a new FD_Element.
	/*!
		\param configure    Whether the factory should assimilate the object.
		\param image        The image to be used.
		\param x            The x-coordinate of the element.
		\param y            The y-coordinate of the element.
		\param angle        The angle of the element.
		\param z            The layer of the element.
		\param scale        The horizontal scale for the element.
		\param scale_y      The vertical scale for the element.
		\param camera_bound Whether the element should be drawn relative to the camera.
		\param style        The drawing style for the element (see FD_DrawStyle).
	*/
	std::shared_ptr<FD_Element> generateElement(bool configure, std::weak_ptr<FD_Image> image, double x, double y,
		double angle, int z, double scale = 1, double scale_y = -1,
		bool camera_bound = false, FD_DrawStyle style = FD_CENTERED);

	//! Generates a new FD_Box.
	/*!
		\param configure    Whether the factory should assimilate the object.
		\param x            The x-coordinate of the rectangle.
		\param y            The y-coordinate of the rectangle.
		\param w            The width of the rectangle.
		\param h            The height of the rectangle.
		\param z            The layer of the line.
		\param camera_bound Whether the object is drawn relative to the camera.
		\param colour       The colour of the line.
	*/
	std::shared_ptr<FD_Box> generateBox(bool configure, double x, double y, double w, 
		double h, int z, bool camera_bound, SDL_Colour colour);

	//! Generates a new FD_Line.
	/*!
		\param configure    Whether the factory should assimilate the object.
		\param x1           The x-coordinate of the first point of the line.
		\param y1           The y-coordinate of the first point of the line.
		\param x2           The x-coordinate of the second point of the line.
		\param y2           The y-coordinate of the second point of the line.
		\param z            The layer of the line.
		\param camera_bound Whether the object is drawn relative to the camera.
		\param colour       The colour of the line.
	*/
	std::shared_ptr<FD_Line> generateLine(bool configure, double x1, double y1, 
		double x2, double y2, int z, bool camera_bound, SDL_Colour colour);

	//! Generates a new FD_ObjectGroup.
	/*!
		\param configure Whether the object group should be added to the scene.
		\param set       Whether the factory should operate on this group.
		\param o         The object list index (given by FD_Scene).
	*/
	std::shared_ptr<FD_ObjectGroup> generateObjectGroup(bool configure, bool set, int o);

	//! Configures an object to the current group.
	/*!
		\param o The object to configure
	*/
	void configure(std::shared_ptr<FD_Line> o);
	//! Configures an object to the current group.
	/*!
		\param o The object to configure
	*/
	void configure(std::shared_ptr<FD_Box> o);
	//! Configures an object to the current group.
	/*!
		\param o The object to configure
	*/
	void configure(std::shared_ptr<FD_Object> o);

	//! Sets the current group the factory is using
	void setGroup(std::weak_ptr<FD_ObjectGroup> group);

};

#endif
