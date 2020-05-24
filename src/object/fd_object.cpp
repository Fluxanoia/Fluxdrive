#include "fd_object.hpp"

// FD_Object Member Functions

FD_Object::FD_Object(int z, bool camera_bound, double x, double y, double w, double h,
	double angle, double scale_w, double scale_h, Uint8 opacity, bool visible)
	: FD_Layered(z, camera_bound) {
	this->x = new FD_Tween(x);
	this->y = new FD_Tween(y);
	this->w = new FD_Tween(w);
	this->h = new FD_Tween(h);
	this->angle = new FD_Tween(angle);
	this->scale_w = new FD_Tween(scale_w);
	this->scale_h = new FD_Tween(scale_h);
	this->opacity = new FD_Tween(opacity);
	this->center_x = new FD_Tween(0.5);
	this->center_y = new FD_Tween(0.5);
	this->visible = visible;
}
FD_Object::~FD_Object() {
	delete x;
	delete y;
	delete w;
	delete h;
	delete angle;
	delete scale_w;
	delete scale_h;
	delete opacity;
	delete center_x;
	delete center_y;
	if (srcrect != nullptr) delete srcrect;
	if (dstrect != nullptr) delete dstrect;
}

void FD_Object::render(SDL_Renderer* renderer, const Uint8 alpha,
	const std::shared_ptr<const FD_Camera> camera) const {
	std::shared_ptr<FD_Image> image{ getImage() };
	Uint8 opacity{ getOpacity() };
	if (alpha == 0) {
		opacity = 0;
	} else if (alpha != 255) {
		opacity = static_cast<Uint8>((alpha / 255.0) * opacity);
	}
	SDL_Rect dr{ };
	dr.x = getDestinationRect()->x;
	dr.y = getDestinationRect()->y;
	dr.w = getDestinationRect()->w;
	dr.h = getDestinationRect()->h;
	double angle{ getAngle() };
	if (this->isCameraBound()) {
		if (!camera->manipulate(dr, angle)) return;
	}
	image->render(renderer, opacity, getSourceRect(), &dr,
		angle, getCenterX(), getCenterY(), getFlipFlags(), 
		getBlendMode(), getClipRect());
}

void FD_Object::updateBounds(SDL_Rect* rect) {
	this->updateBounds(rect, this->x->value(), this->y->value(),
		this->w->value(), this->h->value(), draw_style);
}
void FD_Object::updateBounds(SDL_Rect* rect, double x, double y,
	double w, double h, FD_DrawStyle style) {
	if (rect == nullptr) {
		FD_Handling::error("Null rectangle being updated.");
		return;
	}
	rect->w = static_cast<int>(w);
	rect->h = static_cast<int>(h);
	switch (style) {
	case FD_TOP_LEFT:
		rect->x = static_cast<int>(x);
		rect->y = static_cast<int>(y);
		break;
	case FD_TOP_RIGHT:
		rect->x = static_cast<int>(x - rect->w);
		rect->y = static_cast<int>(y);
		break;
	case FD_BOTTOM_LEFT:
		rect->x = static_cast<int>(x);
		rect->y = static_cast<int>(y - rect->h);
		break;
	case FD_BOTTOM_RIGHT:
		rect->x = static_cast<int>(x - rect->w);
		rect->y = static_cast<int>(y - rect->h);
		break;
	default: // CENTERED case
		rect->x = static_cast<int>(x - (rect->w / 2.0));
		rect->y = static_cast<int>(y - (rect->h / 2.0));
		break;
	}
}

double FD_Object::getX() const { return x->value(); }
double FD_Object::getY() const { return y->value(); }
double FD_Object::getWidth() const {
	double v = w->value();
	if (v < 0) return 0;
	return v;
}
double FD_Object::getHeight() const {
	double v = h->value();
	if (v < 0) return 0;
	return v;
}
double FD_Object::getWidthScale() const {
	double v = scale_w->value();
	if (v < 0) return 0;
	return v;
}
double FD_Object::getHeightScale() const {
	double v = scale_h->value();
	if (v < 0) return 0;
	return v;
}
bool FD_Object::isVisible() const { return visible; }

SDL_Rect* FD_Object::getSourceRect() const { return srcrect; }
SDL_Rect* FD_Object::getDestinationRect() const { return dstrect; }
double FD_Object::getAngle() const { return angle->value(); }
double FD_Object::getCenterX() const { return center_x->value(); };
double FD_Object::getCenterY() const { return center_y->value(); };
SDL_RendererFlip FD_Object::getFlipFlags() const { return flip_flags; }
SDL_BlendMode FD_Object::getBlendMode() const { return blend_mode;  }
SDL_Rect* FD_Object::getClipRect() const { return clip_rect; }
Uint8 FD_Object::getOpacity() const {
	double v = opacity->value();
	if (v > 255) return 255;
	if (v < 0) return 0;
	return static_cast<Uint8>(v);
}

// FD_Line Member Functions

FD_Line::FD_Line(double x1, double y1, double x2, double y2, int z,
	bool camera_bound, SDL_Colour colour) : FD_Layered(z, camera_bound) {
	this->colour = colour;
	this->x1 = new FD_Tween(x1);
	this->y1 = new FD_Tween(y1);
	this->x2 = new FD_Tween(x2);
	this->y2 = new FD_Tween(y2);
	this->opacity = new FD_Tween(255);
}
FD_Line::~FD_Line() {
	delete x1;
	delete y1;
	delete x2;
	delete y2;
	delete opacity;
}

void FD_Line::removePoints() {
	p1 = nullptr;
	p2 = nullptr;
}
void FD_Line::supplyPoints(SDL_Point* p1, SDL_Point* p2) {
	this->p1 = p1;
	this->p2 = p2;
}

void FD_Line::supplyClipRect(SDL_Rect* rect) {
	this->clip_rect = rect;
}
void FD_Line::removeClipRect() {
	this->clip_rect = nullptr;
}

void FD_Line::render(SDL_Renderer* renderer, const Uint8 alpha,
	const std::shared_ptr<const FD_Camera> camera) const {
	if (alpha == 0 || !visible) return;
	SDL_BlendMode old_blend{ };
	SDL_GetRenderDrawBlendMode(renderer, &old_blend);
	SDL_SetRenderDrawBlendMode(renderer, blend_mode);
	Uint8 colour_alpha = colour.a;
	if (alpha != 255) {
		colour_alpha = static_cast<Uint8>(colour.a * (alpha / 255.0)
			* (opacity->value() / 255.0));
	}
	SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour_alpha);
	// Get the dstrect to manipulate
	int min_x, min_y, max_x, max_y;
	if (p1 == nullptr || p2 == nullptr) {
		min_x = static_cast<int>(x1->value());
		min_y = static_cast<int>(y1->value());
		max_x = static_cast<int>(x2->value());
		max_y = static_cast<int>(y2->value());
	} else {
		min_x = p1->x;
		min_y = p1->y;
		max_x = p2->x;
		max_y = p2->y;
	}
	if (min_x > max_x) {
		int temp{ max_x };
		max_x = min_x;
		min_x = temp;
	}
	if (min_y > max_y) {
		int temp{ max_y };
		max_y = min_y;
		min_y = temp;
	}
	SDL_Rect dr{ min_x, min_y, max_x - min_x, max_y - min_y };
	if (this->isCameraBound()) {
		double angle;
		if (!camera->manipulate(dr, angle)) return;
	}
	SDL_Rect* old_clip{ nullptr };
	bool clipping{ clip_rect != nullptr };
	if (clipping) {
		SDL_RenderGetClipRect(renderer, old_clip);
		SDL_RenderSetClipRect(renderer, clip_rect);
	}
	SDL_RenderDrawLine(renderer, dr.x, dr.y, dr.x + dr.w, dr.y + dr.h);
	if (clipping) SDL_RenderSetClipRect(renderer, old_clip);
	SDL_SetRenderDrawBlendMode(renderer, old_blend);
}

void FD_Line::setColour(SDL_Colour colour) { this->colour = colour; }
void FD_Line::setVisible(bool visible) { this->visible = visible; }
void FD_Line::setBlendMode(SDL_BlendMode bm) { this->blend_mode = bm; }

SDL_Colour FD_Line::getColour() const { return colour; }
bool FD_Line::isVisible() const { return visible; }

double FD_Line::getX1() const { return x1->value(); }
double FD_Line::getY1() const { return y1->value(); }
double FD_Line::getX2() const { return x2->value(); }
double FD_Line::getY2() const { return y2->value(); }
Uint8 FD_Line::getOpacity() const {
	return static_cast<int>(opacity->value());
}
SDL_BlendMode FD_Line::getBlendMode() const { return blend_mode; }

void FD_Line::updatePoints(double x1, double y1, double x2, double y2) {
	this->x1->set(x1);
	this->y1->set(y1);
	this->x2->set(x2);
	this->y2->set(y2);
}

FD_Tween* FD_Line::getTweenX1() { return x1; }
FD_Tween* FD_Line::getTweenY1() { return y1; }
FD_Tween* FD_Line::getTweenX2() { return x2; }
FD_Tween* FD_Line::getTweenY2() { return y2; }
FD_Tween* FD_Line::getTweenOpacity() { return opacity; }

void FD_Line::assimilate(std::shared_ptr<FD_ObjectGroup> group) {
	group->addObject(shared_from_this());
}

// Box Member Functions

FD_Box::FD_Box(double x, double y, double w, double h, int z,
	bool camera_bound, SDL_Colour colour) : FD_Layered(z, camera_bound) {
	this->colour = colour;
	this->x = new FD_Tween(x);
	this->y = new FD_Tween(y);
	this->w = new FD_Tween(w);
	this->h = new FD_Tween(h);
	this->opacity = new FD_Tween(255);
}
FD_Box::~FD_Box() {
	delete x;
	delete y;
	delete w;
	delete h;
	delete opacity;
}

void FD_Box::removeRect() {
	rect = nullptr;
}
void FD_Box::supplyRect(SDL_Rect* rect) {
	this->rect = rect;
}

void FD_Box::supplyClipRect(SDL_Rect* rect) {
	this->clip_rect = rect;
}
void FD_Box::removeClipRect() {
	this->clip_rect = nullptr;
}

void FD_Box::render(SDL_Renderer* renderer, const Uint8 alpha,
	const std::shared_ptr<const FD_Camera> camera) const {
	if (alpha == 0 || !visible) return;
	Uint8 colour_alpha = static_cast<Uint8>(colour.a * (alpha / 255.0)
		* (opacity->value() / 255.0));
	Uint8 underlay_alpha = static_cast<Uint8>(underlay_colour.a * (alpha / 255.0)
		* (opacity->value() / 255.0));
	Uint8 overlay_alpha = static_cast<Uint8>(overlay_colour.a * (alpha / 255.0)
		* (opacity->value() / 255.0));
	SDL_Rect dr{};
	if (rect == nullptr) {
		dr.x = static_cast<int>(x->value());
		dr.y = static_cast<int>(y->value());
		dr.w = static_cast<int>(w->value());
		dr.h = static_cast<int>(h->value());
	} else {
		dr = SDL_Rect(*rect);
	}
	if (this->isCameraBound()) {
		double angle;
		if (!camera->manipulate(dr, angle)) return;
	}
	SDL_Rect* old_clip{ nullptr };
	bool clipping{ clip_rect != nullptr };
	if (clipping) {
		SDL_RenderGetClipRect(renderer, old_clip);
		SDL_RenderSetClipRect(renderer, clip_rect);
	}
	SDL_BlendMode old_blend{ };
	SDL_GetRenderDrawBlendMode(renderer, &old_blend);
	SDL_SetRenderDrawBlendMode(renderer, blend_mode);

	SDL_SetRenderDrawColor(renderer, underlay_colour.r,
		underlay_colour.g, underlay_colour.b, underlay_alpha);
	SDL_RenderFillRect(renderer, &dr);
	SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour_alpha);
	SDL_RenderDrawRect(renderer, &dr);
	SDL_SetRenderDrawColor(renderer, overlay_colour.r,
		overlay_colour.g, overlay_colour.b, overlay_alpha);
	SDL_RenderFillRect(renderer, &dr);

	if (clipping) SDL_RenderSetClipRect(renderer, old_clip);
	SDL_SetRenderDrawBlendMode(renderer, old_blend);
}

void FD_Box::setUnderlayColour(SDL_Colour colour) { this->underlay_colour = colour; }
void FD_Box::setColour(SDL_Colour colour) { this->colour = colour; }
void FD_Box::setOverlayColour(SDL_Colour colour) { this->overlay_colour = colour; }
void FD_Box::setVisible(bool visible) { this->visible = visible; }

SDL_Colour FD_Box::getUnderlayColour() const { return underlay_colour; }
SDL_Colour FD_Box::getColour() const { return colour; }
SDL_Colour FD_Box::getOverlayColour() const { return overlay_colour; }
bool FD_Box::isVisible() const { return visible; }
void FD_Box::setBlendMode(SDL_BlendMode bm) {
	this->blend_mode = bm;
}

double FD_Box::getX() const { return x->value(); }
double FD_Box::getY() const { return y->value(); }
double FD_Box::getWidth() const { return w->value(); }
double FD_Box::getHeight() const { return h->value(); }
Uint8 FD_Box::getOpacity() const {
	return static_cast<int>(opacity->value());
}
SDL_BlendMode FD_Box::getBlendMode() const {
	return blend_mode;
}

void FD_Box::updateRect(double x, double y, double w, double h) {
	this->x->set(x);
	this->y->set(y);
	this->w->set(w);
	this->h->set(h);
}

FD_Tween* FD_Box::getTweenX() { return x; }
FD_Tween* FD_Box::getTweenY() { return y; }
FD_Tween* FD_Box::getTweenWidth() { return w; }
FD_Tween* FD_Box::getTweenHeight() { return h; }
FD_Tween* FD_Box::getTweenOpacity() { return opacity; }

void FD_Box::assimilate(std::shared_ptr<FD_ObjectGroup> group) {
	group->addObject(shared_from_this());
}
