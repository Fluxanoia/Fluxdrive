#include "fd_imageManager.hpp"

#include "../main/fd_handling.hpp"

// Font Member Functions

FD_Font::FD_Font(const std::weak_ptr<FD_Registry> registry,
	const FD_FontRegister reg, const int size) : reg{ reg }, size{ size } {
	std::string path;
	std::shared_ptr<FD_Registry> r;
	FD_Handling::lock(registry, r, true);
	if (r->get(reg, path)) {
		FD_Paths::ADD_BASE_PATH(path);
		font = TTF_OpenFont(path.c_str(), size);
		loaded = font != nullptr;
	}
}
FD_Font::~FD_Font() {
	if (font != nullptr) TTF_CloseFont(font);
}

bool FD_Font::verify(const std::shared_ptr<FD_Font> font) const {
	return (reg == font->getRegister()) && (size == font->getSize());
}
bool FD_Font::verify(const FD_FontRegister reg, const int size) const {
	return (this->reg == reg) && (this->size == size);
}

bool FD_Font::getRenderedDimensions(std::string s, int& w, int& h) {
	int width{ 0 }, height{ 0 };
	if (TTF_SizeText(font, s.c_str(), &width, &height)) {
		return false;
	}
	w = width;
	h = height;
	return true;
}

TTF_Font* FD_Font::getFont() { return font; }
FD_FontRegister FD_Font::getRegister() const { return reg; }
int FD_Font::getSize() const { return size; }
bool FD_Font::isLoaded() const { return loaded; }

// Image Member Functions

FD_Image::FD_Image(const ImageType type) : type{ type } { }
FD_Image::~FD_Image() {
	if (texture != nullptr) SDL_DestroyTexture(texture);
}

// Gets information about the image and checks if it's loaded
void FD_Image::query() {
	if (texture != nullptr) {
		int w, h;
		Uint32 f = static_cast<Uint32>(SDL_PIXELFORMAT_UNKNOWN);
		SDL_QueryTexture(texture,
			&f, nullptr, &w, &h);
		this->width = static_cast<Uint32>(w);
		this->height = static_cast<Uint32>(h);
		this->loaded = true;
	}
}

void FD_Image::render(SDL_Renderer* renderer, Uint8 alpha,
	const SDL_Rect* srcrect, const SDL_Rect* dstrect,
	double angle, double center_x, double center_y, SDL_RendererFlip flip,
	SDL_BlendMode blend, const SDL_Rect* clip) {
	// If there's no texture or it's completely transparent, stop
	if (!loaded || texture == nullptr || alpha == 0) return;
	// Prepare the destination
	SDL_Point temp_center;
	if (dstrect == nullptr) {
		int out_w{ 0 }, out_h{ 0 };
		SDL_GetRendererOutputSize(renderer, &out_w, &out_h);
		temp_center.x = static_cast<int>(center_x * out_w);
		temp_center.y = static_cast<int>(center_y * out_h);
	} else {
		temp_center.x = static_cast<int>(center_x * dstrect->w);
		temp_center.y = static_cast<int>(center_y * dstrect->h);
	}
	SDL_Rect temp_dstrect = (dstrect == nullptr) ? SDL_Rect() : SDL_Rect(*dstrect);
	// Extrude if needed
	if (extrusion > 0) temp_dstrect = FD_Maths::extrude(temp_dstrect, extrusion);
	// Set the renderer and the texture's blend modes
	SDL_BlendMode old_draw_blend{};
	SDL_BlendMode old_texture_blend{};
	SDL_GetRenderDrawBlendMode(renderer, &old_draw_blend);
	SDL_GetTextureBlendMode(texture, &old_texture_blend);
	SDL_SetRenderDrawBlendMode(renderer, blend);
	SDL_SetTextureBlendMode(texture, blend);
	// Set the clipping
	SDL_Rect* old_clip{ nullptr };
	bool clipping = (clip != nullptr);
	if (clipping) {
		SDL_RenderGetClipRect(renderer, old_clip);
		SDL_RenderSetClipRect(renderer, clip);
	}
	// If the underlay colour is not completely transparent, draw it
	if (underlay_colour.a != 0) {
		// Set the alpha in accordance to the overall alpha
		Uint8 underlay_alpha = static_cast<Uint8>(underlay_colour.a * (alpha / 255.0));
		SDL_SetRenderDrawColor(renderer, underlay_colour.r, underlay_colour.g,
			underlay_colour.b, underlay_alpha);
		// Fill in the appropriate dstrect
		if (dstrect == nullptr) {
			SDL_RenderFillRect(renderer, nullptr);
		} else {
			SDL_RenderFillRect(renderer, &temp_dstrect);
		}
	}
	// Draw the texture to the appropriate dstrect
	SDL_SetTextureAlphaMod(texture, alpha);
	if (dstrect == nullptr) {
		SDL_RenderCopyEx(renderer, texture, srcrect, nullptr, angle, &temp_center, flip);
	} else {
		SDL_RenderCopyEx(renderer, texture, srcrect, &temp_dstrect, angle, &temp_center, flip);
	}
	if (alpha != 255) SDL_SetTextureAlphaMod(texture, 255);
	// If the overlay colour is not completely transparent, draw it
	if (overlay_colour.a != 0) {
		// Set the alpha in accordance to the overall alpha
		Uint8 overlay_alpha = static_cast<Uint8>(overlay_colour.a * (alpha / 255.0));
		SDL_SetRenderDrawColor(renderer, overlay_colour.r, overlay_colour.g,
			overlay_colour.b, overlay_alpha);
		// Fill in the appropriate dstrect
		if (dstrect == nullptr) {
			SDL_RenderFillRect(renderer, nullptr);
		} else {
			SDL_RenderFillRect(renderer, &temp_dstrect);
		}
	}
	// Reset the renderer and the texture's blend modes
	if (clipping) SDL_RenderSetClipRect(renderer, old_clip);
	SDL_SetRenderDrawBlendMode(renderer, old_draw_blend);
	SDL_SetTextureBlendMode(texture, old_texture_blend);
}

bool FD_Image::verify(const FD_ImageRegister path) const { return false; }
bool FD_Image::verify(const std::shared_ptr<FD_Font> font,
	const std::string prefix,
	const std::string text,
	const std::string suffix,
	const SDL_Colour colour) const {
	return false;
}

void FD_Image::setToExtrude(int size) {
	this->extrusion = size;
}
void FD_Image::setOverlayColour(SDL_Colour colour) {
	this->overlay_colour.r = colour.r;
	this->overlay_colour.g = colour.g;
	this->overlay_colour.b = colour.b;
	this->overlay_colour.a = colour.a;
}
void FD_Image::setUnderlayColour(SDL_Colour colour) {
	this->underlay_colour.r = colour.r;
	this->underlay_colour.g = colour.g;
	this->underlay_colour.b = colour.b;
	this->underlay_colour.a = colour.a;
}

bool FD_Image::isLoaded() const { return loaded; }
Uint32 FD_Image::getWidth() const { return width; }
Uint32 FD_Image::getHeight() const { return height; }
SDL_Texture* FD_Image::getTexture() const { return texture; }

// File Image Member Functions

FD_FileImage::FD_FileImage(const std::weak_ptr<FD_Registry> registry,
	const FD_ImageRegister reg, SDL_Renderer* renderer)
	: FD_Image(IT_FILE), reg{ reg } {
	std::string path;
	std::shared_ptr<FD_Registry> r;
	FD_Handling::lock(registry, r, true);
	if (r->get(reg, path)) {
		FD_Paths::ADD_BASE_PATH(path);
		texture = IMG_LoadTexture(renderer, path.c_str());
	}
	query();
}
FD_FileImage::~FD_FileImage() { }

bool FD_FileImage::verify(const FD_ImageRegister reg) const {
	return this->reg == reg;
}

FD_ImageRegister FD_FileImage::getRegister() const { return reg; }

// Text Image Member Functions

FD_TextImage::FD_TextImage(SDL_Renderer* renderer,
	const std::shared_ptr<FD_Font> font,
	const std::string prefix,
	const std::string text,
	const std::string suffix,
	const SDL_Colour colour)
	: FD_Image(IT_TEXT), font{ font }, prefix{ prefix }, suffix{ suffix }, colour{ colour } {
	changeText(renderer, text);
}
FD_TextImage::~FD_TextImage() { }

void FD_TextImage::changeText(SDL_Renderer* renderer, std::string text) {
	this->text = text;
	if (texture != nullptr) {
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}
	// Load a surface
	SDL_Surface* surface = TTF_RenderText_Blended(font->getFont(), (prefix + text + suffix).c_str(), colour);
	// Convert the surface to a texture
	if (surface != nullptr) {
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		query();
		SDL_FreeSurface(surface);
	}
}

void FD_TextImage::setTextColour(SDL_Colour c) {
	this->colour = c;
}

// Pure Image Member Functions

FD_PureImage::FD_PureImage(SDL_Renderer* renderer,
	Uint32 width, Uint32 height, std::vector<FD_PureElement*> elements)
	: FD_Image(IT_PURE), FD_Resizable() {
	this->renderer = renderer;
	this->pure_width = width;
	this->pure_height = height;
	this->elements = elements;
	this->redraw();
	this->query();
}
FD_PureImage::~FD_PureImage() { }

void FD_PureImage::remove(FD_PureElement* e) {
	auto it{ elements.begin() };
	while (it != elements.end()) {
		if ((*it) == e) {
			it = elements.erase(it);
		} else {
			it++;
		}
	}
}

void FD_PureImage::add(FD_PureElement* e) {
	elements.push_back(e);
}

void FD_PureImage::clear() {
	elements.clear();
}

void FD_PureImage::redraw(Uint32 width, Uint32 height) {
	this->pure_width = width;
	this->pure_height = height;
	this->redraw();
}
void FD_PureImage::redraw() {
	if (texture != nullptr) SDL_DestroyTexture(texture);
	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		this->pure_width, this->pure_height);
	double cx, cy;
	std::shared_ptr<FD_Image> image;
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	SDL_RenderClear(renderer);
	for (FD_PureElement* e : elements) {
		FD_Handling::lock(e->image, image, true);
		if (e->center == nullptr) {
			cx = cy = 0.5;
		} else {
			cx = e->center->x;
			cy = e->center->y;
		}
		image->render(renderer, e->opacity, e->srcrect,
			e->dstrect, e->angle, cx, cy, e->flags,
			e->blend_mode, e->clip);
	}
	SDL_SetRenderTarget(renderer, nullptr);
	this->query();
}

void FD_PureImage::resized(int width, int height) {
	redraw();
}

// Image Manager Member Functions

FD_ImageManager::FD_ImageManager(SDL_Renderer* renderer) : FD_Registered(),
renderer{ renderer } { }
FD_ImageManager::~FD_ImageManager() {
	renderer = nullptr;
	text_images.clear();
	file_images.clear();
	fonts.clear();
	FD_Handling::debug("FD_ImageManager destroyed.");
}

std::weak_ptr<FD_FileImage> FD_ImageManager::loadImage(const FD_ImageRegister reg) {
	// Check if the associated image is already in memory
	for (std::shared_ptr<FD_FileImage> i : file_images) if (i->verify(reg)) return i;
	// Load the image, returning it if it has been loaded
	std::shared_ptr<FD_FileImage> image = std::make_shared<FD_FileImage>(registry, reg, renderer);
	if (image->isLoaded()) {
		file_images.push_back(image);
		return image;
	}
	// Handle the lack of loaded image
	FD_Handling::error("An image could not be loaded.", true);
	return std::weak_ptr<FD_FileImage>();
}
std::weak_ptr<FD_TextImage> FD_ImageManager::loadImage(const std::shared_ptr<FD_Font> font,
	const std::string text,
	const SDL_Colour colour) {
	return loadImage(font, "", text, "", colour);
}
std::weak_ptr<FD_TextImage> FD_ImageManager::loadImage(const std::shared_ptr<FD_Font> font,
	const std::string prefix,
	const std::string text,
	const std::string suffix,
	const SDL_Colour colour) {
	// Check if the associated image is already in memory
	for (std::shared_ptr<FD_TextImage> i : text_images) if (i->verify(font, prefix, text, suffix, colour)) return i;
	// Load the image, returning it if it has been loaded
	std::shared_ptr<FD_TextImage> image = std::make_shared<FD_TextImage>(renderer, font, prefix, text, suffix, colour);
	if (image->isLoaded()) {
		text_images.push_back(image);
		return image;
	}
	// Handle the lack of loaded image
	FD_Handling::error("An image could not be loaded, text:");
	FD_Handling::error(text.c_str());
	return std::weak_ptr<FD_TextImage>();
}

std::vector<std::weak_ptr<FD_FileImage>> FD_ImageManager::bulkLoadImage(
	const std::vector<FD_ImageRegister> regs) {
	std::vector<std::weak_ptr<FD_FileImage>> v{};
	for (FD_ImageRegister reg : regs) v.push_back(this->loadImage(reg));
	return v;
}
std::vector<std::weak_ptr<FD_TextImage>> FD_ImageManager::bulkLoadImage(
	const std::shared_ptr<FD_Font> font,
	const std::vector<std::string> texts,
	const SDL_Colour colour) {
	return bulkLoadImage(font,
		std::vector<std::string>(), texts,
		std::vector<std::string>(), colour);
}
std::vector<std::weak_ptr<FD_TextImage>> FD_ImageManager::bulkLoadImage(
	const std::shared_ptr<FD_Font> font,
	const std::vector<std::string> prefixes,
	const std::vector<std::string> texts,
	const std::vector<std::string> suffixes,
	const SDL_Colour colour) {
	std::string p, s;
	std::vector<std::weak_ptr<FD_TextImage>> v{};
	for (size_t i = 0; i < texts.size(); i++) {
		p = s = "";
		if (i < prefixes.size()) p = prefixes.at(i);
		if (i < suffixes.size()) s = suffixes.at(i);
		v.push_back(this->loadImage(font, p,
			texts.at(i), s, colour));
	}
	return v;
}

std::weak_ptr<FD_Font> FD_ImageManager::loadFont(const FD_FontRegister reg, const int size) {
	// Check if the associated font is already in memory
	for (std::shared_ptr<FD_Font> f : fonts) if (f->verify(reg, size)) return f;
	// Load the font, returning it if it has been loaded
	std::shared_ptr<FD_Font> font = std::make_shared<FD_Font>(registry,
		reg, size);
	if (font->isLoaded()) {
		fonts.push_back(font);
		return font;
	}
	// Handle the lack of a loaded font
	FD_Handling::error("A font could not be loaded.", true);
	return std::weak_ptr<FD_Font>();
}

bool FD_ImageManager::deleteImage(const FD_ImageRegister reg) {
	auto it = file_images.begin();
	while (it != file_images.end()) {
		if ((*it)->verify(reg)) {
			file_images.erase(it);
			return true;
		} else {
			it++;
		}
	}
	return false;
}
bool FD_ImageManager::deleteImage(const std::shared_ptr<FD_Font> font,
	const std::string text,
	const SDL_Colour colour) {
	return deleteImage(font, "", text, "", colour);
}
bool FD_ImageManager::deleteImage(const std::shared_ptr<FD_Font> font,
	const std::string prefix,
	const std::string text,
	const std::string suffix,
	const SDL_Colour colour) {
	auto it = text_images.begin();
	while (it != text_images.end()) {
		if ((*it)->verify(font, prefix, text, suffix, colour)) {
			text_images.erase(it);
			return true;
		} else {
			it++;
		}
	}
	return false;
}

bool FD_ImageManager::deleteFont(const FD_FontRegister reg, const int size) {
	auto it = fonts.begin();
	while (it != fonts.end()) {
		if ((*it)->verify(reg, size)) {
			fonts.erase(it);
			return true;
		} else {
			it++;
		}
	}
	return false;
}
