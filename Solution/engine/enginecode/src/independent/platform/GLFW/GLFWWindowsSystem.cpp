#include "engine_pch.h"
#include "platform/GLFW/GLFWWindowsSystem.h"
#include "core/core.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	void GLFWWindowsSystem::start(SystemSignal init, ...)
	{
		int success = glfwInit(); // Initialize GLFW
		// If initializing glfw is unsuccessful, log an error and throw an exception
		ENGINE_ASSERT(success, "Could not initialise GLFW");

		m_bGLFWInitialized = true; // glfw has been initialized
	}

	void GLFWWindowsSystem::stop(SystemSignal close, ...)
	{
		glfwTerminate(); // Terminate glfw
		m_bGLFWInitialized = false; // glfw is not initialized
	}
}