/** \file uniformBuffer.cpp
*/
#include "engine_pch.h"

#include "rendering/renderAPI.h"
#include "rendering/uniformBuffer.h"
#include "platform/openGL/OpenGLUniformBuffer.h"
#include "systems/log.h"

namespace Engine
{
	UniformBuffer* UniformBuffer::create(unsigned int size, UniformBufferLayout& layout)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLUniformBuffer(size, layout); // Create and return a pointer to an openGL uniform buffer
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}

	UniformBuffer* UniformBuffer::create(unsigned int size, unsigned int rangeStart, unsigned int rangeEnd, UniformBufferLayout& layout)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLUniformBuffer(size, rangeStart, rangeEnd, layout); // Create and return a pointer to an openGL uniform buffer
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}
}