/** \file resourceManager.h
*/
#pragma once

#include "system.h"
#include "core/assetManager.h"
#include "core/assetIncludes.h"

namespace Engine
{
	/**
	\class ResourceManager
	\brief Manages all asset managers
	*/
	class ResourceManager : public System
	{
	private:
		static AssetManager<IndexBuffer> s_indexBuffers; //!< Index buffer asset manager
		static AssetManager<Shader> s_shaders; //!< Shader asset manager
		static AssetManager<Texture> s_textures; //!< Texture asset manager
		static AssetManager<VertexArray> s_VAOs; //!< Vertex array asset manager
		static AssetManager<VertexBuffer> s_VBOs; //!< Vertex buffer asset manager
		static AssetManager<Material> s_materials; //!< Material asset manager
		static AssetManager<UniformBuffer> s_UBOs; //!< Uniform buffer asset manager
		static AssetManager<Model> s_models; //!< Model asset manager

		static std::map<std::pair<std::string, unsigned int>, std::vector<Character>> s_characters; //!< Stores all loaded characters in all fonts and sizes
		static const int s_ASCIIStart = 32; //!< First ascii value being used ' '
		static const int s_ASCIIEnd = 126; //!< Last ascii value being used '~'
		static std::shared_ptr<Texture> s_fontTexture; //!< Pointer to the texture with all of the characters on it
	public:
		ResourceManager() {} //!< Constructor

		//! Function to start the manager
		void start(SystemSignal init = SystemSignal::None, ...) override {}
		//! Function to stop the manager
		void stop(SystemSignal close = SystemSignal::None, ...) override;

		//! Function to add an index buffer
		/*!
		\param name The name of the buffer
		\param indices The indices of the thing being drawn
		\param count The number of indices
		\return A pointer to the buffer
		*/
		static std::shared_ptr<IndexBuffer> addIndexBuffer(const std::string& name, unsigned int* indices, unsigned int count);
		//! Function to add a shader
		/*!
		\param filepath The file path for where the shader file is
		\return A pointer to the shader
		*/
		static std::shared_ptr<Shader> addShader(const std::string& filepath);
		//! Function to add a shader
		/*!
		\param name The name of the buffer
		\param vertexFilepath The file path for where the vertex shader file is
		\param fragmentFilepath The file path for where the fragment shader file is
		\return A pointer to the shader
		*/
		static std::shared_ptr<Shader> addShader(const std::string& name, const std::string& vertexFilepath, const std::string& fragmentFilepath);
		//! Function to add a texture
		/*!
		\param filepath The file path for where the texture is
		\return A pointer to the texture
		*/
		static std::shared_ptr<Texture> addTexture(const std::string& filepath);
		//! Function to add a cubemap texture
		/*!
		\param filepaths The file paths for where the textures are
		\return A pointer to the cubemap
		*/
		static std::shared_ptr<Texture> addCubemap(const std::vector<std::string>& filepaths);
		//! Function to add a texture
		/*!
		\param name The name of the shader
		\param width The width of the texture
		\param height The height of the texure
		\param channels The number of channels
		\param texData The texture data
		\return A pointer to the texture
		*/
		static std::shared_ptr<Texture> addTexture(const std::string& name, unsigned int width, unsigned int height, unsigned int channels, unsigned char* texData);
		//! Function to add a vertex array
		/*!
		\param name The name of the array
		\return A pointer to the array
		*/
		static std::shared_ptr<VertexArray> addVAO(const std::string& name);
		//! Function to add a vertex buffer
		/*!
		\param name The name of the buffer
		\param vertices The number of vertices
		\param size The size in memory of the vertices
		\param layout The buffer layout
		\return A pointer to the buffer
		*/
		static std::shared_ptr<VertexBuffer> addVBO(const std::string& name, float* vertices, unsigned int size, BufferLayout& layout);

		//! Function to add a material
		/*!
		\param name The name of the material
		\param shader Pointer to the materials shader
		\param VAO Pointer to a vertex array
		\return A pointer to the material
		*/
		static std::shared_ptr<Material> addMaterial(const std::string& name, std::shared_ptr<Shader> shader, std::shared_ptr<VertexArray> VAO);
		//! Function to change a material
		/*!
		\param name The name of the material
		\param shader Pointer to the materials shader
		\param VAO Pointer to a vertex array
		\return A pointer to the material
		*/
		static std::shared_ptr<Material> changeMaterial(const std::string& name, std::shared_ptr<Shader> shader, std::shared_ptr<VertexArray> VAO);
		//! Function to add a material
		/*!
		\param name The name of the material
		\param shader Pointer to the materials shader
		\param VBO Pointer to a vertex buffer
		\return A pointer to the material
		*/
		static std::shared_ptr<Material> addMaterial(const std::string& name, std::shared_ptr<Shader> shader, std::shared_ptr<VertexBuffer> VBO);

		//! Function to add a uniform buffer
		/*!
		\param name The name of the buffer
		\param layout Pointer to the buffer layout
		\param shaderName The name of the shader the layout came from
		\return A pointer to the buffer
		*/
		static std::shared_ptr<UniformBuffer> addUBO(const std::string& name, std::shared_ptr<UniformBufferLayout> layout, const std::string& shaderName);
		//! Function to manually add a uniform buffer
		/*!
		\param name The name of the buffer
		\param stride The stride of the buffer layout
		\param layout The buffer layout
		*/
		static std::shared_ptr<UniformBuffer> addUBO(const std::string& name, unsigned int stride, UniformBufferLayout layout);

		//! Function to add a uniform buffer
		/*!
		\param name The name of the buffer
		\param rangeStart
		\param rangeEnd
		\param layout Pointer to the buffer layout
		\param shaderName The name of the shader the layout came from
		\return A pointer to the buffer
		*/
		static std::shared_ptr<UniformBuffer> addUBO(const std::string& name, unsigned int rangeStart, unsigned int rangeEnd, std::shared_ptr<UniformBufferLayout> layout, const std::string& shaderName);

		//! Function to get a uniform buffer
		/*!
		\param name The name of the buffer
		\return A pointer to a uniform buffer
		*/
		static std::shared_ptr<UniformBuffer> getUBO(const std::string& name);

		//! Function to add and return a model
		/*!
		\param name The name of the model
		\return A pointer to the model
		*/
		static std::shared_ptr<Model> addModel(const std::string& name);

		//! Function to create a texture with all characters that can be used
		/*!
		\param fontsAndSizes A map of fonts and sizes to load characters in
		*/
		static void populateCharacters(std::unordered_multimap<std::string, unsigned int> fontsAndSizes);
		//! Function to get a character
		/*!
		\param font The font of the character being got
		\param size The size of the character
		\param ASCIICode Which character
		\return A pointer to a character
		*/
		static std::shared_ptr<Character> getCharacter(std::string font, unsigned int size, unsigned int ASCIICode);
		//! Function to get the character texture
		/*!
		\return a pointer to the character texture
		*/
		inline static std::shared_ptr<Texture> getFontTexture() { return s_fontTexture; }

		//! Function to get the name of a file from a file path
		/*!
		\param str The file path
		\return The name of the file
		*/
		static std::string parseFilePath(const std::string& str);
	};
}