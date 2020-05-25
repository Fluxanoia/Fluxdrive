#include "fd_textBox.hpp"

FD_TextBox::FD_TextBox(std::weak_ptr<FD_Scene> s,
	const FD_TextTemplate& type_temp, int x, int y, int z,
	bool camera_bound, FD_DrawStyle style)
	: FD_Object(z + 1, camera_bound, x, y, type_temp.box_width,
		type_temp.box_height, 0, 1, 1, 255, true), type_temp{ type_temp } {
	this->scene = s;
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(s, scene, true);
	// Set values
	this->draw_style = style;
	this->line_offset = new FD_Tween(0);
	this->caret_timer = new FD_Timer();
	// Create the caret
	this->caret = std::make_shared<FD_Box>(0, 0, 0, 0,
		z + 1, camera_bound, type_temp.font_colour);
	this->caret_rect = new SDL_Rect();
	this->caret_rect->x = x;
	this->caret_rect->y = y;
	this->caret_rect->w = 2;
	this->caret_rect->h = type_temp.font->getSize();
	this->caret->supplyRect(this->caret_rect);
	this->caret->setVisible(false);
	// Set values
	this->srcrect = new SDL_Rect();
	FD_Object::updateBounds(this->srcrect, 0, 0, this->w->value(),
		this->h->value(), FD_TOP_LEFT);
	this->dstrect = new SDL_Rect();
	FD_Object::updateBounds(this->dstrect, this->x->value(),
		this->y->value(), this->w->value(), this->h->value(), draw_style);
	// Create the pure image
	this->pure_image = std::make_shared<FD_PureImage>(
		scene->getWindow()->getRenderer(),
		static_cast<Uint32>(this->getWidth()),
		static_cast<Uint32>(this->getHeight()), pure_elements);
	scene->getWindow()->addResizable(pure_image);
	this->changeText("");
}
FD_TextBox::~FD_TextBox() {
	this->clearPureElements();
	delete caret_rect;
	delete line_offset;
	delete caret_timer;
}

void FD_TextBox::clearPureElements() {
	for (auto p : pure_elements) {
		if (p->dstrect != nullptr) delete p->dstrect;
		if (p->srcrect != nullptr) delete p->srcrect;
		delete p;
	}
}

void FD_TextBox::wake() {
	this->update();
}

void FD_TextBox::update() {
	// Caret
	if (editing && FD_Maths::hasIntersection(dstrect, caret_rect)) {
		if (caret_timer->finished()) {
			caret->setVisible(!caret->isVisible());
			caret_timer->start(type_temp.caret_blink_delay);
		}
	} else {
		caret->setVisible(false);
	}
	// Offset updating
	if (line_offset->moved()) {
		if (type_temp.horz_scroll) {
			this->srcrect->x = static_cast<int>(-line_offset->value());
			this->srcrect->y = 0;
			caret_rect->x = caret_x + dstrect->x + static_cast<int>(line_offset->value());
			caret_rect->y = caret_y + dstrect->y;
		} else {
			this->srcrect->x = 0;
			this->srcrect->y = static_cast<int>(-line_offset->value());
			caret_rect->x = caret_x + dstrect->x;
			caret_rect->y = caret_y + dstrect->y + static_cast<int>(line_offset->value());
		}
		this->updateBoxes();
	}
	// Bounds updating
	if (x->moved() || y->moved() || w->moved() || h->moved()) {
		FD_Object::updateBounds(dstrect);
	}
}

void FD_TextBox::changeText(const std::string text) {
	FD_TextInfo info{ FD_TextInfo{ text } };
	this->changeText(info);
}
void FD_TextBox::changeText(const FD_TextInfo info) {
	text_info = info;
	caret->setVisible(true);
	caret_timer->start(type_temp.caret_blink_delay);
	if (type_temp.horz_scroll) {
		this->updateHorizontalImage();
	} else {
		this->updateVerticalImage();
	}
}

void FD_TextBox::prepareRender() {
	// Prepare the images
	std::shared_ptr<FD_Scene> s;
	FD_Handling::lock(scene, s);
	while (text_images.size() < lines.size()) {
		text_images.push_back(std::make_shared<FD_TextImage>(
			s->getWindow()->getRenderer(),
			type_temp.font, "", "", "", type_temp.font_colour
		));
	}
	// Prepare the selection boxes
	for (auto b : boxes) b->setVisible(false);
	while (boxes.size() < lines.size()) {
		boxes.push_back(std::make_shared<FD_Box>(0, 0, 0, 0,
			this->getLayer(), this->isCameraBound(), type_temp.selection_colour));
		boxes.back()->setUnderlayColour(type_temp.selection_colour);
		boxes.back()->setVisible(false);
		if (auto g{ object_group.lock() }) {
			boxes.back()->assimilate(g);
		} else {
			FD_Handling::error("Could not add selection box to FD_ObjectGroup in FD_TextBox.");
		}
	}
}
void FD_TextBox::updateHorizontalImage() {
	// Text variables
	size_t index{ 0 };
	bool selecting{ false };
	std::string text_stack{ };
	std::string text{ text_info.text };
	lines.clear();
	// Positioning variables
	caret_x = caret_y = 0;
	int w{ 0 }, h{ 0 };
	Uint32 x_buffer{ 0 };
	if (index == text_info.selection_start) selecting = true;
	if (index == text_info.selection_end) selecting = false;
	// Iterating over the text until we finish a line
	while (text.size() > 0) {
		// Get the dimensions of the current stack plus the next character
		type_temp.font->getRenderedDimensions(text_stack + text.front(), w, h);
		// Add to the stack
		text_stack.push_back(text.front());
		text.erase(text.begin());
		// Increment our index to the next char
		index++;
		if (index == text_info.selection_start) {
			// If the selection is starting on the next char,
			// finish this section
			lines.push_back(LineSection{ text_stack, x_buffer, 0, w, h, selecting, index });
			x_buffer += w;
			text_stack.clear();
			selecting = true;
		}
		if (index == text_info.selection_end) {
			// If the selection ends on the next char, finish the section
			// if we have selected a non-zero amount of chars
			if (text_stack.size() > 0) {
				lines.push_back(LineSection{ text_stack, x_buffer, 0, w, h, selecting, index });
				x_buffer += w;
				text_stack.clear();
			}
			selecting = false;
		}
	}
	// If there are characters left on the stack, add the final line
	if (text_stack.size() > 0) {
		type_temp.font->getRenderedDimensions(text_stack, w, h);
		lines.push_back(LineSection{ text_stack, x_buffer, 0, w, h, selecting, text_info.text.size() });
	}
	// Clear the image
	this->clearPureElements();
	pure_elements.clear();
	pure_image->clear();
	this->prepareRender();
	// Create the elements
	size_t next_image{ 0 };
	Uint32 pure_w{ 0 };
	std::shared_ptr<FD_Box> box;
	std::shared_ptr<FD_TextImage> image;
	std::shared_ptr<FD_Scene> s;
	FD_Handling::lock(scene, s);
	for (auto l : lines) {
		// Get the image
		image = text_images.at(next_image);
		box = boxes.at(next_image);
		// Colour the image
		if (l.sel) {
			image->setTextColour(type_temp.selection_text_colour);
			box->setVisible(true);
		} else {
			image->setTextColour(type_temp.font_colour);
			box->setVisible(false);
		}
		// Set the text
		image->changeText(s->getWindow()->getRenderer(), l.text);
		// Add the element
		FD_PureElement* pd{ new FD_PureElement() };
		pd->image = image;
		pd->dstrect = new SDL_Rect();
		pd->dstrect->x = l.x;
		pd->dstrect->y = l.y;
		pd->dstrect->w = image->getWidth();
		pd->dstrect->h = image->getHeight();
		if (pd->dstrect->x + pd->dstrect->w > static_cast<int>(pure_w)) {
			pure_w = pd->dstrect->x + pd->dstrect->w;
		}
		pure_elements.push_back(pd);
		next_image++;
	}
	// Add the elements
	for (auto p : pure_elements) pure_image->add(p);
	// Redraw the pure image
	if (pure_w < pure_image->getWidth()) pure_w = pure_image->getWidth();
	pure_image->redraw(pure_w, type_temp.box_height);
	// Set the dstrect height
	if (pure_w < type_temp.box_width) {
		this->dstrect->w = pure_w;
	} else {
		this->dstrect->w = type_temp.box_width;
	}
	// Update the caret
	this->updateCaret();
	// Update the boxes
	this->updateBoxes();
}
void FD_TextBox::updateVerticalImage() {
	// Text variables
	size_t index{ 0 };
	bool selecting{ false };
	std::string text_stack{ };
	std::string text{ text_info.text };
	lines.clear();
	// Positioning variables
	caret_x = caret_y = 0;
	int w{ 0 }, h{ 0 };
	Uint32 x_buffer{ 0 }, y_buffer{ 0 };
	if (index == text_info.selection_start) selecting = true;
	if (index == text_info.selection_end) selecting = false;
	// Iterating over the text until we finish a line
	while (text.size() > 0) {
		// Get the dimensions of the current stack plus the next character
		type_temp.font->getRenderedDimensions(text_stack + text.front(), w, h);
		if (x_buffer + w > type_temp.box_width) {
			// If we overflow our line, add a new line and reposition
			lines.push_back(LineSection{ text_stack, x_buffer, y_buffer, w, h, selecting, index });
			x_buffer = 0;
			y_buffer += h + type_temp.line_spacing;
			text_stack.clear();
		} else {
			// If we don't overflow the line, add to the stack
			text_stack.push_back(text.front());
			text.erase(text.begin());
			// Increment our index to the next char
			index++;
			if (index == text_info.selection_start) {
				// If the selection is starting on the next char,
				// finish this section
				lines.push_back(LineSection{ text_stack, x_buffer, y_buffer, w, h, selecting, index });
				x_buffer += w;
				text_stack.clear();
				selecting = true;
			}
			if (index == text_info.selection_end) {
				// If the selection ends on the next char, finish the section
				// if we have selected a non-zero amount of chars
				if (text_stack.size() > 0) {
					lines.push_back(LineSection{ text_stack, x_buffer, y_buffer, w, h, selecting, index });
					x_buffer += w;
					text_stack.clear();
				}
				selecting = false;
			}
		}
	}
	// If there are characters left on the stack, add the final line
	if (text_stack.size() > 0) {
		type_temp.font->getRenderedDimensions(text_stack, w, h);
		lines.push_back(LineSection{ text_stack, x_buffer, y_buffer, w, h, selecting, text_info.text.size() });
	}
	// Clear the image
	this->clearPureElements();
	pure_elements.clear();
	pure_image->clear();
	this->prepareRender();
	// Create the elements
	size_t next_image{ 0 };
	Uint32 pure_h{ 0 };
	std::shared_ptr<FD_Box> box;
	std::shared_ptr<FD_TextImage> image;
	std::shared_ptr<FD_Scene> s;
	FD_Handling::lock(scene, s);
	for (auto l : lines) {
		// Get the image
		image = text_images.at(next_image);
		box = boxes.at(next_image);
		// Colour the image
		if (l.sel) {
			image->setTextColour(type_temp.selection_text_colour);
		} else {
			image->setTextColour(type_temp.font_colour);
		}
		// Set the text
		image->changeText(s->getWindow()->getRenderer(), l.text);
		// Add the element
		FD_PureElement* pd{ new FD_PureElement() };
		pd->image = image;
		pd->dstrect = new SDL_Rect();
		pd->dstrect->x = l.x;
		pd->dstrect->y = l.y;
		pd->dstrect->w = image->getWidth();
		pd->dstrect->h = image->getHeight();
		if (pd->dstrect->y + pd->dstrect->h > static_cast<int>(pure_h)) {
			pure_h = pd->dstrect->y + pd->dstrect->h;
		}
		pure_elements.push_back(pd);
		next_image++;
	}
	// Add the elements
	for (auto p : pure_elements) pure_image->add(p);
	// Redraw the pure image
	if (pure_h < pure_image->getHeight()) pure_h = pure_image->getHeight();
	pure_image->redraw(type_temp.box_width, pure_h);
	// Set the dstrect height
	if (pure_h < type_temp.box_height) {
		this->dstrect->h = pure_h;
	} else {
		this->dstrect->h = type_temp.box_height;
	}
	// Update the caret
	this->updateCaret();
	// Update the boxes
	this->updateBoxes();
}
void FD_TextBox::updateBoxes() {
	size_t index{ 0 };
	SDL_Rect tr{};
	LineSection l;
	std::shared_ptr<FD_Box> b;
	while (index < lines.size()) {
		l = lines.at(index);
		b = boxes.at(index);
		if (!l.sel) {
			b->setVisible(false);
			index++;
			continue;
		}
		tr.w = l.w;
		tr.h = l.h;
		if (type_temp.horz_scroll) {
			tr.x = l.x + dstrect->x + static_cast<int>(line_offset->value());
			tr.y = l.y + dstrect->y;
		} else {
			tr.x = l.x + dstrect->x;
			tr.y = l.y + dstrect->y + static_cast<int>(line_offset->value());
		}
		if (!FD_Maths::hasIntersection(&tr, dstrect)) {
			b->setVisible(false);
			index++;
			continue;
		}
		if (tr.x < dstrect->x) {
			tr.w -= dstrect->x - tr.x;
			tr.x = dstrect->x;
		}
		if (tr.x + tr.w > dstrect->x + dstrect->w) {
			tr.w = dstrect->x + dstrect->w - tr.x;
		}
		if (tr.y < dstrect->y) {
			tr.h -= dstrect->y - tr.y;
			tr.y = dstrect->y;
		}
		if (tr.y + tr.h > dstrect->y + dstrect->h) {
			tr.h = dstrect->y + dstrect->h - tr.y;
		}
		b->setVisible(true);
		b->updateRect(tr.x, tr.y, tr.w, tr.h);
		index++;
	}
}

void FD_TextBox::updateCaret() {
	this->updateCaret(text_info.caret_pos);
}
void FD_TextBox::updateCaret(size_t pos) {
	caret->setVisible(true);
	caret_timer->start(type_temp.caret_blink_delay);
	// Set the new position
	text_info.caret_pos = pos;
	// Manage scroll
	bool found{ false };
	size_t lower_bound{ 0 };
	LineSection capture{ "", 0, 0, 0, 0, false, 1 };
	for (auto l : lines) {
		if (l.index_bound >= text_info.caret_pos) {
			found = true;
			capture = l;
			break;
		}
		lower_bound = l.index_bound;
	}
	int w, h;
	if (found) {
		std::string pre{ capture.text.substr(0, text_info.caret_pos - lower_bound) };
		type_temp.font->getRenderedDimensions(pre, w, h);
	} else {
		w = h = 0;
	}
	caret_x = capture.x + w;
	caret_y = capture.y;
	if (type_temp.horz_scroll) {
		if (caret_x + line_offset->destination() < 0) {
			line_offset->move(FD_TWEEN_EASE_OUT, -caret_x, 200);
		} else if (caret_x + line_offset->destination() > type_temp.box_width) {
			line_offset->move(FD_TWEEN_EASE_OUT,
				static_cast<double>(type_temp.box_width) - caret_x, 200);
		}
	} else {
		if (caret_y + line_offset->destination() < 0) {
			line_offset->move(FD_TWEEN_EASE_OUT, -caret_y, 200);
		} else if (caret_y + line_offset->destination() + h > type_temp.box_height) {
			line_offset->move(FD_TWEEN_EASE_OUT,
				static_cast<double>(type_temp.box_height) - caret_y - h, 200);
		}
		Uint32 final_y{ 0 };
		if (lines.size() > 0) final_y = lines.back().y + lines.back().h;
		if (-line_offset->destination() + srcrect->h > final_y) {
			double push_up{ static_cast<double>(srcrect->h - final_y) };
			if (push_up > 0) push_up = 0;
			line_offset->move(FD_TWEEN_EASE_OUT, push_up, 200);
		}
	}
	// Position the caret
	if (type_temp.horz_scroll) {
		caret_rect->x = caret_x + dstrect->x + static_cast<int>(line_offset->value());
		caret_rect->y = caret_y + dstrect->y;
	} else {
		caret_rect->x = caret_x + dstrect->x;
		caret_rect->y = caret_y + dstrect->y + static_cast<int>(line_offset->value());
	}
	this->update();
}

void FD_TextBox::setEditing(bool edit) {
	this->editing = edit;
	if (!edit) for (auto b : boxes) b->setVisible(false);
}

std::shared_ptr<FD_Image> FD_TextBox::getImage() const {
	return pure_image;
}
bool FD_TextBox::isEditing() const {
	return editing;
}
FD_TextInfo FD_TextBox::getTextInfo() const {
	return text_info;
}

FD_Tween* FD_TextBox::getTweenX() {
	return x;
}
FD_Tween* FD_TextBox::getTweenY() {
	return y;
}

void FD_TextBox::assimilate(std::shared_ptr<FD_ObjectGroup> group) {
	this->object_group = group;
	group->addObject(shared_from_this());
	caret->assimilate(group);
}
