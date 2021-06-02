/** \file audioManager.h
*/

// Currently a bunch of stuff is commented out because FMOD API not added (can't install without admin privilege)

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <map>

#include "system.h"

namespace Engine
{
	class AudioManager : public System
	{
	private:
		//! Pointer to the studio system
		//FMOD::Studio::System* m_pStudioSystem;

		//! Pointer to the low level system
		//FMOD::System* m_pLowLevelSystem;

		//! Map of sounds
		//std::map<std::string, FMOD::Sound*> m_sounds;

		//! Map of channels
		//std::map<int, FMOD::Channel*> m_channels;

		//! Map of geometry ??? Occlusion geometry?
		//std::map<std::string, FMOD::Geometry*> m_geometry;

		//! Map of events
		//std::map<std::string, FMOD::Studio::EventInstance*> m_events;

		//! Map of banks ???
		//std::map<std::string, FMOD::Studio::Bank*> m_banks;

		//! Check for errors
		/*!
		\param result Result to check ???
		*/
		//int errorCheck(FMOD_RESULT result) const;

		//! Convert GLM vectors to FMOD vectors
		/*!
		\param vec Vector to convert
		*/
		//FMOD_VECTOR GLMVecToFMOD(const glm::vec3& vec);

	public:
		//! Start the system
		/*!
		\param init Init signal
		*/
		void start(SystemSignal init = SystemSignal::None, ...);

		//! Stop the system
		/*!
		\param close Close signal
		*/
		void stop(SystemSignal close = SystemSignal::None, ...);

		//! Update every frame
		void update();

		//! Load FMOD bank
		/*!
		\param sBankName Name of the bank to load
		\param flags ???
		*/
		//void loadBank(const std::string& sBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);

		//! Load FMOD event
		/*!
		\param sEventName Name of the event to load
		*/
		void loadEvent(const std::string& sEventName);

		//! Load sound
		/*!
		\param sSoundName Name of the sound to load
		\param b3d ???
		\param bLooping Is the sound looping, defaults to false
		\param bStream ???
		\param minDist Minimum distance sound may travel, defaults to 0.25f
		\param maxDist Maximum distance sound may travel, defaults to 10000.f
		\param rollOff ???, defaults to 'inverse'
		*/
		//void loadSound(const std::string& sSoundName, bool b3d = true, bool bLooping = false, bool bStream = false, float minDist = 0.25f, float maxDist = 10000.f, RollOff rollOff = RollOff::Inverse);

		//! Unload sound
		/*!
		\param sSoundName Name of the sound to unload
		*/
		void unLoadSound(const std::string &sSoundName);

		//! ???
		/*!
		\param position ???
		\param forward ???
		\param up ???
		*/
		void set3dListenerAndOrientation(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up);

		//! ???
		/*!
		\param label ???
		\param def ???
		*/
		// void addGeometry(const std::string& label, const AudioGeometryDefinition& def);
		
		//! ???
		/*!
		\param label
		\param position
		*/
		void moveGeometry(const std::string& sLabel, const glm::vec3& position);

		//! Play a sound at a position ???, defaults at center of world
		/*!
		\param strSoundName Name of the sound
		\param vPos Position to play sound from ???
		*/
		int playSound(const std::string& sSoundName, const glm::vec3& position = glm::vec3{ 0, 0, 0 });

		//! Play an event ???
		/*!
		\param sEventName Name of the event being played
		*/
		void playEvent(const std::string& sEventName);

		//! Toggle pause on a specified channel
		/*!
		\param iChannelID ID of the channel having pause toggled
		*/
		void toggleChannelPause(int iChannelID);

		//! Stop an event ???
		/*!
		\param sEventName Name of the event being stopped
		\param bImmediate ???
		*/
		void stopEvent(const std::string& sEventName, bool bImmediate = false);

		//! ???
		/*!
		\param sEventName Name of event
		\param sEventParameter ???
		\param fpValue ???
		*/
		void getEventParameter(const std::string& sEventName, const std::string& sEventParameter, float* fpValue);

		//! ???
		/*!
		\param sEventName Name of event
		\param sParameterName ???
		\param fValue ???
		*/
		void setEventParameter(const std::string& sEventName, const std::string& sParameterName, float fValue);

		//! Set position of event
		/*!
		\param sEventName Name of event
		\param position Position to set
		*/
		void setEventPosition(const std::string& sEventName, const glm::vec3& position);

		//! Toggle pause for all channels
		void togglePauseAllChannels();

		//! ???
		/*!
		\param iChannelID ???
		\param position ???
		*/
		void setChannel3dPosition(int iChannelID, const glm::vec3& position);

		//! ???
		/*
		\param iChannelID
		*/
		bool isPlaying(int iChannelID) const;

		//! ???
		/*
		\param sEventName Name of event
		*/
		bool isEventPlaying(const std::string &sEventName) const;
	};
}