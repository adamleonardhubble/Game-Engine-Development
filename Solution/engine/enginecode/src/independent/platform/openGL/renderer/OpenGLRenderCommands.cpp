/** \file OpenGLRenderCommands.cpp
\brief File with implementations for the open GL action commands
*/
#include "engine_pch.h"

#include "platform/openGL/renderer/OpenGLRenderCommands.h"

#include <glad/glad.h>

namespace Engine
{
	void OpenGLClearDepthColourBufferCommand::action()
	{
		// Clear the colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLSetClearColourCommand::action()
	{
		// Clear the window to display a colour
		glClearColor(m_r, m_g, m_b, m_a);
	}

	void OpenGLSetDepthTestLessCommand::action()
	{
		// If it is being enabled
		if (m_bEnabled)
		{
			// Enable depth test
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS); // Set to less
		}
		else // If it is being disabled
		{
			// Disable depth test
			glDisable(GL_DEPTH_TEST);
		}
	}

	void OpenGLSetBackfaceCullingCommand::action()
	{
		// If it is being enabled
		if (m_bEnabled)
		{
			// Enable face culling
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK); // Set to back face culling
		}
		else // If it is being disabled
		{
			// Disable face culling
			glDisable(GL_CULL_FACE);
		}
	}

	void OpenGLClearColourBufferCommand::action()
	{
		// Clear the colour buffer
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void OpenGLSetOneMinusAlphaBlendingCommand::action()
	{
		// If it is being enabled
		if (m_bEnabled)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else // If it is being disabled
		{
			glDisable(GL_BLEND);
		}
	}
}