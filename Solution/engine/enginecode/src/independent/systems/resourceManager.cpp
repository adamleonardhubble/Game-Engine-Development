/** \file resourceManager.cpp
*/
#include "engine_pch.h"

#include "systems/resourceManager.h"
#include "load/assimpLoader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Engine
{
	// Initialize the static asset managers
	AssetManager<IndexBuffer> ResourceManager::s_indexBuffers;
	AssetManager<Shader> ResourceManager::s_shaders;
	AssetManager<Texture> ResourceManager::s_textures;
	AssetManager<VertexArray> ResourceManager::s_VAOs;
	AssetManager<VertexBuffer> ResourceManager::s_VBOs;
	AssetManager<Material> ResourceManager::s_materials;
	AssetManager<UniformBuffer> ResourceManager::s_UBOs;
	AssetManager<Model> ResourceManager::s_models;

	std::map<std::pair<std::string, unsigned int>, std::vector<Character>> ResourceManager::s_characters;
	std::shared_ptr<Texture> ResourceManager::s_fontTexture;

	void ResourceManager::stop(SystemSignal close, ...)
	{
		// Call the clear functions for the asset managers
		s_indexBuffers.clear();
		s_shaders.clear();
		s_textures.clear();
		s_VAOs.clear();
		s_VBOs.clear();
		s_materials.clear();
		s_UBOs.clear();
		s_models.clear();
		s_characters.clear();

		s_fontTexture = nullptr;
	}

	std::shared_ptr<IndexBuffer> ResourceManager::addIndexBuffer(const std::string& name, unsigned int* indices, unsigned int count)
	{
		std::shared_ptr<IndexBuffer> temp; // Make a temporary shared pointer
		if (s_indexBuffers.contains(name))
		{
			// Make temp the one that exists
			temp = s_indexBuffers.get(name);
		}
		else
		{
			temp.reset(IndexBuffer::create(indices, count)); // Create a texture on the temporary pointer
			s_indexBuffers.add(name, temp); // Add the texture to the texture asset manager
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<Shader> ResourceManager::addShader(const std::string& filepath)
	{
		std::shared_ptr<Shader> temp; // Make a temporary shared pointer
		const std::string name = parseFilePath(filepath);
		// If the shader already exists
		if (s_shaders.contains(name))
		{
			// Make temp the one that exists
			temp = s_shaders.get(name);
		}
		else
		{
			temp.reset(Shader::create(filepath)); // Create a shader on the temporary pointer
			s_shaders.add(name, temp); // Add the shader to the shader asset manager

			// Get any uniform buffer layouts from the shader just made
			UniformBufferLayouts buffers = temp->getUniformBufferLayouts();

			// If there is a uniform buffer
			if (!buffers.empty())
			{
				// For each element in the map
				for (auto& element : buffers)
				{
					// Add a new uniform buffer
					addUBO(element.first, element.second, name);
				}
			}
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<Shader> ResourceManager::addShader(const std::string& name, const std::string& vertexFilepath, const std::string& fragmentFilepath)
	{
		std::shared_ptr<Shader> temp; // Make a temporary shared pointer
		// If the shader already exists
		if (s_shaders.contains(name))
		{
			// Make temp the one that exists
			temp = s_shaders.get(name);
		}
		else
		{
			temp.reset(Shader::create(vertexFilepath, fragmentFilepath)); // Create a shader on the temporary pointer
			s_shaders.add(name, temp); // Add the shader to the shader asset manager

			// Get any uniform buffer layouts from the shader just made
			UniformBufferLayouts buffers = temp->getUniformBufferLayouts();

			// If there is a uniform buffer
			if (!buffers.empty())
			{
				// For each element in the map
				for (auto& element : buffers)
				{
					// Add a new uniform buffer
					addUBO(element.first, element.second, name);
				}
			}
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<Texture> ResourceManager::addTexture(const std::string& filepath)
	{
		std::shared_ptr<Texture> temp; // Make a temporary shared pointer
		// If the texture already exists
		if (s_textures.contains(filepath))
		{
			// Make temp the one that exists
			temp = s_textures.get(filepath);
		}
		else
		{
			temp.reset(Texture::createFromFile(filepath)); // Create a texture on the temporary pointer
			s_textures.add(filepath, temp); // Add the texture to the texture asset manager
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<Texture> ResourceManager::addCubemap(const std::vector<std::string>& filepaths)
	{
		std::shared_ptr<Texture> temp; // Make a temporary shared pointer
		// If the texture already exists
		if (s_textures.contains(filepaths.front()))
		{
			// Make temp the one that exists
			temp = s_textures.get(filepaths.front());
		}
		else
		{
			temp.reset(Texture::createCubeMap(filepaths)); // Create a texture on the temporary pointer
			s_textures.add(filepaths.front(), temp); // Add the texture to the texture asset manager
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<Texture> ResourceManager::addTexture(const std::string& name, unsigned int width, unsigned int height, unsigned int channels, unsigned char* texData)
	{
		std::shared_ptr<Texture> temp; // Make a temporary shared pointer
		// If the texture already exists
		if (s_textures.contains(name))
		{
			// Make temp the one that exists
			temp = s_textures.get(name);
		}
		else
		{
			temp.reset(Texture::createFromRawData(width, height, channels, texData)); // Create a texture on the temporary pointer
			s_textures.add(name, temp); // Add the texture to the texture asset manager
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<VertexArray> ResourceManager::addVAO(const std::string& name)
	{
		std::shared_ptr<VertexArray> temp; // Make a temporary shared pointer
		if (s_VAOs.contains(name))
		{
			// Make temp the one that exists
			temp = s_VAOs.get(name);
		}
		else
		{
			temp.reset(VertexArray::create()); // Create a texture on the temporary pointer
			s_VAOs.add(name, temp); // Add the texture to the texture asset manager
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<VertexBuffer> ResourceManager::addVBO(const std::string& name, float* vertices, unsigned int size, BufferLayout& layout)
	{
		std::shared_ptr<VertexBuffer> temp; // Make a temporary shared pointer
		if (s_VBOs.contains(name))
		{
			// Make temp the one that exists
			temp = s_VBOs.get(name);
		}
		else
		{
			temp.reset(VertexBuffer::create(vertices, size, layout)); // Create a texture on the temporary pointer
			s_VBOs.add(name, temp); // Add the texture to the texture asset manager
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<Material> ResourceManager::addMaterial(const std::string& name, std::shared_ptr<Shader> shader, std::shared_ptr<VertexArray> VAO)
	{
		std::shared_ptr<Material> temp; // Make a temporary shared pointer
		if (s_materials.contains(name))
		{
			// Make temp the one that exists
			temp = s_materials.get(name);
		}
		else
		{
			temp.reset(Material::create(shader, VAO)); // Create a texture on the temporary pointer
			s_materials.add(name, temp); // Add the texture to the texture asset manager
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<Material> ResourceManager::changeMaterial(const std::string& name, std::shared_ptr<Shader> shader, std::shared_ptr<VertexArray> VAO)
	{
		std::shared_ptr<Material> temp; // Make a temporary shared pointer
		if (s_materials.contains(name))
		{
			temp.reset(Material::create(shader, VAO)); // Create a material on the temporary pointer
			s_materials.replace(name, temp);
		}
		else
		{
			temp.reset(Material::create(shader, VAO)); // Create a texture on the temporary pointer
			s_materials.add(name, temp); // Add the texture to the texture asset manager
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<Material> ResourceManager::addMaterial(const std::string& name, std::shared_ptr<Shader> shader, std::shared_ptr<VertexBuffer> VBO)
	{
		std::shared_ptr<Material> temp; // Make a temporary shared pointer
		if (s_materials.contains(name))
		{
			// Make temp the one that exists
			temp = s_materials.get(name);
		}
		else
		{
			temp.reset(Material::create(shader, VBO)); // Create a texture on the temporary pointer
			s_materials.add(name, temp); // Add the texture to the texture asset manager
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<UniformBuffer> ResourceManager::addUBO(const std::string& name, std::shared_ptr<UniformBufferLayout> layout, const std::string& shaderName)
	{
		std::shared_ptr<UniformBuffer> temp; // Make a temporary shared pointer

		// If the uniform buffer doesn't already exist
		if (!s_UBOs.contains(name))
		{
			// Create a new buffer on the temporary pointer
			temp.reset(UniformBuffer::create(layout->getSize(), *layout));
			s_UBOs.add(name, temp); // Add the buffer to the uniform buffer asset manager
		}
		else // If the uniform buffer already exists
		{
			temp = s_UBOs.get(name); // Set the temporary pointer to the uniform buffer
		}
		temp->attachShaderBlock(s_shaders.get(shaderName), name); // Attach the shader block to the buffer
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<UniformBuffer> ResourceManager::addUBO(const std::string& name, unsigned int stride, UniformBufferLayout layout)
	{
		std::shared_ptr<UniformBuffer> temp; // Make a temporary shared pointer

		// If the uniform buffer doesn't already exist
		if (!s_UBOs.contains(name))
		{
			// Create a new buffer on the temporary pointer
			temp.reset(UniformBuffer::create(stride, layout));
			s_UBOs.add(name, temp); // Add the buffer to the uniform buffer asset manager
		}
		else // If the uniform buffer already exists
		{
			temp = s_UBOs.get(name); // Set the temporary pointer to the uniform buffer
		}
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<UniformBuffer> ResourceManager::addUBO(const std::string& name, unsigned int rangeStart, unsigned int rangeEnd, std::shared_ptr<UniformBufferLayout> layout, const std::string& shaderName)
	{
		std::shared_ptr<UniformBuffer> temp; // Make a temporary shared pointer

		// If the uniform buffer doesn't already exist
		if (!s_UBOs.contains(name))
		{
			// Create a new buffer on the temporary pointer
			temp.reset(UniformBuffer::create(layout->getSize(), rangeStart, rangeEnd, *layout));
			s_UBOs.add(name, temp); // Add the buffer to the uniform buffer asset manager
		}
		else // If the uniform buffer already exists
		{
			temp = s_UBOs.get(name); // Set the temporary pointer to the uniform buffer
		}
		temp->attachShaderBlock(s_shaders.get(shaderName), name); // Attach the shader block to the buffer
		return temp; // Return the temporary pointer
	}

	std::shared_ptr<UniformBuffer> ResourceManager::getUBO(const std::string& name)
	{
		// If the uniform buffer exists
		if (s_UBOs.contains(name))
		{
			return s_UBOs.get(name); // Return the buffer
		}
		else // If the uniform buffer doesn't exist
		{
			LOG_ERROR("Uniform buffer with name '{0}' doesn't exist", name);
		}
	}

	std::shared_ptr<Model> ResourceManager::addModel(const std::string& name)
	{
		std::shared_ptr<Model> temp; // Make a temporary shared pointer
		std::string fileName = parseFilePath(name);
		// If the texture already exists
		if (s_models.contains(fileName))
		{
			// Make temp the one that exists
			temp = s_models.get(fileName);
		}
		else
		{
			temp.reset(new Model); // Create a texture on the temporary pointer
			s_models.add(fileName, temp); // Add the texture to the texture asset manager
			AssimpLoader::loadModel(name, temp);
		}
		return temp; // Return the temporary pointer
	}

	void ResourceManager::populateCharacters(std::unordered_multimap<std::string, unsigned int> fontsAndSizes)
	{
		FT_Library ft;
		FT_Face face;
		// Initialize freetype
		if (FT_Init_FreeType(&ft))
			LOG_CRITICAL("Could not start FreeType");

		unsigned char* texMemory; // Texture memory for characters
		int memH = 1024; // Memory size
		int memW = 1024;
		int usedX = 0;
		int usedY = 0;
		// Allocate and clear memory for the texture
		texMemory = (unsigned char*)malloc(memW * memH);
		memset(texMemory, 0, memW * memH);

		std::map<std::pair<std::string, unsigned int>, std::pair<unsigned int, unsigned int>> maxSize; // Stores the maximum size for characters for each font and size
		// For each size and font
		for (auto& element : fontsAndSizes)
		{
			// Load the font
			if (FT_New_Face(ft, element.first.c_str(), 0, &face))
				LOG_CRITICAL("FreeType coudn't loat font: {0}", element.first);
			// Set the font size
			if (FT_Set_Pixel_Sizes(face, 0, element.second))
				LOG_CRITICAL("FreeType couldn't set font face size of {0}", element.second);
			// Add a value to the map to get the max sizes
			maxSize.insert(std::make_pair(element, std::make_pair(0, 0)));
			// For each ASCII value being used for characters
			for (int i = s_ASCIIStart; i <= s_ASCIIEnd; i++)
			{
				// Load the character
				if (FT_Load_Char(face, i, FT_LOAD_RENDER))
					LOG_CRITICAL("Could not load the character {0}", i);
				// If either of the sizes are bigger than the corresponding stored ones, change the stored one
				maxSize[element].first = std::max(maxSize[element].first, face->glyph->bitmap.width);
				maxSize[element].second = std::max(maxSize[element].second, face->glyph->bitmap.rows);
			}
			FT_Done_Face(face); // Done with the face
		}

		unsigned int biggestInRow = 0;
		// For each size and font
		for (auto& element : fontsAndSizes)
		{
			// Load the font
			if (FT_New_Face(ft, element.first.c_str(), 0, &face))
				LOG_CRITICAL("FreeType coudn't loat font: {0}", element.first);
			// Set the font size
			if (FT_Set_Pixel_Sizes(face, 0, element.second))
				LOG_CRITICAL("FreeType couldn't set font face size of {0}", element.second);
			// Add the font and size to the map
			s_characters.insert(std::make_pair(std::make_pair(parseFilePath(element.first), element.second), std::vector<Character>()));

			// For each ASCII value being used for characters
			for (int i = s_ASCIIStart; i <= s_ASCIIEnd; i++)
			{
				// Load the character
				if (FT_Load_Char(face, i, FT_LOAD_RENDER))
					LOG_CRITICAL("Could not load the character {0}", i);
				// Set the character values and add it to the vector in the map
				s_characters[std::make_pair(parseFilePath(element.first), element.second)].push_back(Character(
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					face->glyph->advance.x,
					glm::vec2((float)usedX / (float)memW, (float)usedY / (float)memH),
					glm::vec2(((float)usedX + (float)face->glyph->bitmap.width) / (float)memW, ((float)usedY + (float)face->glyph->bitmap.rows) / (float)memH)
				));
				// Get the face buffer (texture data)
				unsigned char* buffer = face->glyph->bitmap.buffer;
				// For each row
				for (int i = 0; i < face->glyph->bitmap.rows; i++)
				{
					if (usedY + maxSize[element].second > memH)
					{
						LOG_CRITICAL("Can't fit all characters on the texture, use smaller character sizes and scale");
						return;
					}
					// Copy the row from the buffer to the new texture
					memcpy(texMemory + (((memW * usedY) + (i * memW)) + usedX), buffer + (i * face->glyph->bitmap.width), face->glyph->bitmap.width);
				}
				biggestInRow = glm::max(biggestInRow, maxSize[element].second); // Always be equal to the tallest character that has been added to a row

				// Increase the used x value
				usedX += face->glyph->bitmap.width;
				// If reached the edge
				if (usedX + maxSize[element].first > memW)
				{
					usedX = 0; // Reset used x
					usedY += biggestInRow; // Move down by the size of the tallest character in the row
					biggestInRow = 0;
				}
			}
			FT_Done_Face(face); // Done with the face
		}
		
		FT_Done_FreeType(ft); // Done with freetype
		// Create a texture from the data created
		s_fontTexture.reset(Texture::createFromRawData(memW, memH, 1, texMemory));
	}

	std::shared_ptr<Character> ResourceManager::getCharacter(std::string font, unsigned int size, unsigned int ASCIICode)
	{
		std::map<std::pair<std::string, unsigned int>, std::vector<Character>>::iterator it; // Make an iterator for the map
		std::string fontName = parseFilePath(font);
		it = s_characters.find(std::make_pair(fontName, size)); // Find the key in the map

		// If the key is not in the map
		if (it == s_characters.end())
		{
			// Log an error, cannot find key in the map
			LOG_ERROR("Font: '{0}' of size '{1}' can't be found", fontName, size);
			return nullptr; // Return a null pointer
		}
		else // If the key is in the map
		{
			return std::make_shared<Character>(it->second[ASCIICode - s_ASCIIStart]); // Return the pointer
		}
	}

	std::string ResourceManager::parseFilePath(const std::string& str)
	{
		int i = str.rfind('/', str.length()); // Find the position in the string of the last occurence of '/'
		int j = str.rfind('.', str.length()); // Find the position in the string of the last occurence of '.'
		if (i != std::string::npos && j != std::string::npos) // If '/' and '.' are found
		{
			// Return a string with the name of the file
			return str.substr(i + 1, (str.length() - i) - (str.length() - (j - 1)));
		}
		else // If '/' and '.' were not found
		{
			// Log an error
			LOG_INFO("Problem finding name of file: {0}, using full filepath", str);
			return str; // Return the full filepath
		}
	}
}