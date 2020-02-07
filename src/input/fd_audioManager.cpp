#include "fd_audioManager.hpp"

#include "../main/fd_handling.hpp"

// Audio Manager Member Functions

FD_AudioManager::FD_AudioManager() : FD_Registered() {}
FD_AudioManager::~FD_AudioManager() {
	Mix_HaltMusic();
	musics.clear();
	sfxs.clear();
	FD_Handling::debug("FD_AudioManager destroyed.");
}

void FD_AudioManager::update() {
	if (!Mix_PlayingMusic() && Mix_FadingMusic() == MIX_NO_FADING) {
		for (std::shared_ptr<FD_Music> m : musics) {
			if (Mix_PlayingMusic() || Mix_FadingMusic() != MIX_NO_FADING) break;
			m->playIfQueued();
		}
	}
}

void FD_AudioManager::setMusicVolume(Sint8 volume) { Mix_VolumeMusic(volume); }
void FD_AudioManager::setSFXVolume(Sint8 volume) { Mix_Volume(-1, volume); }

void FD_AudioManager::haltMusic() { 
	Mix_HaltMusic();
}
void FD_AudioManager::haltSFX() {
	Mix_HaltChannel(-1);
}

std::weak_ptr<FD_Music> FD_AudioManager::loadMusic(const FD_MusicRegister reg) {
	for (std::shared_ptr<FD_Music> m : musics) {
		if (m->verify(reg)) return m;
	}
	std::shared_ptr<FD_Music> music = std::make_shared<FD_Music>(registry, reg);
	if (music->isLoaded()) {
		musics.push_back(music);
		return music;
	}
	FD_Handling::error("A music track could not be loaded.", true);
	return std::weak_ptr<FD_Music>();
}
std::weak_ptr<FD_SFX> FD_AudioManager::loadSoundEffect(const FD_SFXRegister reg, 
	const Uint32 value) {
	for (std::shared_ptr<FD_SFX> s : sfxs) {
		if (s->verify(reg, value)) return s;
	}
	std::shared_ptr<FD_SFX> sfx = std::make_shared<FD_SFX>(registry, reg, value);
	if (sfx->isLoaded()) {
		sfxs.push_back(sfx);
		return sfx;
	}
	FD_Handling::error("A sound effect could not be loaded.", true);
	return std::weak_ptr<FD_SFX>();
}
bool FD_AudioManager::deleteMusic(const FD_MusicRegister reg) {
	auto it = musics.begin();
	while (it != musics.end()) {
		if ((*it)->verify(reg)) {
			musics.erase(it);
			return true;
		} else {
			it++;
		}
	}
	return false;
}
bool FD_AudioManager::deleteSoundEffect(const FD_SFXRegister reg, const Uint32 value) {
	auto it = sfxs.begin();
	while (it != sfxs.end()) {
		if ((*it)->verify(reg, value)) {
			sfxs.erase(it);
			return true;
		} else {
			it++;
		}
	}
	return false;
}

// FD_Music Member Functions

FD_Music::FD_Music(const std::weak_ptr<FD_Registry> registry,
	const FD_MusicRegister reg) : reg{ reg } {
	std::string path;
	std::shared_ptr<FD_Registry> r;
	FD_Handling::lock(registry, r, true);
	if (r->get(reg, path)) {
		FD_Paths::ADD_BASE_PATH(path);
		music = Mix_LoadMUS(path.c_str());
		this->loaded = music != nullptr;
	}
}
FD_Music::~FD_Music() {
	if (music != nullptr) Mix_FreeMusic(music);
}

void FD_Music::playIfQueued() {
	if (queued) {
		Mix_FadeInMusic(music, -1, fadeIn);
		queued = false;
	}
}

void FD_Music::playMusic(int fadeOut, int fadeIn) {
	if (Mix_PlayingMusic() && fadeOut > 0) {
		Mix_FadeOutMusic(fadeOut);
	} else if (fadeOut == 0) { 
		Mix_HaltMusic();
	}
	if (fadeIn > 0) {
		if (Mix_PlayingMusic() && Mix_FadingMusic()) {
			queued = true;
			this->fadeIn = fadeIn;
		} else {
			Mix_FadeInMusic(music, -1, fadeIn);
		}
	} else {
		Mix_PlayMusic(music, -1);
	}
}

bool FD_Music::verify(const FD_MusicRegister reg) const {
	return this->reg == reg;
}

bool FD_Music::isLoaded() const {
	return loaded;
}

// FD_SFX Member Functions

FD_SFX::FD_SFX(const std::weak_ptr<FD_Registry> registry,
	const FD_SFXRegister reg, const Uint32 value) : reg{ reg }, value{ value } {
	std::string path;
	std::shared_ptr<FD_Registry> r;
	FD_Handling::lock(registry, r, true);
	if (r->get(reg, path)) {
		if (value != 0) {
			path.insert(path.find_last_of('.'), std::to_string(value));
		}
		FD_Paths::ADD_BASE_PATH(path);
		sfx = Mix_LoadWAV(path.c_str());
		this->loaded = sfx != nullptr;
	}
}
FD_SFX::~FD_SFX() {
	if (sfx != nullptr) Mix_FreeChunk(sfx);
}

void FD_SFX::play(int loops) const {
	Mix_PlayChannel(-1, sfx, loops);
}

bool FD_SFX::verify(const FD_SFXRegister reg, const Uint32 value) const {
	return this->reg == reg && this->value == value;
}

bool FD_SFX::isLoaded() const {
	return loaded;
}
