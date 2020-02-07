#include "fd_fileManager.hpp"

#include <filesystem>

#include "fd_serialisation.hpp"
#include "../main/fd_handling.hpp"

// File Manager Member Functions

FD_FileManager::FD_FileManager() : FD_Registered() {}

FD_FileManager::~FD_FileManager() {
	files.clear();
	FD_Handling::debug("FD_FileManager destroyed.");
}

std::shared_ptr<FD_File> FD_FileManager::loadFile(
	const std::string path, const std::string ending){
	// Check if the file is already loaded
	for (std::shared_ptr<FD_File> f : files) {
		if (f->hasPath(path, ending)) return f;
	}
	// Load the file and return it if it's not loaded
	std::shared_ptr<FD_File> file = std::make_shared<FD_File>(path, ending);
	files.push_back(file);
	return file;
}
std::shared_ptr<FD_File> FD_FileManager::loadFile(
	const FD_FileRegister reg, const std::string ending) {
	std::string path;
	std::shared_ptr<FD_Registry> r;
	FD_Handling::lock(registry, r, true);
	r->get(reg, path);
	FD_Paths::ADD_BASE_PATH(path);
	return loadFile(path, ending);
}


bool FD_FileManager::deleteFile(const std::string path, const std::string ending) {
	// Check if the file is already loaded
	for (auto it = files.begin(); it != files.end(); it++) {
		if ((*it)->hasPath(path, ending)) {
			files.erase(it);
			break;
		}
	}
	// Delete the file
	return remove((path + ending).c_str()) == 0;
}
bool FD_FileManager::deleteFile(const FD_FileRegister reg, const std::string ending) {
	std::string path{};
	std::shared_ptr<FD_Registry> r;
	FD_Handling::lock(registry, r, true);
	r->get(reg, path);
	return deleteFile(path, ending);
}

// File Member Functions

FD_File::FD_File(const std::string path, const std::string ending) {
	this->path = path;
	this->path.append(ending);
	this->load();
}

FD_File::~FD_File() {
	data.clear();
}

void FD_File::load() {
	data.clear();
	std::ifstream stream{ this->path, std::ios::in };
	if (stream.is_open() && stream.good()) {
		std::string buffer;
		while (std::getline(stream, buffer)) data.push_back(buffer);
		this->existing = true;
	} else {
		this->existing = false;
	}
	stream.close();
}

// Creates a file if it doesn't exist already
bool FD_File::create() {
	if (this->existing) return false;
	size_t last_slash{ path.find_last_of('/') };
	std::filesystem::create_directories(path.substr(0, last_slash));
	std::ofstream creator{ this->path, std::ios::out | std::ios::trunc };
	this->existing = true;
	creator.close();
	return true;
}

// Append lines to the end of a file
bool FD_File::append(const std::vector<std::string> &inputs) {
	if (!this->existing) return false;
	std::ofstream stream { this->path, std::ios::out | std::ios::app };
	for (auto it = inputs.begin(); it != inputs.end(); it++) {
		stream << (*it) << "\n";
	}
	stream.close();
	load();
	return existing;
}

// Write lines to the file
bool FD_File::overwrite(const std::vector<std::string> &inputs) {
	std::ofstream stream { this->path, std::ios::out | std::ios::trunc };
	for (auto it = inputs.begin(); it != inputs.end(); it++) {
		stream << (*it) << "\n";
	}
	stream.close();
	load();
	return existing;
}

// Returns the value of a key in the file
bool FD_File::getValue(const std::string key, std::string &value) const {
	if (!existing) return false;
	for (auto it = data.begin(); it != data.end(); it++) {
		if (strcmp(key.c_str(), (*it).substr(0, (*it).find(delim)).c_str()) == 0) {
			value = (*it).substr((*it).find(delim) + 1, (*it).length()).c_str();
			return true;
		}
	}
	return false;
}
bool FD_File::getIntegerValue(const std::string key, int &value) const {
	std::string value_str;
	if (!this->getValue(key, value_str)) return false;
	std::istringstream stream{ value_str };
	stream >> value;
	return !stream.fail();
}
bool FD_File::getDoubleValue(const std::string key, double &value) const {
	std::string value_str;
	if (!this->getValue(key, value_str)) return false;
	std::istringstream stream{ value_str };
	stream >> value;
	return !stream.fail();
}
bool FD_File::getBoolValue(const std::string key, bool &value) const {
	std::string value_str;
	if (!this->getValue(key, value_str)) return false;
	std::istringstream stream{ value_str };
	int int_value;
	stream >> int_value;
	value = (int_value > 0);
	return !stream.fail();
}

// Returns the value of a key, adding the field with a default if it doesn't exist
bool FD_File::getValue(const std::string key, std::string &value, const std::string &def) {
	if (getValue(key, value)) return true;
	std::vector<std::string> field{ key + delim + def };
	if (append(field)) {
		value = def;
		return true;
	}
	return false;
}
bool FD_File::getIntegerValue(const std::string key, int &value, const std::string &def) {
	std::string pre_value;
	if (getValue(key, pre_value, def)) {
		std::istringstream stream{ pre_value };
		stream >> value;
		return !stream.fail();
	}
	return false;
}
bool FD_File::getDoubleValue(const std::string key, double &value, const std::string &def) {
	std::string pre_value;
	if (getValue(key, pre_value, def)) {
		std::istringstream stream{ pre_value };
		stream >> value;
		return !stream.fail();
	}
	return false;
}
bool FD_File::getBoolValue(const std::string key, bool &value, const std::string &def) {
	std::string pre_value;
	if (getValue(key, pre_value, def)) {
		std::istringstream stream{ pre_value };
		int int_value;
		stream >> int_value;
		value = (int_value > 0);
		return !stream.fail();
	}
	return false;
}

// Sets the value of a key in the file
bool FD_File::setValue(const std::string &key, const std::string &value) {
	if (!existing) return false;
	std::string line = key + delim + value;
	for (auto it = data.begin(); it != data.end(); it++) {
		if (strcmp(key.c_str(), (*it).substr(0, (*it).find(delim)).c_str()) == 0) {
			(*it) = line;
			overwrite(data);
			return true;
		}
	}
	return false;
}

bool FD_File::hasPath(const std::string path, const std::string ending) const {
	std::string p{};
	p.append(path);
	p.append(ending);
	return (strcmp(this->path.c_str(), p.c_str())) == 0;
}

// Returns whether the path to the file is empty
bool FD_File::isPathEmpty() const {
	return (strcmp(this->path.c_str(), "")) == 0;
}

// Returns a copy of read data
std::vector<std::string> FD_File::getData() const {
	return data;
}

// Returns whether data has been loaded
bool FD_File::exists() const {
	if (isPathEmpty()) return false;
	return existing;
}