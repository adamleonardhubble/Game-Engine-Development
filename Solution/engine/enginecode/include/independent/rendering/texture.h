/** \file texture.h
*/
#pragma once

#include <string>

#include <list>
#include <vector>

namespace Engine
{
	/**
	\class Texture
	\brief A base interface class for API specific texture classes to use
	*/
	class Texture
	{
	protected:
		static unsigned int s_iTextureSlot; //!< The next texture slot to use
		static std::list<unsigned int> s_deletedSlots; //!< List of slots which were used but are no longer being used so can be used again

		bool m_bCubeMap; //!< If it is a cubemap or not
	public:
		//! Destructor
		virtual ~Texture() {}

		//! Function to get the width of the texture
		/*!
		\return The width of the texture
		*/
		virtual unsigned int getWidth() const = 0;

		//! Function to get the height of the texture
		/*!
		\return The height of the texture
		*/
		virtual unsigned int getHeight() const = 0;

		//! Function to get the channels of the texture
		/*!
		\return The channels of the texture
		*/
		virtual unsigned int getChannels() const = 0;

		//! Function to get the texture slot
		/*!
		\return The texture slot
		*/
		virtual unsigned int getSlot() const = 0;

		//! Function to get the texture id
		/*!
		\return The texture id
		*/
		virtual unsigned int getID() const = 0;

		//!	Function to get the current texture slot
		/*!
		\return The current texture slot
		*/
		static unsigned int assignTextureSlot();

		//! Function to clear the texture slot given
		/*!
		\param slot The texture slot to free
		*/
		static void clearTextureSlot(unsigned int slot);

		//! Function to create the texture from a file
		/*!
		\param filepath The filepath for where the texture is saved
		\return A pointer to the texture
		*/
		static Texture* createFromFile(const std::string& filepath);
		//! Function to create the cubeMap texture from a file
		/*!
		\param filepaths The filepaths for where the textures are saved
		\return A pointer to the texture
		*/
		static Texture* createCubeMap(std::vector<std::string> filepaths);

		//! Function to create the texture from data
		/*!
		\param width The width of the texture
		\param height The height of the texure
		\param channels The number of channels
		\param texData The texture data
		\return A pointer to the texture
		*/
		static Texture* createFromRawData(unsigned int width, unsigned int height, unsigned int channels, unsigned char* texData);
	};
}