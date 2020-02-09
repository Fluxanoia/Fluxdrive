#ifndef FD_IMAGE_MANAGER_H_
#define FD_IMAGE_MANAGER_H_

#include <memory>
#include <vector>
#include <string>

#include <SDL_ttf.h>
#include <SDL_image.h>

#include "fd_paths.hpp"
#include "fd_registry.hpp"
#include "../display/fd_resizable.hpp"

/*!
	@file
	@brief The file containing the classes relating to image management.
*/

//! The data type of the font register value.
typedef int FD_FontRegister;
//! The data type of the image register value.
typedef int FD_ImageRegister;

//! The FD_Font class, manages a font.
class FD_Font {
private:

	FD_FontRegister reg;
	const int size{ 0 };
	TTF_Font* font{ nullptr };
	bool loaded{ false };

public:

	//! Constructs a FD_Font.
	/*!
		\param registry The registry of to use.
		\param reg      The register of the path.
		\param size     The font size.
	*/
	FD_Font(const std::weak_ptr<FD_Registry> registry,
		const FD_FontRegister reg, const int size);
	//! Destroys the FD_Font.
	~FD_Font();

	//! Checks whether two fonts are identical.
	/*!
		\param font The font to check against.

		\return Whether the two fonts are identical.
	*/
	bool verify(const std::shared_ptr<FD_Font> font) const;
	//! Checks whether two fonts are identical.
	/*!
		\param reg  The register of the font to check against.
		\param size The size of the font to check against.

		\return Whether the two fonts are identical.
	*/
	bool verify(const FD_FontRegister reg, const int size) const;

	//! Returns the size of the font.
	/*!
		\return The size of the font.
	*/
	int getSize() const;
	//! Returns the raw font.
	/*!
		\return The raw font.
	*/
	TTF_Font* getFont();
	//! Returns whether the font is loaded or not.
	/*
		\return Whether the font is loaded or not.
	*/
	bool isLoaded() const;
	//! Returns the register used by the font.
	/*!
		\return The register used by the font.
	*/
	FD_FontRegister getRegister() const;

};

//! The FD_Image class, manages all types of visual information.
class FD_Image {
protected:

	//! The different image types.
	enum ImageType {
		//! Corresponds to an invalid image.
		IT_INVALID = -1,
		//! Corresponds to FD_TextImage.
		IT_TEXT,
		//! Corresponds to FD_FileImage.
		IT_FILE,
		//! Corresponds to FD_PureImage.
		IT_PURE
	};

	//! The type of the image.
	const ImageType type;
	//! Whether the image is loaded or not.
	bool loaded{ false };
	//! The extrusion of the image.
	/*!
		The extrusion allows images to be drawn slightly larger than
		intended. This is useful if you're trying to draw a grid and there are
		small gaps between the cells.
	*/
	int extrusion{ 0 };
	//! The width of the image.
	Uint32 width{ 0 };
	//! The height of the image.
	Uint32 height{ 0 };
	//! The texture of the image.
	SDL_Texture* texture{ nullptr };
	//! The underlay colour of the image.
	/*!
		This is drawn before the overlay colour and image are drawn.
	*/
	SDL_Colour underlay_colour{ 0, 0, 0, 0 };
	//! The overlay colour of the image.
	/*!
		This is drawn after the underlay colour and image are drawn.
	*/
	SDL_Colour overlay_colour{ 0, 0, 0, 0 };

	//! Updates the dimensions and loaded status of the image.
	void query();
	//! Extrudes a rectangle.
	/*!
		Increases the width and height by double the given extrusion value,
		centered about the centre of the rectangle.

		\param rect The rectangle to extrude.
		\param size The amount to extrude.
		
		\return The extruded rectangle.

		\sa extrusion
	*/
	SDL_Rect extrude(const SDL_Rect rect, const int size);

public:

	//! Constructs a FD_Image.
	FD_Image(const ImageType type);
	//! Destroys the FD_Image.
	~FD_Image();

	//! Renders the image.
	/*!
		\param renderer The renderer to use.
		\param alpha    The alpha of the image.
		\param srcrect  The source rectangle of the image.
		\param dstrect  The destination rectangle of the image.
		\param angle    The angle of the image.
		\param center_x The x center of the image relative to its top left corner, in units of the image's width.
		\param center_y The y center of the image relative to its top left corner, in units of the image's height.
		\param flip     The flip flags of the image.
	*/
	virtual void render(SDL_Renderer* renderer,
		const Uint8 alpha = 255,
		const SDL_Rect * srcrect = nullptr,
		const SDL_Rect * dstrect = nullptr,
		const double angle = 0.0,
		const double center_x = 0.5,
		const double center_y = 0.5,
		const SDL_RendererFlip flip = SDL_FLIP_NONE);

	//! Checks whether two images are identical using a register.
	/*!
		\param reg The register to check against.
	
		\return Whether the register corresponds to this image.
	*/
	virtual bool verify(const FD_ImageRegister reg) const;
	//! Checks whether two images are identical using a font, text and, colour.
	/*!
		\param font   The register to check against.
		\param prefix The prefix to check against.
		\param text   The text to check against.
		\param suffix The suffix to check against.
		\param colour The colour to check against.

		\return Whether the register corresponds to this image.
	*/
	virtual bool verify(const std::shared_ptr<FD_Font> font, 
		const std::string prefix,
		const std::string text,
		const std::string suffix,
		const SDL_Colour colour = { 255, 255, 255, 255 }) const;

	//! Returns whether the image is loaded or not.
	/*!
		\return Whether the image is loaded or not.
	*/
	bool isLoaded() const;
	//! Returns the image width.
	/*!
		\return The image width.
	*/
	Uint32 getWidth() const;
	//! Returns the image height.
	/*!
		\return The image height.
	*/
	Uint32 getHeight() const;
	//! Returns the raw texture.
	/*!
		\return The raw texture.
	*/
	SDL_Texture* getTexture() const;

	//! Sets the extrusion of the image.
	/*!
		\param size The new extrusion value.

		\sa extrusion
	*/
	void setToExtrude(int size);
	//! Sets the overlay colour.
	/*!
		\param colour The new overlay colour.
	*/
	void setOverlayColour(SDL_Colour colour);
	//! Sets the underlay colour.
	/*!
		\param colour The new underlay colour.
	*/
	void setUnderlayColour(SDL_Colour colour);

};

//! The FD_FileImage class, specialising the image to work with files.
class FD_FileImage : public FD_Image {
private:

	FD_ImageRegister reg;

public:

	//! Constructs a FD_FileImage.
	/*!
		\param registry The registry of to use.
		\param reg      The register of the path.
		\param renderer The renderer to use.
	*/
	FD_FileImage(const std::weak_ptr<FD_Registry> registry,
		 const FD_ImageRegister reg, SDL_Renderer* renderer);
	//! Destroys the FD_FileImage.
	~FD_FileImage();

	//! Checks whether two images are identical using a register.
	/*!
		\param reg The register to check against.

		\return Whether the register corresponds to this image.
	*/
	bool verify(const FD_ImageRegister reg) const override;

	//! Returns the register of this image.
	/*!
		\return The register of this image.
	*/
	FD_ImageRegister getRegister() const;

};

//! The FD_TextImage class, specialises the image to work with text.
class FD_TextImage : public FD_Image {
private:

	const std::string prefix;
	const std::string suffix;
	std::string text;
	const SDL_Colour colour;
	const std::shared_ptr<FD_Font> font;

public:

	//! Constructs a FD_TextImage.
	/*!
		The prefix and suffix cannot be changed once this class is instantiated
		but the text can. This can be useful for score displays, etc.

		\param renderer The renderer to use.
		\param font     The font to use.
		\param prefix   The prefix to the text.
		\param text     The text.
		\param suffix   The suffix to the text.
		\param colour   The colour of the text.
	*/
	FD_TextImage(SDL_Renderer* renderer,
		const std::shared_ptr<FD_Font> font,
		const std::string prefix,
		const std::string text,
		const std::string suffix,
		const SDL_Colour colour = { 255,255,255,255 });
	//! Destroys the FD_TextImage.
	~FD_TextImage();

	//! Changes the text of the image.
	/*!
		\param renderer The renderer to use.
		\param text     The new text.
	*/
	void changeText(SDL_Renderer* renderer, std::string text);

};

//! This allows the FD_PureImage to re-draw itself by storing the components seperately.
typedef struct FD_PureElement_ {
	//! The image used by the element.
	std::weak_ptr<FD_Image> image;
	//! The opacity of the image.
	Uint8 opacity{ 255 };
	//! The source rectangle of the element.
	SDL_Rect* srcrect{ nullptr };
	//! The destination rectangle of the element.
	SDL_Rect* dstrect{ nullptr };
	//! The angle of the element.
	double angle{ 0 };
	//! The center of the element, relative to the top left.
	SDL_Point* center{ nullptr };
	//! The flipping flags of the element.
	SDL_RendererFlip flags;
	//! The blend mode.
	SDL_BlendMode blend_mode{ SDL_BLENDMODE_BLEND };
} FD_PureElement;

//! The FD_PureImage class, specialises the image for custom renderered texture.
/*!
	If the texture given to this class has the SDL_TEXTUREACCESS_RENDER flag,
	add this class to the appropriate FD_Window's resizable list.
*/
class FD_PureImage : public FD_Image, public FD_Resizable {
private:

	SDL_Renderer* renderer;
	Uint32 pure_width, pure_height;
	std::vector<FD_PureElement> elements;

	void redraw();

public:

	//! Constructs a FD_PureImage.
	/*!
		You should add this image to the resizable list unless you aren't
		planning on resizing the window.

		\param renderer The renderer to use to construct the texture.
		\param width    The width of the made texture.
		\param height   The height of the made texture.
		\param elements The elements to render.

		\sa FD_PureElement
	*/
	FD_PureImage(SDL_Renderer* renderer, Uint32 width, Uint32 height, 
		std::vector<FD_PureElement> elements);
	//! Destroys the FD_PureImage.
	~FD_PureImage();

	//! When the window is resized, the video device is lost - this circumvents that issue.
	/*!
		\param width  The new width of the window.
		\param height The new height of the window.
	*/
	void resized(int width, int height);

};

//! The FD_ImageManager class, can manage all the visual resources for the program.
class FD_ImageManager : public FD_Registered {
private:

	SDL_Renderer* renderer;
	std::vector<std::shared_ptr<FD_FileImage>> file_images{  };
	std::vector<std::shared_ptr<FD_TextImage>> text_images{  };
	std::vector<std::shared_ptr<FD_Font>> fonts{  };

public:
	
	//! Constructs the FD_ImageManager.
	/*!
		\param renderer The renderer to use.
	*/
	FD_ImageManager(SDL_Renderer* renderer);
	//! Destroys the FD_ImageManager.
	~FD_ImageManager();
	
	//! Loads a file image using a register.
	/*!
		\param reg The register to use.

		\return The loaded image.
	*/
	std::weak_ptr<FD_FileImage> loadImage(const FD_ImageRegister reg);
	//! Loads a text image using a font, text and, colour.
	/*!
		\param font   The font to use.
		\param text   The text to use.
		\param colour The colour to use.

		\return The loaded image.
	*/
	std::weak_ptr<FD_TextImage> loadImage(const std::shared_ptr<FD_Font> font,
		const std::string text,
		const SDL_Colour colour = { 255,255,255,255 });
	//! Loads a text image with a prefix and suffix.
	/*!
		\param font   The font to use.
		\param prefix The prefix to use.
		\param text   The text to use.
		\param suffix The suffix to use.
		\param colour The colour to use.

		\return The loaded image.
	*/
	std::weak_ptr<FD_TextImage> loadImage(const std::shared_ptr<FD_Font> font,
		const std::string prefix, 
		const std::string text, 
		const std::string suffix, 
		const SDL_Colour colour = { 255,255,255,255 });

	//! Loads file images in bulk.
	/*!
		\param regs The file registers to use.

		\return The loaded images.
	*/
	std::vector<std::weak_ptr<FD_FileImage>> bulkLoadImage(
		const std::vector<FD_ImageRegister> regs);
	//! Loads text images in bulk.
	/*!
		\param font   The font to use.
		\param texts  The text to use.
		\param colour The colour to use.

		\return The loaded images.
	*/
	std::vector<std::weak_ptr<FD_TextImage>> bulkLoadImage(
		const std::shared_ptr<FD_Font> font,
		const std::vector<std::string> texts,
		const SDL_Colour colour = { 255,255,255,255 });
	//! Loads text images with prefixes and suffixes in bulk.
	/*!
		\param font     The font to use.
		\param prefixes The prefixes to use.
		\param texts    The text to use.
		\param suffixes The suffixes to use.
		\param colour   The colour to use.

		\return The loaded images.
	*/
	std::vector<std::weak_ptr<FD_TextImage>> bulkLoadImage(
		const std::shared_ptr<FD_Font> font,
		const std::vector<std::string> prefixes, 
		const std::vector<std::string> texts, 
		const std::vector<std::string> suffixes, 
		const SDL_Colour colour = { 255,255,255,255 });

	//! Loads a font.
	/*!
		\param reg  The register to use.
		\param size The size of the font.

		\return The loaded font.
	*/
	std::weak_ptr<FD_Font> loadFont(const FD_FontRegister reg, const int size);

	//! Deletes an image from the manager.
	/*!
		\param reg The register of the image to delete.

		\return Whether an image was deleted.
	*/
	bool deleteImage(const FD_ImageRegister reg);
	//! Deletes an image from the manager.
	/*!
		\param font   The font of the image to delete.
		\param text   The text of the image to delete.
		\param colour The colour of the image to delete.

		\return Whether an image was deleted.
	*/
	bool deleteImage(const std::shared_ptr<FD_Font> font,
		const std::string text,
		const SDL_Colour colour = { 255,255,255,255 });
	//! Deletes an image from the manager.
	/*!
		\param font   The font of the image to delete.
		\param prefix The prefix of the image to delete.
		\param text   The text of the image to delete.
		\param suffix The suffix of the image to delete.
		\param colour The colour of the image to delete.

		\return Whether an image was deleted.
	*/
	bool deleteImage(const std::shared_ptr<FD_Font> font,
		const std::string prefix, 
		const std::string text, 
		const std::string suffix, 
		const SDL_Colour colour = { 255,255,255,255 });
	//! Deletes a font from the manager.
	/*!
		\param reg  The register of the font to delete.
		\param size The size of the font to delete.
	*/
	bool deleteFont(const FD_FontRegister reg, const int size);

};

#endif