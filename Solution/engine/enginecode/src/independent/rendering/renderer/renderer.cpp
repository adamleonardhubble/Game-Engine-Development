/** \file renderer.cpp
*/

#include "engine_pch.h"

#include "rendering/renderAPI.h"
#include "rendering/renderer/renderer.h"
#include "platform/openGL/renderer/OpenGLSuperSimpleRenderer.h"
#include "platform/openGL/renderer/OpenGLBasicRenderer.h"
#include "platform/openGL/renderer/OpenGLTextRenderer.h"
#include "systems/log.h"

namespace Engine
{
	Renderer* Renderer::createSimple3D()
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLSuperSimpleRenderer(); // Create and return a pointer to an openGL renderer
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}
	Renderer* Renderer::createBasic3D()
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLBasicRenderer(); // Create and return a pointer to an openGL renderer
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}

		return nullptr;
	}
	Renderer* Renderer::createText()
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLTextRenderer(); // Create and return a pointer to an openGL renderer
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}

		return nullptr;
	}
}