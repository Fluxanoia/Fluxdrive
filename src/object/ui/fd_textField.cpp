#include "fd_textField.hpp"

FD_TextField::FD_TextField(const FD_ButtonTemplate& temp,
		const FD_TextTemplate& type_temp,
		int input_list, int x, int y, int code,
		int padding_left, int padding_top,
		int padding_right, int padding_bottom) : FD_Button(temp) {
	this->scene = temp.scene;
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	// Assign variables
	this->padding_left = padding_left;
	this->padding_top = padding_top;
	this->padding_bottom = padding_bottom;
	this->padding_right = padding_right;
	// Getting the textures
	std::shared_ptr<FD_Image> background;
	bool has_background{ false };
	if (background = temp.background.lock()) has_background = true;
	// Get the largest width and height
	Uint32 width{ temp.width }, height{ temp.height };
	Uint32 text_box_width{ type_temp.box_width + padding_left + padding_right };
	Uint32 text_box_height{ type_temp.box_height + padding_top + padding_bottom };
	if (has_background) {
		width = background->getWidth();
		height = background->getHeight();
	}
	if (temp.stretch_bg) {
		bool box_width{ false };
		bool box_height{ false };
		if (text_box_width > width) {
			width = text_box_width;
			box_width = true;
		}
		if (text_box_height > height) {
			height = text_box_height;
			box_height = true;
		}
		if (box_width) width += temp.stretch_buffer_w;
		if (box_height) height += temp.stretch_buffer_h;
	}
	if (width == 0 || height == 0) FD_Handling::error("FD_TextField constructed with no area.");
	// Create the text box
	text_box = std::make_shared<FD_TextBox>(
		scene, type_temp, x, y, temp.z + 1, 
		temp.camera_bound, FD_CENTERED
	);
	// Add the background
	if (has_background) elements.push_back(new FD_PureElement{ background });
	pure_image = std::make_shared<FD_PureImage>(
			scene->getWindow()->getRenderer(), width, height, elements);
	scene->getWindow()->addResizable(pure_image);
	this->image = pure_image;
	// Create an input set
	std::shared_ptr<FD_InputSet> parent_set;
	FD_Handling::lock(scene->getInputManager()->getInputSet(input_list),
		parent_set, true);
	input = parent_set->generateSharedSet();
	input->addKeyMap(FD_MAP_PRESSED, SDLK_x, CUT);
	input->addKeyMap(FD_MAP_PRESSED, SDLK_c, COPY);
	input->addKeyMap(FD_MAP_PRESSED, SDLK_a, SELECT_ALL);
	input->addKeyMap(FD_MAP_PRESSED, SDLK_v, PASTE);
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
FD_TextField::~FD_TextField() { 
	for (auto e : elements) delete e;
}

void FD_TextField::press() {
	if (selected) {
		if (auto sfx = sfx_press.lock()) sfx->play();
		held = true;
	} else {
		if (focus) {
			input->resetTextSelection();
			text_box->setEditing(false);
			changeTextBox();
		}
		focus = false;
	}
	this->updateColour();
}
bool FD_TextField::release() {
	if (held) {
		held = false;
		if (!focus) input->resetTyped(text_box->getTextInfo().text);
		focus = true;
		text_box->setEditing(true);
		if (auto sfx = sfx_release.lock()) sfx->play();
		this->updateColour();
		return true;
	}
	return false;
}
void FD_TextField::reset() {
	if (auto i = image.lock()) {
		i->setOverlayColour({ 0, 0, 0, 0 });
	}
}

void FD_TextField::wake() {
	text_box->wake();
}
void FD_TextField::changeTextBox() {
	FD_TextInfo info{ FD_TextInfo{
			input->getTypedText(),
			input->getCaretPosition(),
			input->getSelectionStartIndex(),
			input->getSelectionEndIndex()
	} };
	text_box->changeText(info);
	has_changed = true;
}
void FD_TextField::update(FD_ButtonActivity activity) {
	// Inputs
	FD_InputEvent e;
	while (focus && input->getEvent(e)) {
		switch (e.code) {
		case CUT:
			input->cutText();
			break;
		case COPY:
			input->copyText();
			break;
		case PASTE:
			input->pasteText();
			break;
		case SELECT_ALL:
			input->selectAllText();
			break;
		}
	}
	if (!focus) input->empty();
	// Bounds updating
	if (x->moved() || y->moved() || w->moved() || h->moved()) {
		FD_Object::updateBounds(dstrect, x->value(),
			y->value(), w->value(), h->value(), FD_CENTERED);
		text_box->getTweenX()->set(x->value());
		text_box->getTweenY()->set(y->value());
	}
	// Text
	text_box->update();
	if (focus && input->hasTypedTextChanged()) {
		changeTextBox();
	} else if (focus && input->hasCaretChanged()) {
		text_box->updateCaret(input->getCaretPosition());
	}
	// Selection
	if (!activity.selected) activity.selected = mouseSelected(activity);
	if (activity.selected != this->selected) {
		this->selected = activity.selected;
		if (!selected) {
			held = false;
			if (!activity.mouse) {
				focus = false;
				input->resetTextSelection();
				text_box->setEditing(false);
				changeTextBox();
			}
		}
		this->updateColour();
		if (auto sfx = sfx_hover.lock()) sfx->play();
	}
}
void FD_TextField::updateColour() {
	if (auto i = image.lock()) {
		if (selected) {
			if (held) {
				i->setOverlayColour({ 0, 0, 0, 120 });
			} else {
				i->setOverlayColour({ 255, 255, 255, 120 });
			}
		} else {
			if (focus) {
				i->setOverlayColour({ 255, 255, 255, 40 });
			} else {
				i->setOverlayColour({ 0, 0, 0, 0 });
			}
		}
	}
}

bool FD_TextField::mouseSelected(FD_ButtonActivity a) const {
	if (!a.mouse) return false;
	if (this->isCameraBound()) {
		return FD_Maths::pointInRect(a.camera_x, a.camera_y, dstrect);
	} else {
		return FD_Maths::pointInRect(a.x, a.y, dstrect);
	}
}

bool FD_TextField::hasTextChanged() {
	if (has_changed) {
		has_changed = false;
		return true;
	}
	return false;
}

FD_TextInfo FD_TextField::getTextInfo() const {
	return text_box->getTextInfo();
}

bool FD_TextField::isFocused() const {
	return focus;
}

void FD_TextField::assimilate(std::shared_ptr<FD_ObjectGroup> group) {
	group->addObject(shared_from_this());
	text_box->assimilate(group);
}


