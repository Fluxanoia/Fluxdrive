#include "fd_element.hpp"

#include "../display/fd_scene.hpp"

FD_Element::FD_Element(std::weak_ptr<FD_Image> image, double x,
	double y, double angle, int z, double scale, double scale_y,
	bool camera_bound, FD_DrawStyle style)
	: FD_Object(z, camera_bound, x, y, 0, 0, angle) {
	// Set image
	this->setImage(image);
	this->scale_w->set(scale);
	this->scale_h->set((scale_y < 0) ? scale : scale_y);
	// Set draw style
	draw_style = style;
	// Create the bounds
	this->dstrect = new SDL_Rect();
	this->updateBounds();
}
FD_Element::~FD_Element() {}

void FD_Element::update() {
	if (x->moved() || y->moved() || angle->moved()
		|| scale_w->moved() || scale_h->moved()) {
		updateBounds();
	}
}

void FD_Element::updateBounds() {
	if (dstrect != nullptr) {
		FD_Object::updateBounds(dstrect, x->value(),
			y->value(), getWidth(),
			getHeight(), draw_style);
	}
}

void FD_Element::setVisible(bool visible) {
	this->visible = visible;
}
void FD_Element::setOverlayColour(SDL_Colour colour) {
	if (auto image = this->image.lock()) {
		image->setOverlayColour(colour);
	}
}
void FD_Element::setUnderlayColour(SDL_Colour colour) {
	if (auto image = this->image.lock()) {
		image->setUnderlayColour(colour);
	}
}
void FD_Element::setImage(std::weak_ptr<FD_Image> image) {
	this->image = image;
	if (auto i = image.lock()) {
		w->set(i->getWidth());
		h->set(i->getHeight());
	} else {
		w->set(0);
		h->set(0);
	}
	updateBounds();
}
void FD_Element::setSourceRect(SDL_Rect rect) {
	if (srcrect == nullptr) srcrect = new SDL_Rect();
	srcrect->x = rect.x;
	srcrect->y = rect.y;
	srcrect->w = rect.w;
	srcrect->h = rect.h;
	FD_Element::updateBounds();
}
void FD_Element::removeSourceRect() {
	if (srcrect != nullptr) {
		delete srcrect;
		srcrect = nullptr;
	}
}
void FD_Element::setWidth(int width) {
	if (auto image = this->image.lock()) {
		scale_w->set(width / static_cast<double>(image->getWidth()));
	}
}
void FD_Element::setHeight(int height) {
	if (auto image = this->image.lock()) {
		scale_h->set(height / static_cast<double>(image->getHeight()));
	}
}
void FD_Element::setFlipFlags(SDL_RendererFlip f) { flip_flags = f; }

std::shared_ptr<FD_Image> FD_Element::getImage() const {
	if (auto image = this->image.lock()) {
		return (visible) ? image : nullptr;
	} else {
		return nullptr;
	}
}

double FD_Element::getWidth() const {
	if (srcrect == nullptr) {
		if (auto image = this->image.lock()) {
			return static_cast<int>(image->getWidth() * scale_w->value());
		} else {
			return 0;
		}
	}
	return static_cast<int>(srcrect->w * scale_w->value());
}
double FD_Element::getHeight() const {
	if (srcrect == nullptr) {
		if (auto image = this->image.lock()) {
			return static_cast<int>(image->getHeight() * scale_h->value());
		} else {
			return 0;
		}
	}
	return static_cast<int>(srcrect->h * scale_h->value());
}

FD_Tween* FD_Element::getTweenX() { return x; }
FD_Tween* FD_Element::getTweenY() { return y; }
FD_Tween* FD_Element::getTweenAngle() { return angle; }
FD_Tween* FD_Element::getTweenScaleX() { return scale_w; }
FD_Tween* FD_Element::getTweenScaleY() { return scale_h; }
FD_Tween* FD_Element::getTweenOpacity() { return opacity; }
FD_Tween* FD_Element::getTweenCenterX() { return center_x; }
FD_Tween* FD_Element::getTweenCenterY() { return center_y; }

void FD_Element::assimilate(std::shared_ptr<FD_ObjectGroup> group) {
	group->addObject(shared_from_this());
}
