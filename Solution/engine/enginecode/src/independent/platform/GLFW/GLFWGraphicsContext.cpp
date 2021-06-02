/** \file GLFWGraphicsContext.cpp
*/
#include "engine_pch.h"
#include "platform/GLFW/GLFWGraphicsContext.h"
#include "core/core.h"

void OpenGL_GLFWGraphicsContext::init()
{
	// Make the context of the window current
	glfwMakeContextCurrent(m_pWindow);
	// Load glad
	int result = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// If loading glad is unsuccessful, log an error and throw an exception
	ENGINE_ASSERT(result, "Could not initialize GLAD");

	ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
	ImGui_ImplOpenGL3_Init("#version 440");
}

void OpenGL_GLFWGraphicsContext::swapBuffers()
{
	glfwSwapBuffers(m_pWindow); // Swap the window buffers
}