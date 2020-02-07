#include "fd_button.hpp"

// Button Member Functions

FD_Button::FD_Button(const FD_ButtonTemplate temp)
	: FD_Object(temp.z, temp.camera_bound) {
	this->scene = temp.scene;
}
FD_Button::~FD_Button() {}

void FD_Button::press() {
	if (!selected) return;
	if (auto sfx = sfx_press.lock()) sfx->play();
	newly_held = held = true;
}
void FD_Button::reset() {
	this->selected = false;
	this->held = this->newly_held = false;
	this->update(FD_ButtonActivity{});
}

void FD_Button::enter() {
	this->entered = true;
}
void FD_Button::buttonUp() {}
void FD_Button::buttonDown() {}
void FD_Button::exit() {
	this->entered = false;
}

int FD_Button::getCode() const {
	return return_code;
}
std::shared_ptr<FD_Image> FD_Button::getImage() const {
	if (image.expired()) {
		return nullptr;
	} else {
		return image.lock();
	}
}
bool FD_Button::isSelected() const {
	return selected;
}
bool FD_Button::isEntered() const {
	return entered;
}
FD_Tween* FD_Button::getTweenX() { return x; }
FD_Tween* FD_Button::getTweenY() { return y; }
FD_Tween* FD_Button::getTweenWidth() { return w; }
FD_Tween* FD_Button::getTweenHeight() { return h; }
FD_Tween* FD_Button::getTweenOpacity() { return opacity; }

// Basic Button Member Functions

FD_BasicButton::FD_BasicButton(const FD_ButtonTemplate& temp,
	const int x, const int y,
	const int code,
	const std::weak_ptr<FD_Image> overlay_weak) : FD_Button(temp) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	// Getting the textures
	std::shared_ptr<FD_Image> background, overlay;
	bool has_overlay{ false }, has_background{ false };
	if (overlay = overlay_weak.lock()) has_overlay = true;
	if (background = temp.background.lock()) has_background = true;
	// Get the largest width and height
	Uint32 width{ temp.width }, height{ temp.height };
	if (has_background) {
		width = background->getWidth();
		height = background->getHeight();
	}
	bool overlay_width{ false };
	bool overlay_height{ false };
	if (has_overlay) {
		if (overlay->getWidth() > width) {
			width = overlay->getWidth();
			overlay_width = true;
		}
		if (overlay->getHeight() > height) {
			height = overlay->getHeight();
			overlay_height = true;
		}
	}
	if (temp.stretch_bg) {
		if (overlay_width) width += temp.stretch_buffer_w;
		if (overlay_height) height += temp.stretch_buffer_h;
	}
	if (width == 0 || height == 0) FD_Handling::error("Button constructed with no area.");
	// Create the texture
	SDL_Texture* texture = SDL_CreateTexture(scene->getWindow()->getRenderer(),
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	// Set the texture as the target
	SDL_SetRenderTarget(scene->getWindow()->getRenderer(), texture);
	SDL_Rect dstrect = { 0, 0, 0, 0 };
	// Draw the button background
	if (has_background) {
		if (temp.stretch_bg) {
			SDL_RenderCopy(scene->getWindow()->getRenderer(),
				background->getTexture(), nullptr, nullptr);
		} else {
			dstrect.x = (width - background->getWidth()) / 2;
			dstrect.y = (height - background->getHeight()) / 2;
			dstrect.w = background->getWidth();
			dstrect.h = background->getHeight();
			SDL_RenderCopy(scene->getWindow()->getRenderer(),
				background->getTexture(), nullptr, &dstrect);
		}
	}
	// Draw the overlay
	if (has_overlay) {
		dstrect.x = (width - overlay->getWidth()) / 2;
		dstrect.y = (height - overlay->getHeight()) / 2;
		dstrect.w = overlay->getWidth();
		dstrect.h = overlay->getHeight();
		SDL_RenderCopy(scene->getWindow()->getRenderer(),
			overlay->getTexture(), nullptr, &dstrect);
	}
	// Reset the render target
	SDL_SetRenderTarget(scene->getWindow()->getRenderer(), nullptr);
	// Assign image
	this->pure_image = std::make_shared<FD_PureImage>(texture);
	this->image = pure_image;
	// Set values
	this->x->set(static_cast<double>(temp.origin_x) + x);
	this->y->set(static_cast<double>(temp.origin_y) + y);
	if (temp.width > 0) width = temp.width;
	if (temp.height > 0) height = temp.height;
	this->w->set(width);
	this->h->set(height);
	this->dstrect = new SDL_Rect();
	FD_Object::updateBounds(this->dstrect, this->x->value(),
		this->y->value(), w->value(), h->value(), FD_CENTERED);
	// Set the code
	this->return_code = code;
	// Get the sound
	sfx_hover = temp.sfx_hover;
	sfx_press = temp.sfx_press;
	sfx_release = temp.sfx_release;
}
FD_BasicButton::~FD_BasicButton() {}

bool FD_BasicButton::release() {
	if (held) {
		newly_held = true;
		held = false;
		if (auto sfx = sfx_release.lock()) sfx->play();
		return true;
	}
	return false;
}
void FD_BasicButton::reset() {
	if (auto i = image.lock()) {
		i->setOverlayColour({ 0, 0, 0, 0 });
	}
}
void FD_BasicButton::update(FD_ButtonActivity activity) {
	// Selection
	if (!activity.selected) activity.selected = mouseSelected(activity);
	if (activity.selected != this->selected || newly_held) {
		newly_held = false;
		if (auto i = image.lock()) {
			if (activity.selected) {
				if (held) {
					i->setOverlayColour({ 0, 0, 0, 120 });
				} else {
					i->setOverlayColour({ 255, 255, 255, 120 });
				}
			} else {
				i->setOverlayColour({ 0, 0, 0, 0 });
			}
		}
	}
	if (activity.selected != this->selected) {
		if (auto sfx = sfx_hover.lock()) sfx->play();
		this->selected = activity.selected;
		if (!selected) held = false;
	}
	// Bounds updating
	if (x->moved() || y->moved() || w->moved() || h->moved()) {
		FD_Object::updateBounds(dstrect, x->value(),
			y->value(), w->value(), h->value(), FD_CENTERED);
	}
}
bool FD_BasicButton::mouseSelected(FD_ButtonActivity a) const {
	if (!a.mouse) return false;
	if (this->isCameraBound()) {
		return FD_Maths::pointInRect(a.camera_x, a.camera_y, dstrect);
	} else {
		return FD_Maths::pointInRect(a.x, a.y, dstrect);
	}
}

void FD_BasicButton::assimilate(std::shared_ptr<FD_ObjectGroup> group) {
	group->addObject(shared_from_this());
}

// Dropdown Button Member Functions

FD_DropdownButton::FD_DropdownButton(const FD_ButtonTemplate& temp,
	const int x, const int y,
	const std::vector<int> codes,
	const std::vector<std::weak_ptr<FD_Image>> overlays_weak,
	const size_t preselection) : FD_Button(temp) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	// Get the background
	std::shared_ptr<FD_Image> background;
	if (background = temp.background.lock()) {
		has_background = true;
	}
	// Get all the overlays
	for (auto ow : overlays_weak) {
		if (auto o = ow.lock()) {
			auto e = std::make_shared<FD_Element>(
				o, temp.origin_x + x,
				temp.origin_y + y,
				0, temp.z + 3, 1, 1, temp.camera_bound);
			option_elements.push_back(e);
			option_images.push_back(o);
			e->getTweenOpacity()->set(0);
		}
	}
	// Get the largest width and height
	Uint32 width{ 0 }, height{ 0 };
	if (has_background) {
		width = background->getWidth();
		height = background->getHeight();
	}
	bool overlay_width{ false };
	bool overlay_height{ false };
	for (auto o : option_images) {
		if (o->getWidth() > width) {
			width = o->getWidth();
			overlay_width = true;
		}
		if (o->getHeight() > height) {
			height = o->getHeight();
			overlay_height = true;
		}
	}
	if (temp.stretch_bg) {
		if (overlay_width) width += temp.stretch_buffer_w;
		if (overlay_height) height += temp.stretch_buffer_h;
	}
	if (width == 0 || height == 0) FD_Handling::error("Button constructed with no area.", true);
	// Create the background
	if (has_background) {
		SDL_Renderer* ren = scene->getWindow()->getRenderer();
		SDL_Texture* texture;
		if (temp.stretch_bg) {
			texture = SDL_CreateTexture(ren,
				SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
				width, height);
		} else {
			texture = SDL_CreateTexture(ren,
				SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
				background->getWidth(), background->getHeight());
		}
		SDL_SetRenderTarget(ren, texture);
		SDL_RenderCopy(ren,
			background->getTexture(),
			nullptr, nullptr);
		SDL_SetRenderTarget(ren, nullptr);
		bg_cpy = std::make_shared<FD_PureImage>(texture);
		bg = std::make_shared<FD_Element>(bg_cpy,
			temp.origin_x + x,
			temp.origin_y + y,
			0, temp.z - 1, 1, 1, temp.camera_bound);
	}
	// Set dimensions and destination rectangle
	this->x->set(static_cast<double>(temp.origin_x) + x);
	this->y->set(static_cast<double>(temp.origin_y) + y);
	if (temp.width > 0) width = temp.width;
	if (temp.height > 0) height = temp.height;
	this->w->set(width);
	this->h->set(height);
	this->dstrect = new SDL_Rect();
	FD_Object::updateBounds(dstrect, this->x->value(),
		this->y->value(), w->value(), h->value(), FD_CENTERED);
	// Set some variables
	if (preselection < codes.size()) {
		this->option = preselection;
	}
	while (option_images.size() > codes.size()) {
		option_images.pop_back();
		option_elements.pop_back();
	}
	if (codes.size() != 0) {
		this->return_code = codes.at(this->option);
		if (preselection < option_images.size()) {
			this->image = option_images.at(preselection);
		}
	}
	this->codes = codes;
	// Create the backing rectangle
	SDL_Colour colour{ 0, 0, 0, 120 };
	backing = std::make_shared<FD_Box>(0, 0, 0, 0, temp.z + 1,
		temp.camera_bound);
	backing->setUnderlayColour(colour);
	backing->getTweenOpacity()->set(0);
	// Create the selection rectangle
	colour = { 255, 255, 255, 120 };
	selection = std::make_shared<FD_Box>(0, 0, 0, 0, temp.z + 2,
		temp.camera_bound);
	selection->setOverlayColour(colour);
	selection->getTweenOpacity()->set(0);
	// Get the sound
	sfx_hover = temp.sfx_hover;
	sfx_press = temp.sfx_press;
	sfx_release = temp.sfx_release;
}
FD_DropdownButton::~FD_DropdownButton() {}

bool FD_DropdownButton::release() {
	if (held) {
		newly_held = true;
		held = false;
		if (dropped) {
			if (auto sfx = sfx_release.lock()) sfx->play();
			if (drop_selection >= 0) {
				return_code = codes.at(drop_selection);
				image = option_images.at(drop_selection);
				updateElements();
				return true;
			}
		} else {
			dropped = true;
			drop_selection = -1;
			backing->getTweenOpacity()->move(FD_TWEEN_EASE_OUT, 255, 500);
			backing->getTweenWidth()->move(FD_TWEEN_EASE_OUT, w->value(), 500);
			backing->getTweenHeight()->move(FD_TWEEN_EASE_OUT, h->value() * codes.size(), 500);
			for (auto o : option_elements) {
				o->getTweenOpacity()->move(FD_TWEEN_EASE_OUT, 255, 500, 500);
			}
		}
	}
	return false;
}
void FD_DropdownButton::reset() {
	if (auto i = image.lock()) {
		i->setOverlayColour({ 0, 0, 0, 0 });
	}
}
void FD_DropdownButton::update(FD_ButtonActivity activity) {
	// Selection
	if (!activity.selected) activity.selected |= mouseSelected(activity);
	if (activity.selected) unhovered_ticks = 0;
	if (unhovered_ticks < unhovered_cap && dropped) {
		unhovered_ticks++;
		if (unhovered_ticks == unhovered_cap) {
			dropped = false;
			backing->getTweenOpacity()->move(FD_TWEEN_EASE_OUT, 0, 500, 500);
			backing->getTweenWidth()->move(FD_TWEEN_EASE_OUT, 0, 500, 500);
			backing->getTweenHeight()->move(FD_TWEEN_EASE_OUT, h->value(), 500, 500);
			for (auto o : option_elements) {
				o->getTweenOpacity()->move(FD_TWEEN_EASE_OUT, 0, 500);
			}
		}
	}
	if (activity.selected != this->selected || newly_held) {
		newly_held = false;
		if (has_background) {
			if (activity.selected) {
				if (held) {
					bg->setOverlayColour({ 0, 0, 0, 120 });
				} else {
					bg->setOverlayColour({ 255, 255, 255, 120 });
				}
			} else {
				bg->setOverlayColour({ 0, 0, 0, 0 });
			}
		}
	}
	if (activity.selected != this->selected) {
		if (auto sfx = sfx_hover.lock()) sfx->play();
		this->selected = activity.selected;
		if (!selected) held = false;
	}
	// Drop selection
	if (activity.mouse) configureDropSelection(dropMouseSelected(activity));
	// Bounds updating
	if (x->moved() || y->moved() || w->moved() || h->moved()
		|| backing->getTweenWidth()->moved()
		|| backing->getTweenHeight()->moved()
		|| selection->getTweenY()->moved()) {
		updateElements();
	}
	for (auto e : option_elements) e->update();
}

void FD_DropdownButton::updateElements() {
	// Background
	bg->getTweenX()->set(x->value());
	bg->getTweenY()->set(y->value());
	bg->setWidth(static_cast<int>(w->value()));
	bg->setHeight(static_cast<int>(h->value()));
	// Image
	if (image.expired()) {
		FD_Object::updateBounds(dstrect, x->value(),
			y->value(), w->value(), h->value(), FD_CENTERED);
	} else {
		auto i = image.lock();
		FD_Object::updateBounds(dstrect, x->value(),
			y->value(), i->getWidth(), i->getHeight(), FD_CENTERED);
	}
	// Backing
	const SDL_Rect* bgr = bg->getDestinationRect();
	int bx{ bgr->x + bgr->w };
	int by{ bgr->y };
	backing->getTweenX()->set(bx);
	backing->getTweenY()->set(by);
	double ox = bx + backing->getWidth() / 2.0;
	double oy = by + h->value() / 2.0;
	for (auto o : option_elements) {
		o->getTweenX()->set(ox);
		o->getTweenY()->set(oy);
		oy += h->value();
	}
	// Selection
	selection->getTweenX()->set(backing->getX());
	selection->getTweenWidth()->set(backing->getWidth());
	selection->getTweenHeight()->set(h->value());
}

void FD_DropdownButton::enter() {
	FD_Button::enter();
	configureDropSelection(0);
}

void FD_DropdownButton::buttonUp() {
	if (!dropped) exit();
	int dms = drop_selection;
	if (dms > 0) {
		dms--;
		configureDropSelection(dms);
	}
}
void FD_DropdownButton::buttonDown() {
	if (!dropped) exit();
	int dms = drop_selection + 1;
	if (dms < static_cast<int>(codes.size())) {
		configureDropSelection(dms);
	}
}
void FD_DropdownButton::exit() {
	FD_Button::exit();
	configureDropSelection(-1);
}

int FD_DropdownButton::dropMouseSelected(FD_ButtonActivity a) const {
	if (!dropped) return -1;
	// Getting the x and y
	int x = a.x, y = a.y;
	if (this->isCameraBound()) {
		x = a.camera_x;
		y = a.camera_y;
	}
	// Checking which option is selected
	SDL_Rect o_rect{
		static_cast<int>(backing->getX()),
		static_cast<int>(backing->getY()),
		static_cast<int>(backing->getWidth()),
		static_cast<int>(h->value()) };
	size_t index{ 0 };
	while (o_rect.y <= backing->getY() + backing->getHeight()) {
		if (index >= codes.size()) return -1;
		if (FD_Maths::pointInRect(x, y, &o_rect)) {
			return static_cast<int>(index);
		}
		index++;
		o_rect.y += o_rect.h;
	}
	return -1;
}
bool FD_DropdownButton::mouseSelected(FD_ButtonActivity a) const {
	if (!a.mouse) return false;
	bool on_button;
	if (this->isCameraBound()) {
		on_button = FD_Maths::pointInRect(a.camera_x, a.camera_y, bg->getDestinationRect());
	} else {
		on_button = FD_Maths::pointInRect(a.x, a.y, bg->getDestinationRect());
	}
	return on_button || (dropMouseSelected(a) > -1);
}

void FD_DropdownButton::configureDropSelection(int dms) {
	double so_dest{ selection->getTweenOpacity()->destination() };
	if (dms < 0 && so_dest != 0) {
		selection->getTweenOpacity()->move(FD_TWEEN_EASE_OUT, 0, 500);
	}
	if (dms >= 0 && so_dest != 255) {
		selection->getTweenOpacity()->move(FD_TWEEN_EASE_OUT, 255, 500);
	}
	if (dms != drop_selection) {
		if (dms >= 0) {
			selection->getTweenY()->move(FD_TWEEN_EASE_OUT,
				static_cast<double>(dms) * selection->getHeight()
				+ backing->getY(), 150);
		}
	}
	drop_selection = dms;
}

void FD_DropdownButton::assimilate(std::shared_ptr<FD_ObjectGroup> group) {
	group->addObject(FD_DropdownButton::shared_from_this());
	backing->assimilate(group);
	selection->assimilate(group);
	if (has_background) bg->assimilate(group);
	for (auto e : option_elements) e->assimilate(group);
}