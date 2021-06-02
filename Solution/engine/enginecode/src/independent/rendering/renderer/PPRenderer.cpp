/** \file PPrenderer.cpp
*/

#include "engine_pch.h" // Include precompiled headers

#include "rendering/renderAPI.h" // Include 'renderAPI.h' file

#include "rendering/renderer/PPrenderer.h" // Include 'PPrenderer.h' file

#include "platform/openGL/renderer/OpenGLPPRenderer.h" // Include 'OpenGLPPRenderer.h' file

#include "systems/log.h" // Include 'log.h' file

namespace Engine // Engine namespace
{
	PPRenderer* PPRenderer::createPPRenderer(std::shared_ptr<Shader> shader) // Create a post-processing renderer
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
			return new OpenGLPPRenderer(shader); // Create and return a pointer to an OpenGL renderer

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

		return nullptr; // Return 'nullptr'
	}
}