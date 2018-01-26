#pragma once

//---------------------------------------------------------------------------
#pragma comment( lib, "ThirdParty/fmod/fmodex_vc" ) // Link in the fmodex_vc.lib static library

//---------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>

//---------------------------------------------------------------------------
typedef unsigned int SoundID;
typedef void* AudioChannelHandle;
const unsigned int MISSING_SOUND_ID = 0xffffffff;

//---------------------------------------------------------------------------

/** #General_Usage

AudioSystem* audiosys = new AudioSystem();
audiosys->PlayAudioFromPath("path/of/audio/file");

audiosys->Update(deltaSeconds); // Update every frame;

*/
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();

	SoundID CreateOrGetSound(const std::string& soundFileName, bool loop = false);

	AudioChannelHandle PlayAudioWithSoundID(SoundID soundID, float volumeLevel = 1.f, bool isPaused = false);
	void PlayAudioFromPath(const std::string& soundFileName, float volumeLevel = 1.f, bool loop = false);

	void StopChannel(AudioChannelHandle channel);
	AudioChannelHandle AssignSoundToChannel(FMOD::Sound* sound, float volumeLevel = 1.f, bool isPaused = false);
	void SetPauseOnAudioChannelHandle(AudioChannelHandle audioHandle, bool isPaused);

	AudioChannelHandle SetAudioFromPath(const std::string& soundFileName, float volumenLevel = 1.f, bool loop = false, bool isPaused = false);
	
	void Update();

private:
	bool IsSoundIDValid(const SoundID& soundID);

protected:
	void InitializeFMOD();
	void ShutDownFMOD();
	void ValidateResult(FMOD_RESULT result);

protected:
	FMOD::System*						m_fmodSystem;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;
};

