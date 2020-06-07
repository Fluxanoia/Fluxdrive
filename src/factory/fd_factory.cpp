#include "fd_factory.hpp"

std::shared_ptr<FD_Scene> FD_Factory::lockScene() {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene);
	return scene;
}

std::shared_ptr<FD_ObjectGroup> FD_Factory::lockGroup() {
	std::shared_ptr<FD_ObjectGroup> group;
	FD_Handling::lock(this->group, group);
	return group;
}

FD_Factory::FD_Factory(std::weak_ptr<FD_Scene> scene) : scene{ scene } {}
FD_Factory::~FD_Factory() {}

std::shared_ptr<FD_CameraSet> FD_Factory::generateCameraSet() {
	std::shared_ptr<FD_Scene> scene{ this->lockScene() };
	return std::make_shared<FD_CameraSet>(scene->getWindow());
}

std::shared_ptr<FD_Registry> FD_Factory::generateRegistry() {
	return std::make_shared<FD_Registry>();
}

std::shared_ptr<FD_Tween> FD_Factory::generateTween(double v) {
	return std::make_shared<FD_Tween>(v);
}

std::shared_ptr<FD_ButtonManager> FD_Factory::generateButtonManager(
		std::weak_ptr<FD_CameraSet> cameras,
		int input_list) {
	return std::make_shared<FD_ButtonManager>(scene, cameras, input_list);
}

std::shared_ptr<FD_Text> FD_Factory::generateText(bool configure,
		std::shared_ptr<FD_Font> font,
		std::string prefix,
		std::string text,
		std::string suffix,
		SDL_Colour colour,
		double x, double y,
		FD_DrawStyle style,
		int z, bool camera_bound) {
	std::shared_ptr<FD_Scene> scene{ this->lockScene() };
	std::shared_ptr<FD_Text> t{
		std::make_shared<FD_Text>(
			scene->getWindow()->getRenderer(), font, prefix, text, 
			suffix, colour, x, y, style, z, camera_bound
		)
	};
	if (configure) this->configure(t);
	return t;
}

std::shared_ptr<FD_TextBox> FD_Factory::generateTextBox(
		bool configure,
		const FD_TextTemplate& type_temp,
		int x, int y, int z,
		bool camera_bound, FD_DrawStyle style) {
	std::shared_ptr<FD_TextBox> text_box{
		std::make_shared<FD_TextBox>(
			scene, type_temp, x, y, z, camera_bound, style
		)
	};
	if (configure) this->configure(text_box);
	return text_box;
}

std::shared_ptr<FD_TextField> FD_Factory::generateTextField(
		bool configure,
		const FD_ButtonTemplate& temp,
		const FD_TextTemplate& type_temp,
		int input_list, int x, int y, int code,
		int padding_left, int padding_top,
		int padding_right, int padding_bottom) {
	std::shared_ptr<FD_TextField> text_field{
		std::make_shared<FD_TextField>(
			temp, type_temp, input_list, x, y, code,
			padding_left, padding_top, padding_right, padding_bottom
		)
	};
	if (configure) this->configure(text_field);
	return text_field;
}

std::shared_ptr<FD_Element> FD_Factory::generateElement(
	bool configure, std::weak_ptr<FD_Image> image, 
	double x, double y, double angle, int z, 
	double scale, double scale_y, bool camera_bound, FD_DrawStyle style) {
	std::shared_ptr<FD_Element> element{
		std::make_shared<FD_Element>(
			image, x, y, angle, z, scale, scale_y, camera_bound, style
		)
	};
	if (configure) this->configure(element);
	return element;
}

std::shared_ptr<FD_Box> FD_Factory::generateBox(bool configure, 
	double x, double y, double w, double h, int z, 
	bool camera_bound, SDL_Colour colour) {
	std::shared_ptr<FD_Box> box{
		std::make_shared<FD_Box>(x, y, w, h, z, camera_bound, colour)
	};
	if (configure) this->configure(box);
	return box;
 }

std::shared_ptr<FD_Line> FD_Factory::generateLine(bool configure, 
		double x1, double y1, double x2, double y2, int z, 
		bool camera_bound, SDL_Colour colour) {
	std::shared_ptr<FD_Line> line{
		std::make_shared<FD_Line>(x1, y1, x2, y2, z, camera_bound, colour)
	};
	if (configure) this->configure(line);
	return line;
}

std::shared_ptr<FD_ObjectGroup> FD_Factory::generateObjectGroup(bool configure, bool set, int o) {
	std::shared_ptr<FD_ObjectGroup> og{ std::make_shared<FD_ObjectGroup>(o) };
	if (configure) {
		std::shared_ptr<FD_Scene> scene{ this->lockScene() };
		scene->addObjectGroup(og);
	}
	if (set) this->setGroup(og);
	return og;
}

void FD_Factory::configure(std::shared_ptr<FD_Line> o) {
	o->assimilate(this->lockGroup());
}
void FD_Factory::configure(std::shared_ptr<FD_Box> o) {
	o->assimilate(this->lockGroup());
}
void FD_Factory::configure(std::shared_ptr<FD_Object> o) {
	o->assimilate(this->lockGroup());
}

void FD_Factory::setGroup(std::weak_ptr<FD_ObjectGroup> group) {
	this->group = group;
}
