/** \file renderCommand.cpp
*/
#include "engine_pch.h"

#include "rendering/renderAPI.h"
#include "rendering/renderer/renderCommand.h"
#include "platform/openGL/renderer/OpenGLRenderCommands.h"
#include "systems/log.h"

namespace Engine
{
	RenderCommand* RenderCommand::clearDepthColourBufferCommand(bool keepAlive)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLClearDepthColourBufferCommand(keepAlive); // Create and return a pointer to an openGL render command
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}

	RenderCommand* RenderCommand::setClearColourCommand(float r, float g, float b, float a, bool keepAlive)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLSetClearColourCommand(r, g, b, a, keepAlive); // Create and return a pointer to an openGL render command
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}

	RenderCommand* RenderCommand::setDepthTestLessCommand(bool enabled, bool keepAlive)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLSetDepthTestLessCommand(enabled, keepAlive); // Create and return a pointer to an openGL render command
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}

	RenderCommand* RenderCommand::setBackfaceCullingCommand(bool enabled, bool keepAlive)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLSetBackfaceCullingCommand(enabled, keepAlive); // Create and return a pointer to an openGL render command
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}

	RenderCommand* RenderCommand::clearColourBufferCommand(bool keepAlive)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLClearColourBufferCommand(keepAlive); // Create and return a pointer to an openGL render command
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}

	RenderCommand* RenderCommand::setOneMinusAlphaBlendingCommand(bool enabled, bool keepAlive)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLSetOneMinusAlphaBlendingCommand(enabled, keepAlive); // Create and return a pointer to an openGL render command
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}
}