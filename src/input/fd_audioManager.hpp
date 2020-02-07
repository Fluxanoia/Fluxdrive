#ifndef FD_AUDIO_MANAGER_H_
#define FD_AUDIO_MANAGER_H_

#include <string>
#include <memory>
#include <vector>
#include <functional>

#include <SDL_mixer.h>

#include "fd_paths.hpp"
#include "fd_registry.hpp"

/*!
	@file
	@brief The file containing the classes relating to audio.
*/

//! The data type of the music register value.
typedef int FD_MusicRegister;
//! The data type of the SFX register value.
typedef int FD_SFXRegister;

//! The FD_Music class, containing and managing a music track.
class FD_Music {
private:

	bool loaded{ false };
	const FD_MusicRegister reg;
	Mix_Music* music{ nullptr };

	bool queued{ false };
	int fadeIn{ 0 };

public:

	//! Constructs a FD_Music.
	/*!
		\warning This only works for .wav, .mp3, .flac, .ogg files.

		\param registry The registry to retrieve the music path from.
		\param reg      The register of the path.
	*/
	FD_Music(const std::weak_ptr<FD_Registry> registry, const FD_MusicRegister reg);
	//! Destroys a FD_Music.
	~FD_Music();

	//! When called and the music is queued, it plays instantly.
	/*!
		This allows the music to fade out the previous music and wait on a
		managing class (FD_AudioManager) to play it.
	*/
	void playIfQueued();
	//! This fades out the previous music and fades in this class' track.
	/*!
		\param fadeOut The length of the fade out of the previous music in ms.
		\param fadeIn  The length of the fade in of the new music in ms.
	*/
	void playMusic(const int fadeOut = 0, const int fadeIn = 0);

	//! Checks whether this class' register is identical to the given one.
	/*!
		\param reg The register to check against.

		\return Whether this class' register is identical to the given one.
	*/
	bool verify(const FD_MusicRegister reg) const;

	//! Returns whether the track in this class is loaded.
	/*!
		\return Whether the track in this class is loaded.
	*/
	bool isLoaded() const;

};

//! The FD_SFX class, containing and managing a SFX chunk.
class FD_SFX {
private:

	bool loaded{ false };
	const FD_SFXRegister reg;
	const Uint32 value;
	Mix_Chunk* sfx{ nullptr };

public:

	//! Constructs a FD_SFX.
	/*!
		SFX chunks are retrieved in a unique way through the use of values.
		If the value is zero, the class will directly use the path given by
		the registry. However, if the value is non-zero, it will append it 
		to the end of the path. 
		
		For example, you can label similar sounds:
		"blip1.wav", "blip2.wav", etc. and add "blip.wav" to the registry
		and construct the FD_SFX class with values 1, 2, etc.

		\warning This only works for .wav files.

		\param registry The registry to retrieve the SFX path from.
		\param reg      The register of the path.
		\param value    The value of the track.
	*/
	FD_SFX(const std::weak_ptr<FD_Registry> registry,
		const FD_SFXRegister reg, const Uint32 value);
	//! Destroys the FD_SFX.
	~FD_SFX();

	//! Plays the SFX.
	/*!
		\param loops The number of time the effect should loop.
	*/
	void play(int loops = 0) const;

	//! Checks whether this class' register and value is identical to the given ones.
	/*!
		\param reg   The register to check against.
		\param value The value to check against.

		\return Whether this class' register and value is identical to the given one.
	*/
	bool verify(const FD_SFXRegister reg, const Uint32 value) const;

	//! Returns whether the track in this class is loaded.
	/*!
		\return Whether the track in this class is loaded.
	*/
	bool isLoaded() const;

};

//! The FD_AudioManager class, manages FD_SFX and FD_Music instances.
class FD_AudioManager : public FD_Registered {
private:

	std::vector<std::shared_ptr<FD_SFX>> sfxs{  };
	std::vector<std::shared_ptr<FD_Music>> musics{  };

public:

	//! Constructs a FD_AudioManager.
	FD_AudioManager();
	//! Destroys the FD_AudioManager.
	~FD_AudioManager();

	//! Updates the tracks of the manager.
	void update();

	//! Sets the volume of the music.
	/*!
		\param volume The volume to set to.
	*/
	void setMusicVolume(Sint8 volume);
	//! Sets the volume of the sound effects.
	/*!
		\param volume The volume to set to.
	*/
	void setSFXVolume(Sint8 volume);

	//! Stops all music.
	void haltMusic();
	//! Stops all sound effects.
	void haltSFX();

	//! Loads a music track, adds it to the manager and, returns it.
	/*!
		\param reg The register of the path to load.

		\return The loaded music.
	*/
	std::weak_ptr<FD_Music> loadMusic(const FD_MusicRegister reg);
	//! Loads a SFX chunk, adds it to the manager and, returns it.
	/*!
		\param reg   The register of the path to load.
		\param value The value of the SFX to load.

		\return The loaded SFX.
	*/
	std::weak_ptr<FD_SFX> loadSoundEffect(const FD_SFXRegister reg, const Uint32 value);
	//! Deletes a music track from the manager.
	/*!
		\param reg The register of the path to delete.

		\return Whether a track was deleted or not.
	*/
	bool deleteMusic(const FD_MusicRegister reg);
	//! Deletes a SFX chunk from the manager.
	/*!
		\param reg   The register of the path to delete.
		\param value The value of the SFX to delete.

		\return Whether a chunk was deleted or not.
	*/
	bool deleteSoundEffect(const FD_SFXRegister reg, const Uint32 value);

};

#endif