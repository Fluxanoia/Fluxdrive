#include "fd_testing.hpp"

// Runs the test suite
void FD_Testing::test() {
	// Runs visual test
	std::shared_ptr<FD_Window> window{ std::make_shared<FD_Window>("Fluxdrive Test", 640, 360) };
#ifdef FD_TEST_MANUAL_SCENE
	std::shared_ptr<FD_Scene> scene{ std::make_shared<FD_Scene>(window, false, 1280, 720) };
#else
	std::shared_ptr<FD_Scene> scene{ std::make_shared<FD_Scene>(window, "test/config/display.fdc") };
#endif

	std::shared_ptr<FD_Registry> registry{ std::make_shared<FD_Registry>() };
	registry->log(FD_IMAGE_BACKGROUND, "test/images/bg.png");
	registry->log(FD_IMAGE_BUTTON, "test/images/button.png");
	registry->log(FD_FONT, "test/font/cmunvt.ttf");
	registry->log(FD_SONG, "test/audio/song.ogg");
	scene->getAudioManager()->setRegistry(registry);
	scene->getImageManager()->setRegistry(registry);

	std::shared_ptr<FD_StateManager> state_manager{ std::make_shared<FD_StateManager>(scene) };
	std::shared_ptr<FD_TestChoiceState> choice_test{ std::make_shared<FD_TestChoiceState>(scene) };
	std::shared_ptr<FD_CameraTestState> camera_test{ std::make_shared<FD_CameraTestState>(scene) };
	std::shared_ptr<FD_EventTestState> event_test{ std::make_shared<FD_EventTestState>(scene) };
	std::shared_ptr<FD_AudioTestState> audio_test{ std::make_shared<FD_AudioTestState>(scene) };
	state_manager->logState(choice_test);
	state_manager->logState(camera_test);
	state_manager->logState(event_test);
	state_manager->logState(audio_test);
	state_manager->setState(FD_TEST_CHOICE_STATE);

	state_manager->logEventListener(event_test->getEventListener());

	std::shared_ptr<FD_Looper> looper{ std::make_shared<FD_Looper>(state_manager, 60) };
	looper->loop();
}

// Test Choice State Member Functions

FD_Testing::FD_TestChoiceState::FD_TestChoiceState(std::weak_ptr<FD_Scene> s)
	: FD_State(FD_TEST_CAMERA_STATE, s) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(s, scene, true);
	// Create the group and camera set
	cameras = std::make_shared<FD_CameraSet>(scene->getWindow());
	camera = cameras->addCamera(1920);
	group = std::make_shared<FD_ObjectGroup>(object_list);
	group->setCameraSet(cameras);
	scene->addObjectGroup(group);
	// Create the button manager
	button_manager = new FD_ButtonManager(scene, cameras, input_list);
	button_manager->addDefaultMaps();
	std::shared_ptr<FD_ImageManager> im{ scene->getImageManager() };
	// Add background
	std::shared_ptr<FD_FileImage> bg_image;
	FD_Handling::lock(im->loadImage(FD_IMAGE_BACKGROUND),
		bg_image, true);
	background = std::make_shared<FD_Element>(bg_image,
		0, 0, 0, 0, 1, 1, false, FD_TOP_LEFT);
	background->assimilate(group);
	// Create the buttons
	int increment{ 0 };
	std::shared_ptr<FD_Font> def_font;
	SDL_Colour colour = { 255, 255, 255, 255 };
	FD_ButtonTemplate temp{ s, group, 10, true };
	temp.background = im->loadImage(FD_IMAGE_BUTTON);
	FD_Handling::lock(im->loadFont(FD_FONT, 32), def_font, true);
	for (auto s : button_titles) {
		button_manager->addBasicButton(temp, 0, 90 * increment,
			increment, s, def_font, colour);
		increment++;
	}
	// Get the input
	FD_Handling::lock(scene->getInputManager()->getInputSet(input_list),
		input, true);
}
FD_Testing::FD_TestChoiceState::~FD_TestChoiceState() { delete button_manager; }

void FD_Testing::FD_TestChoiceState::wake() {
	FD_State::wake();
}

void FD_Testing::FD_TestChoiceState::sleep() {
	button_manager->reset();
}

void FD_Testing::FD_TestChoiceState::update() {
	int code;
	while (button_manager->getEvent(code)) {
		switch (code) {
		case CAMERA_TEST:
			nextState = FD_TEST_CAMERA_STATE;
			break;
		case EVENT_TEST:
			nextState = FD_TEST_EVENT_STATE;
			break;
		case AUDIO_TEST:
			nextState = FD_TEST_AUDIO_STATE;
			break;
		case QUIT:
			closed = true;
			break;
		default:
			std::string debug{ "Unhandled code: " };
			debug += std::to_string(code);
			FD_Handling::debug(debug.c_str());
			break;
		}
	}
	background->update();
	button_manager->update();
}

void FD_Testing::FD_TestChoiceState::resized(int w, int h) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	if (background->getWidth() > background->getHeight()) {
		background->setHeight(h);
		background->getTweenScaleX()->set(background->getHeightScale());
	} else {
		background->setWidth(w);
		background->getTweenScaleY()->set(background->getWidthScale());
	}
}

// Camera Test State Member Functions

FD_Testing::FD_CameraTestState::FD_CameraTestState(std::weak_ptr<FD_Scene> s) 
	: FD_State(FD_TEST_CAMERA_STATE, s) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(s, scene, true);
	// Create the group and camera set
	cameras = std::make_shared<FD_CameraSet>(scene->getWindow());
	cameras->setCameraTransitionDuration(300);
	cameras->setCameraTransitionType(FD_TWEEN_EASE_OUT);
	camera_1 = cameras->addCamera(1920);
	camera_2 = cameras->addCamera(1280);
	group = std::make_shared<FD_ObjectGroup>(object_list);
	group->setCameraSet(cameras);
	scene->addObjectGroup(group);
	// Create the button manager
	button_manager = new FD_ButtonManager(scene, cameras, input_list);
	button_manager->addDefaultMouseMaps();
	std::shared_ptr<FD_ImageManager> im{ scene->getImageManager() };
	// Add background
	std::shared_ptr<FD_FileImage> bg_image;
	FD_Handling::lock(im->loadImage(FD_IMAGE_BACKGROUND), 
		bg_image, true);
	background = std::make_shared<FD_Element>(bg_image,
		0, 0, 0, 0, 1, 1, false, FD_TOP_LEFT);
	background->assimilate(group);
	// Create the box to show the other camera
	SDL_Colour colour{ 255, 0, 0, 255 };
	other_cam = std::make_shared<FD_Box>(0, 0, 0, 0, 3, true, colour);
	group->addObject(other_cam);
	// Create the buttons
	int increment{ 0 };
	std::shared_ptr<FD_Font> def_font;
	colour = { 255, 255, 255, 255 };
	FD_ButtonTemplate temp{ s, group, 10, true };
	temp.background = im->loadImage(FD_IMAGE_BUTTON);
	FD_Handling::lock(im->loadFont(FD_FONT, 32), def_font, true);
	for (auto s : button_titles) {
		button_manager->addBasicButton(temp, 0, 90 * increment,
			increment, s, def_font, colour);
		increment++;
	}
	std::vector<int> codes{ SWITCH_CAMERAS_INSTANT, 
		SWITCH_CAMERAS_SMOOTH, SWITCH_CAMERAS_PRESERVED };
	std::vector<std::string> texts{ "Instant", "Smooth", "Smooth (preserved)" };
	button_manager->addDropdownButton(temp, 0, -90, codes, texts, def_font, colour);
	codes.clear();
	texts.clear();
	int index{ 0 };
	for (SDL_Point p : resolutions) {
		codes.push_back(RESIZE_WINDOW + index);
		texts.push_back(std::to_string(p.x) + "x" + std::to_string(p.y));
		index++;
	}
	button_manager->addDropdownButton(temp, 0, -180, codes, texts, def_font, colour);
	// Get the input
	FD_Handling::lock(scene->getInputManager()->getInputSet(input_list),
		input, true);
	input->addKeyMap(FD_MAP_RELEASED, SDLK_ESCAPE, BACK);
	input->addKeyMap(FD_MAP_HELD, SDLK_w, CAMERA_UP);
	input->addKeyMap(FD_MAP_HELD, SDLK_s, CAMERA_DOWN);
	input->addKeyMap(FD_MAP_HELD, SDLK_a, CAMERA_LEFT);
	input->addKeyMap(FD_MAP_HELD, SDLK_d, CAMERA_RIGHT);
	input->addMouseWheelMap(FD_SCROLL_UP, CAMERA_IN);
	input->addMouseWheelMap(FD_SCROLL_DOWN, CAMERA_OUT);
}
FD_Testing::FD_CameraTestState::~FD_CameraTestState() { delete button_manager; }

void FD_Testing::FD_CameraTestState::wake() {
	FD_State::wake();
}

void FD_Testing::FD_CameraTestState::sleep() {
	button_manager->reset();
}

void FD_Testing::FD_CameraTestState::update() {
	int code;
	std::shared_ptr<FD_Camera> cam;
	FD_Handling::lock(cameras->getCurrentCamera(), cam, true);
	while (button_manager->getEvent(code)) {
		switch (code) {
		case SWITCH_CAMERAS:
			if (cameras->getCurrentCameraID() == camera_1) {
				cameras->transitionCamera(camera_2);
			} else {
				cameras->transitionCamera(camera_1);
			}
			break;
		case SHAKE_CAMERA:
			cam->shake(2);
			break;
		case SWITCH_CAMERAS_INSTANT:
			cameras->setCameraTransition(FD_CAMERA_TRAN_INSTANT);
			break;
		case SWITCH_CAMERAS_SMOOTH:
			cameras->setCameraTransition(FD_CAMERA_TRAN_SMOOTH);
			break;
		case SWITCH_CAMERAS_PRESERVED:
			cameras->setCameraTransition(FD_CAMERA_TRAN_SMOOTH_PRESERVED);
			break;
		default:
			int res = code - RESIZE_WINDOW;
			if (res >= 0 && static_cast<Uint32>(res) < resolutions.size()) {
				int res_x{ resolutions.at(res).x };
				int res_y{ resolutions.at(res).y };
				std::shared_ptr<FD_Scene> s;
				FD_Handling::lock(scene, s);
				s->getWindow()->setResolution(res_x, res_y);
				break;
			}
			std::string debug{ "Unhandled code: " };
			debug += std::to_string(code);
			FD_Handling::debug(debug.c_str());
			break;
		}
	}
	FD_InputEvent e;
	while (input->getEvent(e)) {
		switch (e.code) {
		case BACK:
			nextState = FD_TEST_CHOICE_STATE;
			break;
		case CAMERA_UP:
			cam->getTweenY()->move(FD_TWEEN_EASE_OUT,
				cam->getTweenY()->value() - camera_speed, 200);
			break;
		case CAMERA_DOWN:
			cam->getTweenY()->move(FD_TWEEN_EASE_OUT,
				cam->getTweenY()->value() + camera_speed, 200);
			break;
		case CAMERA_LEFT:
			cam->getTweenX()->move(FD_TWEEN_EASE_OUT,
				cam->getTweenX()->value() - camera_speed, 200);
			break;
		case CAMERA_RIGHT:
			cam->getTweenX()->move(FD_TWEEN_EASE_OUT,
				cam->getTweenX()->value() + camera_speed, 200);
			break;
		case CAMERA_IN: {
			double w = cam->getTweenWidth()->value() - camera_speed;
			if (w < 1) w = 1;
			cam->getTweenWidth()->set(w);
			break;
		}
		case CAMERA_OUT: {
			double w = cam->getTweenWidth()->value() + camera_speed;
			cam->getTweenWidth()->set(w);
			break;
		}
		}
	}
	FD_CameraIndex id{ camera_1 };
	if (cameras->getCurrentCameraID() == camera_1) {
		id = camera_2;
	}
	FD_Handling::lock(cameras->getCamera(id), cam, true);
	const SDL_Rect* cam_bounds{ cam->getBounds() };
	other_cam->updateRect(
		cam_bounds->x,
		cam_bounds->y,
		cam_bounds->w,
		cam_bounds->h
	);
	background->update();
	button_manager->update();
}

void FD_Testing::FD_CameraTestState::resized(int w, int h) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	if (background->getWidth() > background->getHeight()) {
		background->setHeight(h);
		background->getTweenScaleX()->set(background->getHeightScale());
	} else {
		background->setWidth(w);
		background->getTweenScaleY()->set(background->getWidthScale());
	}
}

// Event Test State Member Functions

FD_Testing::FD_EventTestState::FD_EventTestState(std::weak_ptr<FD_Scene> s)
	: FD_State(FD_TEST_EVENT_STATE, s) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(s, scene, true);
	// Create the group and camera set
	cameras = std::make_shared<FD_CameraSet>(scene->getWindow());
	camera = cameras->addCamera(1920);
	group = std::make_shared<FD_ObjectGroup>(object_list);
	group->setCameraSet(cameras);
	scene->addObjectGroup(group);
	// Create the listener
	listener = std::make_shared<FD_EventListener>();
	listener->setAccepting(false);
	// Grab the font
	std::shared_ptr<FD_ImageManager> im{ scene->getImageManager() };
	SDL_Color colour = { 255, 255, 255, 255 };
	FD_Handling::lock(im->loadFont(FD_FONT, 32), font, true);
	// Add background
	std::shared_ptr<FD_FileImage> bg_image;
	FD_Handling::lock(im->loadImage(FD_IMAGE_BACKGROUND), bg_image, true);
	background = std::make_shared<FD_Element>(bg_image,
		0, 0, 0, 0, 1, 1, false, FD_TOP_LEFT);
	background->assimilate(group);
	// Get the input
	FD_Handling::lock(scene->getInputManager()->getInputSet(input_list),
		input, true);
	input->addKeyMap(FD_MAP_RELEASED, SDLK_ESCAPE, BACK);
}
FD_Testing::FD_EventTestState::~FD_EventTestState() {  }

void FD_Testing::FD_EventTestState::addEntry(std::string text) {
	std::shared_ptr<FD_Scene> scene;
	SDL_Color colour = { 255, 255, 255, 255 };
	FD_Handling::lock(this->scene, scene, true);
	for (auto t : feed) t->getTweenY()->move(FD_TWEEN_ELASTIC,
		t->getTweenY()->destination() + 36, 750);
	std::shared_ptr<FD_Text> new_text{ std::make_shared<FD_Text>(
		scene->getWindow()->getRenderer(), font,
		"", text, "", colour, 50, 50, FD_TOP_LEFT, 10, false) };
	new_text->getTweenOpacity()->move(FD_TWEEN_EASE_OUT, 0, 1000, 2000);
	new_text->assimilate(group);
	feed.push_back(new_text);
}

void FD_Testing::FD_EventTestState::wake() {
	FD_State::wake();
	this->addEntry("The events will show up here.");
	this->addEntry("Welcome to the Event Listener test.");
	listener->clear();
	listener->setAccepting(true);
}

void FD_Testing::FD_EventTestState::sleep() {
	feed.clear();
	listener->setAccepting(false);
}

void FD_Testing::FD_EventTestState::update() {
	SDL_Event ev;
	while (listener->pullEvent(ev)) {
		std::string str{ "We have an event of type: " };
		str += std::to_string(ev.type);
		addEntry(str);
	}
	auto it = feed.begin();
	while (it != feed.end()) {
		if ((*it)->getTweenOpacity()->value() == 0.0) {
			it = feed.erase(it);
		} else {
			(*it)->update();
			it++;
		}
	}
	FD_InputEvent e;
	while (input->getEvent(e)) {
		switch (e.code) {
		case BACK:
			nextState = FD_TEST_CHOICE_STATE;
			break;
		}
	}
	background->update();
}

void FD_Testing::FD_EventTestState::resized(int w, int h) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	if (background->getWidth() > background->getHeight()) {
		background->setHeight(h);
		background->getTweenScaleX()->set(background->getHeightScale());
	}
	else {
		background->setWidth(w);
		background->getTweenScaleY()->set(background->getWidthScale());
	}
}

std::shared_ptr<FD_EventListener> FD_Testing::FD_EventTestState::getEventListener() const {
	return listener;
}

// Audio Test State Member Functions

FD_Testing::FD_AudioTestState::FD_AudioTestState(std::weak_ptr<FD_Scene> s)
	: FD_State(FD_TEST_EVENT_STATE, s) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(s, scene, true);
	// Create the group and camera set
	cameras = std::make_shared<FD_CameraSet>(scene->getWindow());
	camera = cameras->addCamera(1920);
	group = std::make_shared<FD_ObjectGroup>(object_list);
	group->setCameraSet(cameras);
	scene->addObjectGroup(group);
	// Create the button manager
	button_manager = new FD_ButtonManager(scene, cameras, input_list);
	button_manager->addDefaultMouseMaps();
	std::shared_ptr<FD_ImageManager> im{ scene->getImageManager() };
	// Add background
	std::shared_ptr<FD_FileImage> bg_image;
	FD_Handling::lock(im->loadImage(FD_IMAGE_BACKGROUND),
		bg_image, true);
	background = std::make_shared<FD_Element>(bg_image,
		0, 0, 0, 0, 1, 1, false, FD_TOP_LEFT);
	background->assimilate(group);
	// Add music
	FD_Handling::lock(scene->getAudioManager()->loadMusic(FD_SONG),
		music, true);
	// Create the buttons
	int increment{ 0 };
	std::shared_ptr<FD_Font> def_font;
	SDL_Color colour = { 255, 255, 255, 255 };
	FD_ButtonTemplate temp{ s, group, 10, true };
	temp.background = im->loadImage(FD_IMAGE_BUTTON);
	FD_Handling::lock(im->loadFont(FD_FONT, 32), def_font, true);
	for (auto s : button_titles) {
		button_manager->addBasicButton(temp, 0, 90 * increment,
			increment, s, def_font, colour);
		increment++;
	}
	// Get the input
	FD_Handling::lock(scene->getInputManager()->getInputSet(input_list),
		input, true);
	input->addKeyMap(FD_MAP_RELEASED, SDLK_ESCAPE, BACK);
}
FD_Testing::FD_AudioTestState::~FD_AudioTestState() { delete button_manager; }

void FD_Testing::FD_AudioTestState::wake() {
	FD_State::wake();
	music->playMusic(1000);
}

void FD_Testing::FD_AudioTestState::sleep() {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	scene->getAudioManager()->haltMusic();
	button_manager->reset();
}

void FD_Testing::FD_AudioTestState::update() {
	int code;
	while (button_manager->getEvent(code)) {
		switch (code) {
		default:
			std::string debug{ "Unhandled code: " };
			debug += std::to_string(code);
			FD_Handling::debug(debug.c_str());
			break;
		}
	}
	FD_InputEvent e;
	while (input->getEvent(e)) {
		switch (e.code) {
		case BACK:
			nextState = FD_TEST_CHOICE_STATE;
			break;
		}
	}
	background->update();
	button_manager->update();
}

void FD_Testing::FD_AudioTestState::resized(int w, int h) {
	std::shared_ptr<FD_Scene> scene;
	FD_Handling::lock(this->scene, scene, true);
	if (background->getWidth() > background->getHeight()) {
		background->setHeight(h);
		background->getTweenScaleX()->set(background->getHeightScale());
	}
	else {
		background->setWidth(w);
		background->getTweenScaleY()->set(background->getWidthScale());
	}
}
