#include "fd_objectGroup.hpp"

// FD_Layered Member Functions

FD_Layered::FD_Layered(int z, bool camera_bound) {
	this->z = z;
	this->camera_bound = camera_bound;
}
FD_Layered::~FD_Layered() {}

void FD_Layered::setLayer(int z) {
	if (z != this->z) {
		re_layered = true;
	}
	this->z = z;
}
void FD_Layered::setCameraBound(bool camera_bound) {
	this->camera_bound = camera_bound;
}

int FD_Layered::getLayer() const { return z; }
bool FD_Layered::isCameraBound() const { return camera_bound; }
bool FD_Layered::hasChangedLayer() {
	if (re_layered) {
		re_layered = false;
		return true;
	}
	return false;
}

// FD_Object Group Member Functions

FD_ObjectGroup::FD_ObjectGroup(const int id) : id{ id } {
	opacity = new FD_Tween(255);
}
FD_ObjectGroup::~FD_ObjectGroup() {
	delete opacity;
}

void FD_ObjectGroup::update() {
	std::vector<std::shared_ptr<FD_Layered>> to_readd{};
	auto it = list.begin();
	while (it != list.end()) {
		if ((*it).use_count() == 1) {
			it = list.erase(it);
		} else if ((*it)->hasChangedLayer()) {
			to_readd.push_back((*it));
			it = list.erase(it);
		} else {
			it++;
		}
	}
	for (auto o : to_readd) this->addObject(o);
	if (auto set = cameras.lock()) set->update();
}

void FD_ObjectGroup::pre_render() {
	if (auto set = cameras.lock()) set->pre_render();
}
void FD_ObjectGroup::render(const size_t index, SDL_Renderer* renderer) const {
	if (index >= list.size()) return;
	if (!visible) return;
	if (auto set = cameras.lock()) {
		if (auto c = set->getCurrentCamera().lock()) {
			list.at(index)->render(renderer, this->getOpacity(), c);
		}
	}
}
void FD_ObjectGroup::render_all(SDL_Renderer* renderer) const {
	if (!visible) return;
	if (auto set = cameras.lock()) {
		if (auto c = set->getCurrentCamera().lock()) {
			for (auto o : list) o->render(renderer, this->getOpacity(), c);
		}
	}
}

void FD_ObjectGroup::addObject(const std::shared_ptr<FD_Layered> o) {
	// It's necessary to put the object in at the right z layer
	// position, as the list is ascending, binary search is used
	int layer{ o->getLayer() };
	size_t upper{ list.size() };
	size_t lower{ 0 };
	size_t mid{ static_cast<size_t>(std::floor((lower / 2.0) + (upper / 2.0))) };
	while (upper - lower > 0) {
		// Edge case breakdown
		if (upper - lower == 1) {
			int u_l{ 0 };
			if (upper == list.size()) {
				u_l = list.back()->getLayer();
			} else {
				u_l = list.at(upper)->getLayer();
			}
			int l_l = list.at(lower)->getLayer();
			if (layer < u_l) {
				if (layer > l_l) {
					mid = upper;
				} else {
					mid = lower;
				}
			} else {
				mid = upper;
			}
			break;
		}
		// Recursive element
		mid = static_cast<int>(std::floor((lower / 2.0) + (upper / 2.0)));
		if (list.at(mid)->getLayer() == layer) {
			break;
		} else if (list.at(mid)->getLayer() < layer) {
			lower = mid;
		} else {
			upper = mid;
		}
	}
	list.insert(list.begin() + mid, o);
	return;
}
void FD_ObjectGroup::addObjects(const std::vector<std::shared_ptr<FD_Layered>> os) {
	for (auto o : os) addObject(o);
}

void FD_ObjectGroup::setCameraSet(std::weak_ptr<FD_CameraSet> set) {
	this->cameras = set;
}
std::weak_ptr<FD_CameraSet> FD_ObjectGroup::getCameraSet() const {
	return cameras;
}

int FD_ObjectGroup::getID() const {
	return id;
}
bool FD_ObjectGroup::isVisible() const {
	return visible;
}
Uint8 FD_ObjectGroup::getOpacity() const {
	double v = opacity->value();
	if (v > 255) return 255;
	if (v < 0) return 0;
	return static_cast<Uint8>(v);
}
size_t FD_ObjectGroup::getSize() const {
	return list.size();
}
bool FD_ObjectGroup::getLayer(size_t index, int& layer) const {
	if (index >= list.size()) return false;
	layer = list.at(index)->getLayer();
	return true;
}

void FD_ObjectGroup::setVisible(bool visible) {
	this->visible = visible;
}
FD_Tween* FD_ObjectGroup::getTweenOpacity() { return opacity; }
