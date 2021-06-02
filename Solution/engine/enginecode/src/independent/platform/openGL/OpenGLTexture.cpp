/** \file OpenGLTexture.cpp
*/
#include "engine_pch.h"

#include "platform/openGL/OpenGLTexture.h"
#include "systems/log.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Engine
{
	OpenGLTexture::OpenGLTexture(const std::string& filepath)
	{
		m_bCubeMap = false;

		// Generate and wrap the texture
		genTex(); 
		wrapTex();

		int width, height, channels; // Ints to be used to get the width, height and the channels
		// Load the texture data
		unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
		// Set the class variables
		m_iWidth = width;
		m_iHeight = height;
		m_iChannels = channels;

		setChannels(data); // Set the channels

		stbi_image_free(data); // Free the loaded image
	}

	OpenGLTexture::OpenGLTexture(std::vector<std::string> filepaths)
	{
		m_bCubeMap = true;

		// Generate and wrap the texture
		genTex();
		wrapTex();

		int width, height, channels; // Ints to be used to get the width, height and the channels
		for (unsigned int i = 0; i < filepaths.size(); i++)
		{
			unsigned char* data = stbi_load(filepaths[i].c_str(), &width, &height, &channels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				LOG_ERROR("Cubemap tex failed to load at path: {0}", filepaths[i]);
				stbi_image_free(data);
			}
		}
	}

	OpenGLTexture::OpenGLTexture(unsigned int width, unsigned int height, unsigned int channels, unsigned char* texData)
		: m_iWidth(width), m_iHeight(height), m_iChannels(channels)
	{
		m_bCubeMap = false;

		// Generate and wrap the texture
		genTex();
		wrapTex();

		setChannels(texData); // Set the channels
		stbi_image_free(texData);
	}

	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &m_iTextureID); // Delete the texture

		s_deletedSlots.push_front(m_iTextureSlot); // Add the value of the texture slot to the list so it can be used again
	}

	void OpenGLTexture::genTex()
	{
		glGenTextures(1, &m_iTextureID); // Generate the texture

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

		glActiveTexture(GL_TEXTURE0 + m_iTextureSlot); // Set the current slot to the active one
		// Bind the texture
		if (m_bCubeMap)
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_iTextureID);
		else
			glBindTexture(GL_TEXTURE_2D, m_iTextureID);

		//LOG_INFO("Texture slots used: {0}", s_iTextureSlot);
	}

	void OpenGLTexture::wrapTex()
	{
		// Set texture parameters
		if (m_bCubeMap)
		{
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}

	void OpenGLTexture::setChannels(unsigned char* data)
	{
		// If there is txture data
		if (data)
		{
			// If there are 3 channels
			if (m_iChannels == 3)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 3); // Set storage mode for 3 channels
				// Define the texture image
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_iWidth, m_iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Set storage mode for 4 channels
			}
			else if (m_iChannels == 4) // If there are 4 channels
			{
				// Define the texture image
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else if (m_iChannels == 1) // If there is 1 channel
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Set storage mode for 1 channel
				// Define the texture image
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_iWidth, m_iHeight, 0, GL_RED, GL_UNSIGNED_BYTE, data);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Set storage mode for 4 channels
			}
			else return; // If the number of channels is not 1, 3 or 4 then return

			// Generate a mipmap
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else return; // If no data then return
	}
}