#include "fd_serialisation.hpp"

#include "../main/fd_handling.hpp"

std::string FD_Serialisation::trim(std::string& info) {
	if (info.find(delim()) == std::string::npos) {
		std::string value = info;
		info = "";
		return value;
	}
	std::string value = info.substr(0, info.find(delim()));
	info = info.substr(info.find(delim()) + 1, info.length());
	return value;
}

bool FD_Serialisation::trimBool(std::string& info) {
	bool value;
	std::istringstream stream{ trim(info) };
	stream >> value;
	if (stream.fail()) {
		FD_Handling::error("Serialisation: bool could not be read.", true);
		return false;
	}
	return value;
}

double FD_Serialisation::trimDouble(std::string& info) {
	double value;
	std::istringstream stream{ trim(info) };
	stream >> value;
	if (stream.fail()) {
		FD_Handling::error("Serialisation: double could not be read.", true);
		return 0;
	}
	return value;
}

int FD_Serialisation::trimInteger(std::string& info) {
	int value;
	std::istringstream stream{ trim(info) };
	stream >> value;
	if (stream.fail()) {
		FD_Handling::error("Serialisation: int could not be read.", true);
		return 0;
	}
	return value;
}

void FD_Serialisation::trimRect(std::string& info, SDL_Rect& rect) {
	rect.x = trimInteger(info);
	rect.y = trimInteger(info);
	rect.w = trimInteger(info);
	rect.h = trimInteger(info);
}

void FD_Serialisation::trimPoint(std::string& info, SDL_Point& p) {
	p.x = trimInteger(info);
	p.y = trimInteger(info);
}

std::string FD_Serialisation::inspect(std::string info) {
	if (info.find(delim()) == std::string::npos) return info;
	return info.substr(0, info.find(delim()));
}

bool FD_Serialisation::inspectBool(std::string info) {
	bool value;
	std::istringstream stream{ inspect(info) };
	stream >> value;
	if (stream.fail()) {
		FD_Handling::error("Serialisation: bool could not be read.", true);
		return false;
	}
	return value;
}

double FD_Serialisation::inspectDouble(std::string info) {
	double value;
	std::istringstream stream{ inspect(info) };
	stream >> value;
	if (stream.fail()) {
		FD_Handling::error("Serialisation: double could not be read.", true);
		return 0;
	}
	return value;
}

int FD_Serialisation::inspectInteger(std::string info) {
	int value;
	std::istringstream stream{ inspect(info) };
	stream >> value;
	if (stream.fail()) {
		FD_Handling::error("Serialisation: int could not be read.", true);
		return 0;
	}
	return value;
}

void FD_Serialisation::inspectRect(std::string info, SDL_Rect& rect) {
	trimRect(info, rect);
}

void FD_Serialisation::inspectPoint(std::string info, SDL_Point& p) {
	trimPoint(info, p);
}

void FD_Serialisation::append(std::string& info, const std::string append) {
	info += delim();
	info.append(append);
}
void FD_Serialisation::append(std::string& info, const bool append) { 
	FD_Serialisation::append(info, std::to_string(append));
}
void FD_Serialisation::append(std::string& info, const double append) {
	FD_Serialisation::append(info, std::to_string(append));
}
void FD_Serialisation::append(std::string& info, const int append) {
	FD_Serialisation::append(info, std::to_string(append));
}
void FD_Serialisation::append(std::string& info, const SDL_Rect append) {
	FD_Serialisation::append(info, append.x);
	FD_Serialisation::append(info, append.y);
	FD_Serialisation::append(info, append.w);
	FD_Serialisation::append(info, append.h);
}

char FD_Serialisation::delim() { return '_'; }