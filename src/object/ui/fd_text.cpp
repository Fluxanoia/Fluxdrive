#include "fd_text.hpp"

FD_Text::FD_Text(SDL_Renderer* renderer,
	std::shared_ptr<FD_Font> font,
	std::string prefix,
	std::string text,
	std::string suffix,
	SDL_Colour colour,
	double x, double y,
	FD_DrawStyle style,
	int z, bool camera_bound) : FD_Object(z, camera_bound, x, y) {
	// Set renderer
	this->renderer = renderer;
	// Set image
	this->dstrect = new SDL_Rect();
	image = std::make_shared<FD_TextImage>(renderer, font,
		prefix, "",
		suffix, colour);
	this->changeText(text);
	// Create the bounds
	this->draw_style = style;
	updateBounds();
}
FD_Text::~FD_Text() {}

void FD_Text::updateBounds() {
	FD_Object::updateBounds(dstrect,
		x->value(), y->value(),
		w->value() * scale_w->value(),
		h->value() * scale_h->value(), draw_style);
}
void FD_Text::update() {
	if (x->moved() || y->moved() || w->moved() || h->moved()) {
		updateBounds();
	}
}
void FD_Text::changeText(std::string text) {
	image->changeText(renderer, text);
	this->w->set(image->getWidth());
	this->h->set(image->getHeight());
	updateBounds();
}

void FD_Text::setVisible(bool visible) {
	this->visible = visible;
}
void FD_Text::setOverlayColour(SDL_Colour colour) {
	image->setOverlayColour(colour);
}
void FD_Text::setUnderlayColour(SDL_Colour colour) {
	image->setUnderlayColour(colour);
}

std::shared_ptr<FD_Image> FD_Text::getImage() const {
	return (visible) ? image : nullptr;
}

FD_Tween* FD_Text::getTweenX() { return x; }
FD_Tween* FD_Text::getTweenY() { return y; }
FD_Tween* FD_Text::getTweenScaleX() { return scale_w; }
FD_Tween* FD_Text::getTweenScaleY() { return scale_h; }
FD_Tween* FD_Text::getTweenOpacity() { return opacity; }

void FD_Text::assimilate(std::shared_ptr<FD_ObjectGroup> group) {
	group->addObject(shared_from_this());
}
