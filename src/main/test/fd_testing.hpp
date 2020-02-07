#ifndef FD_TESTING_H_
#define FD_TESTING_H_

#include "../../display/fd_scene.hpp"
#include "../../display/fd_window.hpp"
#include "../../display/fd_cameraSet.hpp"
#include "../../object/fd_objectGroup.hpp"
#include "../../state/fd_stateManager.hpp"
#include "../../object/ui/fd_buttonManager.hpp"

namespace FD_Testing {

	enum FD_TestStates {
		FD_TEST_CAMERA_STATE
	};

	enum FD_TestRegisters {
		FD_IMAGE_BACKGROUND,
		FD_IMAGE_BUTTON,

		FD_FONT,

		FD_SONG
	};

	class FD_CameraTestState : public FD_State {
	private:

		enum Inputs {
			CAMERA_UP,
			CAMERA_DOWN,
			CAMERA_LEFT,
			CAMERA_RIGHT
		};

		enum ButtonResponses {
			SWITCH_CAMERAS,
			QUIT,

			SWITCH_CAMERAS_INSTANT,
			SWITCH_CAMERAS_SMOOTH,
			SWITCH_CAMERAS_PRESERVED
		};
		std::vector<std::string> button_titles{
			"Switch Cameras",
			"Quit"
		};

		const double camera_speed{ 25 };
		FD_CameraIndex camera_1, camera_2;
		std::shared_ptr<FD_CameraSet> cameras;
		std::shared_ptr<FD_ObjectGroup> group;
		std::shared_ptr<FD_InputSet> input;

		FD_ButtonManager* button_manager;
		std::shared_ptr<FD_Element> background;
		std::shared_ptr<FD_Music> music;

		std::shared_ptr<FD_Box> other_cam;

	public:

		FD_CameraTestState(std::weak_ptr<FD_Scene> scene);
		~FD_CameraTestState();

		void sleep() override;
		void wake() override;
		void update() override;

		void resized(int w, int h) override;
	};

	void test();

}

#endif