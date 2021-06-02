/** \file material.cpp
*/
#include "engine_pch.h"

#include "rendering/renderAPI.h"
#include "rendering/material.h"
#include "platform/openGL/OpenGLMaterial.h"
#include "systems/log.h"

namespace Engine
{
	Material* Material::create(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& VAO)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLMaterial(shader, VAO); // Create and return a pointer to an openGL material
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}

	Material* Material::create(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexBuffer>& VBO)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			LOG_CRITICAL("If using Open GL, use a vertex array rather than vertex buffer"); // Log what's happening
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