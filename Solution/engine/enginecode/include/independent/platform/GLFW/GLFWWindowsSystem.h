/** \file GLFWWindowsSystem.h
*/
#pragma once

#include "systems/windows.h"

namespace Engine
{
	/**
	\class GLFWWindowsSystem
	\brief GLFW specific WindowsSystem class, used to initialize and terminate glfw
	*/
	class GLFWWindowsSystem : public WindowsSystem
	{
	private:
		bool m_bGLFWInitialized; //!< A boolean for if glfw is initialized
	public:
		void start(SystemSignal init = SystemSignal::None, ...) override; //!< Start the system
		void stop(SystemSignal close = SystemSignal::None, ...) override; //!< Stop the system
	};
}