#include "fd_registry.hpp"

FD_Registry::FD_Registry() {}
FD_Registry::~FD_Registry() {}

void FD_Registry::log(const int id, const std::string value) {
	maps.insert_or_assign(id, value);
}

bool FD_Registry::get(const int id, std::string& value) const {
	if (maps.find(id) == maps.end()) return false;
	value = maps.at(id);
	return true;
}

void FD_Registered::setRegistry(std::weak_ptr<FD_Registry> registry) {
	this->registry = registry;
}