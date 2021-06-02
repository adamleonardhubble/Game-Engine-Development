/** \file vertexBuffer.cpp
*/

#include "engine_pch.h"	// Include precompiled headers

#include "rendering/renderAPI.h" // Include 'renderAPI.h' file
#include "rendering/frameBuffer.h" // Include 'frameBuffer.h' file

#include "platform/openGL/OpenGLFrameBuffer.h" // Include 'OpenGLFrameBuffer.h' file

#include "systems/log.h" // Include 'log.h' file

namespace Engine // Engine namespace
{
	FrameBuffer* FrameBuffer::create() // Create a frame buffer object
	{
		switch (RenderAPI::getAPI()) // Switch case statement, return a render API
		{
		case RenderAPI::API::None: // If the render API is 'none'
		{
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening

			break; // Break
		}
		case RenderAPI::API::OpenGL: // If the render API is 'OpenGL'
		{
			return new OpenGLFrameBuffer(); // Create and return a pointer to an OpenGL frame buffer
			
			break; // Break
		}
		case RenderAPI::API::Direct3D: // If the render API is 'Direct3D'
		{
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening

			break; // Break
		}
		default: // If not recognised
		{
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
		}
	}
}