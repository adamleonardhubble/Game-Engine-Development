/** \file GLFWInputPoller.cpp
*/

#include "engine_pch.h"
#include "core/application.h"
#include "platform/GLFW/GLFWInputPoller.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	InputPoller* InputPoller::s_pInstance = new GLFWInputPoller(); // Create an instance of the object

	bool GLFWInputPoller::isKeyPressedImpl(int keycode)
	{
		// Get a pointer to the window
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
		int result = glfwGetKey(window, keycode); // Check if the key is currently being pressed
		return result == GLFW_PRESS || result == GLFW_REPEAT; // Return if it is
	}

	bool GLFWInputPoller::isMouseButtonPressedImpl(int button)
	{
#ifdef NG_DEBUG
		ImGuiIO& io = ImGui::GetIO();
		// If mouse is over ImGui window
		if (io.WantCaptureMouse)
		{
			// Return false so don't do anything
			return false;
		}
#endif

		// Get a pointer to the window
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
		int result = glfwGetMouseButton(window, button); // Check if the button is currently being pressed
		return result == GLFW_PRESS; // Return if it is
	}

	glm::vec2 GLFWInputPoller::getMousePositionImpl()
	{
		// Get a pointer to the window
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
		double x; // Variables to get the position
		double y;
		glfwGetCursorPos(window, &x, &y); // Get the mouse position

		return { (float)x, (float)y }; // Return it

	}

	float GLFWInputPoller::getMouseXImpl()
	{
		glm::vec2 pos = getMousePositionImpl(); // Get the mouse position
		return pos.x; // Return the x position
	}

	float GLFWInputPoller::getMouseYImpl()
	{
		glm::vec2 pos = getMousePositionImpl(); // Get the mouse position
		return pos.y; // Return the y position
	}
}