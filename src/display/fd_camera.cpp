#include "fd_camera.hpp"

FD_Camera::FD_Camera(int w, int culling_radius) : FD_Resizable() {
	this->x = new FD_Tween(0);
	this->y = new FD_Tween(0);
	this->w = new FD_Tween(w);
	this->angle = new FD_Tween(0);
	this->scale = new FD_Tween(1);
	this->culling_radius = culling_radius;

	random_x = new FD_RandomGenerator();
	random_y = new FD_RandomGenerator();
	random_angle = new FD_RandomGenerator();

	culling_rect = new SDL_Rect();
	bounds = new SDL_Rect();
	bounding_rect = new SDL_Rect();
	bounds_center = new SDL_Point();
}
FD_Camera::~FD_Camera() {
	delete x;
	delete y;
	delete w;
	delete scale;
	delete angle;
	delete bounds;

	delete random_x;
	delete random_y;
	delete random_angle;

	delete culling_rect;
	delete bounding_rect;
	delete bounds_center;
}
void FD_Camera::associate(std::weak_ptr<FD_Window> window) {
	std::shared_ptr<FD_Window> win;
	FD_Handling::lock(window, win, true);
	win->addResizable(shared_from_this());
	resized(win->getWidth(), win->getHeight());
}

void FD_Camera::update(bool force) {
	// Update our bounds if needed
	bool xm = x->moved();
	bool ym = y->moved();
	if (force || xm || ym || w->moved() || scale->moved()) {
		bounds_center->x = static_cast<int>(x->value());
		bounds_center->y = static_cast<int>(y->value());
		bounds->w = static_cast<int>(w->value() * scale->value());
		bounds->h = static_cast<int>(w->value() * scale->value() * aspect_ratio);
		bounds->x = static_cast<int>(bounds_center->x - bounds->w / 2);
		bounds->y = static_cast<int>(bounds_center->y - bounds->h / 2);
		drawing_scale = resolution_width / (w->value() * scale->value());
		
		max_x_shake = bounds->w * 0.01;
		max_y_shake = bounds->h * 0.01;
	} else if (xm || ym) {
		bounds->x = static_cast<int>(x->value() - bounds->w / 2);
		bounds->y = static_cast<int>(y->value() - bounds->h / 2);
	}
	shake_decrement(shake_amount);
	if (shake_amount < 0.01) shake_amount = 0;
}
void FD_Camera::resized(int width, int height) {
	// Update the aspect ratio and camera scaling
	resolution_width = width;
	resolution_height = height;
	aspect_ratio = static_cast<double>(resolution_height) / resolution_width;
	// Update the camera
	FD_Camera::update(true);
}

void FD_Camera::pre_render() {
	bounding_rect->w = getRotatedWidth();
	bounding_rect->h = getRotatedHeight();
	bounding_rect->x = static_cast<int>(
		bounds_center->x - bounding_rect->w / 2.0);
	bounding_rect->y = static_cast<int>(
		bounds_center->y - bounding_rect->h / 2.0);
	culling_rect->x = bounding_rect->x - culling_radius;
	culling_rect->y = bounding_rect->y - culling_radius;
	culling_rect->w = bounding_rect->w + (culling_radius << 1);
	culling_rect->h = bounding_rect->h + (culling_radius << 1);
}
bool FD_Camera::manipulate(SDL_Rect& dstrect, double& angle) const {
	if (!SDL_HasIntersection(culling_rect, &dstrect)) {
		if (!SDL_RectEmpty(&dstrect)) {
			bool outside{ true };
			SDL_Point p{ dstrect.x, dstrect.y };
			if (SDL_PointInRect(&p, culling_rect)) outside = false;
			p.x = dstrect.x + dstrect.w;
			if (SDL_PointInRect(&p, culling_rect)) outside = false;
			p.y = dstrect.y + dstrect.h;
			if (SDL_PointInRect(&p, culling_rect)) outside = false;
			p.x = dstrect.x;
			if (SDL_PointInRect(&p, culling_rect)) outside = false;
			p.x = dstrect.x + dstrect.w / 2;
			p.y = dstrect.y + dstrect.h / 2;
			if (SDL_PointInRect(&p, culling_rect)) outside = false;
			if (outside) return false;
		} else {
			return false;
		}
	}
	// The camera values to use based on shake
	double cx, cy, ca;
	cx = bounds_center->x + max_x_shake * shake_amount * random_x->getNoise();
	cy = bounds_center->y + max_y_shake * shake_amount * random_y->getNoise();
	ca = this->angle->value() + max_angle_shake * shake_amount * random_angle->getNoise();
	// x and y relative to camera center
	double center_x = dstrect.x + dstrect.w / 2.0 - cx;
	double center_y = dstrect.y + dstrect.h / 2.0 - cy;
	// Create the dstrect with the correct width and height
	SDL_Rect dr = { 0, 0,
		static_cast<int>(dstrect.w * drawing_scale),
		static_cast<int>(dstrect.h * drawing_scale) };
	// Rotate the x and y around the camera center
	dr.x = static_cast<int>(center_x * cos(FD_PI * ca / 180.0)
		- center_y * sin(FD_PI * ca / 180.0));
	dr.y = static_cast<int>(center_x * sin(FD_PI * ca / 180.0)
		+ center_y * cos(FD_PI * ca / 180.0));
	// Make relative to the screen
	dr.x += bounds->w / 2 - dstrect.w / 2;
	dr.y += bounds->h / 2 - dstrect.h / 2;
	dr.x = static_cast<int>(dr.x * drawing_scale);
	dr.y = static_cast<int>(dr.y * drawing_scale);
	dstrect = dr;
	return true;
}

void FD_Camera::still() { shake_amount = 0; }
void FD_Camera::shake(double amount) { shake_amount += amount; }

void FD_Camera::toCameraCoordinates(int& x, int& y) const {
	x = static_cast<int>(x / drawing_scale);
	y = static_cast<int>(y / drawing_scale);
	x += bounds->x;
	y += bounds->y;
}
void FD_Camera::toScreenCoordinates(int& x, int& y) const {
	x -= bounds->x;
	y -= bounds->y;
	x = static_cast<int>(x * drawing_scale);
	y = static_cast<int>(y * drawing_scale);
}
void FD_Camera::toCameraCoordinates(double& x, double& y) const {
	x /= drawing_scale;
	y /= drawing_scale;
	x += bounds->x;
	y += bounds->y;
}
void FD_Camera::toScreenCoordinates(double& x, double& y) const {
	x -= bounds->x;
	y -= bounds->y;
	x *= drawing_scale;
	y *= drawing_scale;
}

int FD_Camera::getRotatedWidth() const {
	double a = angle->value();
	while (a < 0) { a += 180.0; }
	while (a > 180) { a -= 180.0; }
	if (a > 90.0) a = 180 - a;
	a *= FD_PI;
	a /= 180.0;
	return static_cast<int>(w->value() * scale->value() * cos(a)
		+ w->value() * scale->value() * aspect_ratio * sin(a));
}
int FD_Camera::getRotatedHeight() const {
	double a = angle->value();
	while (a < 0) { a += 180.0; }
	while (a > 180) { a -= 180.0; }
	if (a > 90.0) a = 180 - a;
	a *= FD_PI;
	a /= 180.0;
	return static_cast<int>(w->value() * scale->value() * sin(a)
		+ w->value() * scale->value() * aspect_ratio * cos(a));
}

int FD_Camera::getWidth() const { 
	return static_cast<int>(w->value() * scale->value()); 
}
int FD_Camera::getHeight() const { 
	return static_cast<int>(w->value() * scale->value() * aspect_ratio); 
}
int FD_Camera::getCullingRadius() const {
	return culling_radius;
}
const double FD_Camera::getMaxAngularShake() const {
	return max_angle_shake;
}
const std::function<double(double)> FD_Camera::getShakeDecrementer() const {
	return shake_decrement;
}

void FD_Camera::setWidth(double width) { 
	w->set(width); 
}
void FD_Camera::setHeight(double height) { 
	w->set(height / aspect_ratio); 
}
void FD_Camera::setCullingRadius(int radius) {
	this->culling_radius = radius;
}
void FD_Camera::setMaxAngularShake(double angle) {
	this->max_angle_shake = angle;
}
void FD_Camera::setShakeDecrementer(std::function<double(double)> f) {
	this->shake_decrement = f;
}

const SDL_Rect* FD_Camera::getBounds() const { return bounds; }
const SDL_Rect* FD_Camera::getCullingBounds() const {
	return culling_rect;
}

FD_Tween* FD_Camera::getTweenX() { return x; }
FD_Tween* FD_Camera::getTweenY() { return y; }
FD_Tween* FD_Camera::getTweenWidth() { return w; }
FD_Tween* FD_Camera::getTweenScale() { return scale; }
FD_Tween* FD_Camera::getTweenAngle() { return angle; }

bool FD_Camera::isMoving() {
	return !(x->finished())
		|| !(y->finished())
		|| !(w->finished())
		|| !(scale->finished())
		|| !(angle->finished());
}
