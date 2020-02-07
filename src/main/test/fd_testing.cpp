#include "fd_testing.hpp"

void FD_Testing::test() {
	std::shared_ptr<FD_Window> window{ std::make_shared<FD_Window>("Fluxdrive Test", 640, 360) };
	std::shared_ptr<FD_Scene> scene{ std::make_shared<FD_Scene>(window, "test/config/display.fdc") };

	std::shared_ptr<FD_Registry> registry{ std::make_shared<FD_Registry>() };
	registry->log(FD_IMAGE_BACKGROUND, "test/images/bg.png");
	registry->log(FD_IMAGE_BUTTON, "test/images/button.png");
	registry->log(FD_FONT, "test/font/cmunvt.ttf");
	registry->log(FD_SONG, "test/audio/song.ogg");
	scene->getAudioManager()->setRegistry(registry);
	scene->getImageManager()->setRegistry(registry);

	std::shared_ptr<FD_StateManager> state_manager{ std::make_shared<FD_StateManager>(scene) };
	std::shared_ptr<FD_CameraTestState> camera_test{ std::make_shared<FD_CameraTestState>(scene) };
	state_manager->logState(camera_test);
	state_manager->setState(FD_TEST_CAMERA_STATE);

	std::shared_ptr<FD_Looper> looper{ std::make_shared<FD_Looper>(state_manager, 60) };
	looper->loop();
}

// Test State Member Functions

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
	// Add music
	FD_Handling::lock(scene->getAudioManager()->loadMusic(FD_SONG),
		music, true);
	music->playMusic();
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
	// Get the input
	FD_Handling::lock(scene->getInputManager()->getInputSet(input_list),
		input, true);
	input->addKeyMap(FD_MAP_HELD, SDLK_w, CAMERA_UP);
	input->addKeyMap(FD_MAP_HELD, SDLK_s, CAMERA_DOWN);
	input->addKeyMap(FD_MAP_HELD, SDLK_a, CAMERA_LEFT);
	input->addKeyMap(FD_MAP_HELD, SDLK_d, CAMERA_RIGHT);
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
	while (button_manager->getEvent(code)) {
		switch (code) {
		case SWITCH_CAMERAS:
			if (cameras->getCurrentCameraID() == camera_1) {
				cameras->transitionCamera(camera_2);
			} else {
				cameras->transitionCamera(camera_1);
			}
			break;
		case QUIT:
			closed = true;
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
			std::string debug{ "Unhandled code: " };
			debug += std::to_string(code);
			FD_Handling::debug(debug.c_str());
			break;
		}
	}
	FD_InputEvent e;
	std::shared_ptr<FD_Camera> cam;
	while (input->getEvent(e)) {
		switch (e.code) {
		case CAMERA_UP:
			FD_Handling::lock(cameras->getCurrentCamera(),
				cam, true);
			cam->getTweenY()->move(FD_TWEEN_EASE_OUT,
				cam->getTweenY()->value() - camera_speed, 200);
			break;
		case CAMERA_DOWN:
			FD_Handling::lock(cameras->getCurrentCamera(),
				cam, true);
			cam->getTweenY()->move(FD_TWEEN_EASE_OUT,
				cam->getTweenY()->value() + camera_speed, 200);
			break;
		case CAMERA_LEFT:
			FD_Handling::lock(cameras->getCurrentCamera(),
				cam, true);
			cam->getTweenX()->move(FD_TWEEN_EASE_OUT,
				cam->getTweenX()->value() - camera_speed, 200);
			break;
		case CAMERA_RIGHT:
			FD_Handling::lock(cameras->getCurrentCamera(),
				cam, true);
			cam->getTweenX()->move(FD_TWEEN_EASE_OUT,
				cam->getTweenX()->value() + camera_speed, 200);
			break;
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
