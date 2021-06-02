/** \file indexBuffer.cpp
*/
#include "engine_pch.h"

#include "rendering/renderAPI.h"
#include "rendering/indexBuffer.h"
#include "platform/openGL/OpenGLIndexBuffer.h"
#include "systems/log.h"

namespace Engine
{
	IndexBuffer* IndexBuffer::create(unsigned int* indices, unsigned int size)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLIndexBuffer(indices, size); // Create and return a pointer to an openGL index buffer
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}
}