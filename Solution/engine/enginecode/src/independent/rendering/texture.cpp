/** \file texture.cpp
*/
#include "engine_pch.h"

#include "rendering/renderAPI.h"
#include "rendering/texture.h"
#include "platform/openGL/OpenGLTexture.h"
#include "systems/log.h"

namespace Engine
{
	// Initialize static variables
	unsigned int Texture::s_iTextureSlot = 0;
	std::list<unsigned int> Texture::s_deletedSlots = std::list<unsigned int>();

	Texture* Texture::createFromFile(const std::string& filepath)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLTexture(filepath); // Create and return a pointer to an openGL texture class
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}

	Texture* Texture::createCubeMap(std::vector<std::string> filepaths)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLTexture(filepaths); // Create and return a pointer to an openGL texture class
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}

	Texture* Texture::createFromRawData(unsigned int width, unsigned int height, unsigned int channels, unsigned char* texData)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None: // If no render API
			LOG_CRITICAL("Lack of graphics API not supported"); // Log what's happening
			break;
		case RenderAPI::API::OpenGL: // If using openGL
			return new OpenGLTexture(width, height, channels, texData); // Create and return a pointer to an openGL texture class
			break;
		case RenderAPI::API::Direct3D: // If using direct3D
			LOG_CRITICAL("Direct 3D not supported"); // Log what's happening
			break;
		default: // If not recognised
			LOG_CRITICAL("Unknown graphics API"); // Log what's happening
		}
	}

	unsigned int Texture::assignTextureSlot()
	{
		unsigned int m_iTextureSlot; // Store the texture slot

		// If the list of available slots is empty
		if (s_deletedSlots.empty())
		{
			// Set the texture slot to the static one
			m_iTextureSlot = s_iTextureSlot;
			s_iTextureSlot++; // Increase the static one
		}
		else // If there is an available slot in the list
		{
			// Set the texture slot to the front one in the list
			m_iTextureSlot = s_deletedSlots.front();
			s_deletedSlots.pop_front(); // Remove the front one from the list
		}

		return m_iTextureSlot; // Return the current texture slot
	}

	void Texture::clearTextureSlot(unsigned int slot)
	{
		s_deletedSlots.push_front(slot);
	}
}