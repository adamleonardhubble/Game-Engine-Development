/** \file shader.cpp
*/
#include "engine_pch.h"

#include "rendering/renderAPI.h"
#include "rendering/shader.h"
#include "platform/openGL/OpenGLShader.h"
#include "systems/log.h"

namespace Engine
{
	Shader* Shader::create(const std::string& filepath)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLShader(filepath); // Create and return a pointer to an openGL shader class
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}

	Shader* Shader::create(const std::string& vertexFilepath, const std::string& fragmentFilepath)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLShader(vertexFilepath, fragmentFilepath); // Create and return a pointer to an openGL shader class
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}
}