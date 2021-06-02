/** \file GLFWGraphicsContext.h
*/
#pragma once

#include "windows/graphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>

/**
\class OpenGL_GLFWGraphicsContext
\brief Implementation to a graphics context for openGL and GLFW
*/
class OpenGL_GLFWGraphicsContext : public GraphicsContext
{
private:
	GLFWwindow* m_pWindow; //!< Pointer to the window
public:
	//! Constructor, sets the window pointer to the one passed in
	/*!
	\param window A pointer to a GLFW window
	*/
	OpenGL_GLFWGraphicsContext(GLFWwindow* window) : m_pWindow(window) {}
	void init() override; //!< Initialize object
	void swapBuffers() override; //!< Swap the buffers, update the window
};