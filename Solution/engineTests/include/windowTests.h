#pragma once
#include <gtest/gtest.h>
#define private public
// TESTS FOR THE GLFW WINDOW IMPLEMENTATION
#include "events/event.h"
#include "events/keyEvents.h"

#include "platform/GLFW/glfwWindow.h"

// Commented out, tests don't work due to error using GLFWWindowImpl class
//Engine::GLFWWindowImpl* window = new Engine::GLFWWindowImpl(Engine::WindowProperties());

bool worked = false; // If the callback function is set correctly
// Event callback function for window
void eventCallbackFunc(Engine::Event& e)
{
	worked = true;
}
