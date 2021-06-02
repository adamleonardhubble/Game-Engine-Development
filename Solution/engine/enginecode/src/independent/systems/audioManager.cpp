/** \file audioManager.cpp
*/
#include "engine_pch.h"

#include "systems/audioManager.h"

namespace Engine
{
	void AudioManager::start(SystemSignal init, ...)
	{
		//
		//errorCheck(FMOD::Studio::System::create(&m_studioSystem));
	}

	void AudioManager::stop(SystemSignal, ...)
	{

	}

	void AudioManager::update()
	{

	}

	/*
	void AudioManager::loadBank(const std::string& sBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
	{

	}
	*/

	void AudioManager::loadEvent(const std::string& sEventName)
	{

	}

	/*
	void AudioManager::loadSound(const std::string& sSoundName, bool b3d = true, bool bLooping = false, bool bStream = false, float minDist = 0.25f, float maxDist = 10000.f, RollOff rollOff = RollOff::Inverse)
	{

	}
	*/

	void AudioManager::unLoadSound(const std::string &sSoundName)
	{

	}

	void AudioManager::set3dListenerAndOrientation(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up)
	{

	}

	/*
	void AudioManager::addGeometry(const std::string& label, const AudioGeometryDefinition& def)
	{

	}
	*/

	void AudioManager::moveGeometry(const std::string& sLabel, const glm::vec3& position)
	{

	}

	int AudioManager::playSound(const std::string &sSoundName, const glm::vec3& position)
	{
		return 0;
	}
}