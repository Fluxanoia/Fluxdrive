#include "fd_cameraSet.hpp"

FD_CameraSet::FD_CameraSet(std::weak_ptr<FD_Window> window) {
	this->window = window;
}
FD_CameraSet::~FD_CameraSet() {}

void FD_CameraSet::update() {
	if (temping) {
		if (temp_camera != nullptr) {
			temp_camera->update();
			if (!temp_camera->isMoving()) {
				temping = false;
				this->setCamera(next_camera);
			}
		} else {
			temping = false;
		}
	}
	for (auto c : cameras) c.second->update();
}
void FD_CameraSet::pre_render() {
	if (temp_camera != nullptr) temp_camera->pre_render();
	for (auto c : cameras) c.second->pre_render();
}

void FD_CameraSet::transitionCamera(FD_CameraIndex index) {
	if (cameras.find(index) == cameras.end()) {
		FD_Handling::error("Unknown camera to transition to.");
		return;
	}
	switch (cam_tran) {
	case FD_CAMERA_TRAN_INSTANT:
		this->setCamera(index);
		break;
	case FD_CAMERA_TRAN_SMOOTH:
	case FD_CAMERA_TRAN_SMOOTH_PRESERVED:
		if (auto c = this->getCurrentCamera().lock()) {
			if (cam_tran == FD_CAMERA_TRAN_SMOOTH_PRESERVED) {
				std::shared_ptr<FD_Window> window;
				FD_Handling::lock(this->window, window, true);
				temp_camera = std::make_shared<FD_Camera>(c->getWidth(), 
					FD_CAMERA_SET_DEFAULT_CULL);
				temp_camera->associate(window);
				temp_camera->getTweenX()->set(c->getTweenX()->value());
				temp_camera->getTweenY()->set(c->getTweenY()->value());
				temp_camera->getTweenAngle()->set(c->getTweenAngle()->value());
			} else {
				temp_camera = c;
			}
			temping = true;
			transition(temp_camera, cameras.at(index));
			next_camera = index;
		} else {
			this->setCamera(index);
		}
		break;
	default:
		FD_Handling::error("Unknown camera transition", true);
		break;
	}
}
void FD_CameraSet::transition(std::shared_ptr<FD_Camera> from, 
	std::shared_ptr<FD_Camera> to) {
	FD_TweenAction a;
	a.type = cam_tran_type;
	a.duration = cam_tran_duration;
	// x
	a.destination = to->getTweenX()->value();
	from->getTweenX()->apply(a);
	// y
	a.destination = to->getTweenY()->value();
	from->getTweenY()->apply(a);
	// width and scale
	double pre_w = from->getWidth();
	from->getTweenWidth()->set(to->getTweenWidth()->value());
	a.destination = to->getTweenScale()->value();
	from->getTweenScale()->set(pre_w / from->getTweenWidth()->value());
	from->getTweenScale()->apply(a);
	// angle
	a.destination = to->getTweenAngle()->value();
	from->getTweenAngle()->apply(a);
}
void FD_CameraSet::setCameraTransition(FD_CameraTransition tran) {
	cam_tran = tran;
}
void FD_CameraSet::setCameraTransitionType(FD_TweenType type) {
	cam_tran_type = type;
}
void FD_CameraSet::setCameraTransitionDuration(Uint32 duration) {
	cam_tran_duration = duration;
}

void FD_CameraSet::setCamera(FD_CameraIndex index) {
	if (temping) return;
	if (cameras.find(index) != cameras.end()) {
		current_camera = index;
	} else {
		FD_Handling::error("Unknown camera to set to.");
	}
}
FD_CameraIndex FD_CameraSet::getCurrentCameraID() const {
	if (temping) return -1;
	return current_camera;
}
FD_CameraIndex FD_CameraSet::addCamera(int width, int culling_radius) {
	std::shared_ptr<FD_Camera> c{ 
		std::make_shared<FD_Camera>(width, culling_radius) 
	};
	c->associate(window);
	FD_CameraIndex ci{ static_cast<FD_CameraIndex>(camera_id_count) };
	camera_id_count++;
	cameras.insert_or_assign(ci, c);
	if (cameras.size() == 1) current_camera = ci;
	return ci;
}
std::weak_ptr<FD_Camera> FD_CameraSet::getCamera(FD_CameraIndex index) const {
	if (index < 0) return temp_camera;
	if (cameras.find(index) != cameras.end()) {
		return cameras.at(index);
	}
	return std::weak_ptr<FD_Camera>();
}
std::weak_ptr<FD_Camera> FD_CameraSet::getCurrentCamera() const {
	if (temping) return temp_camera;
	return getCamera(current_camera);
}

void FD_CameraSet::resized(int width, int height) {
	if (temp_camera != nullptr) temp_camera->resized(width, height);
}