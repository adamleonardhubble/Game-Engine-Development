/** \file shader.h
*/
#pragma once

#include "bufferLayout.h"
#include "rendering/uniformElement.h"
#include "rendering/uniformBufferLayout.h"

#include <map>

namespace Engine
{
	//! UniformLayout is a map with a string and a pointer to a uniform object
	using UniformLayout = std::map<std::string, std::shared_ptr<UniformElement>>;
	//! UniformBufferLayouts is a map with a string and a UniformBufferLayout
	using UniformBufferLayouts = std::map<std::string, std::shared_ptr<UniformBufferLayout>>;
	
	/**
	\class Shader
	\brief A base interface class for API specific shader classes to use
	*/
	class Shader
	{
	public:
		//! Destructor
		virtual ~Shader() {}

		//! Function to return the ID
		/*!
		\return The ID of the shader
		*/
		virtual unsigned int id() = 0;

		virtual void bind() = 0; //!< Function to bind the shader
		virtual void unbind() = 0; //!< Function to unbind the shader

		//! Function to upload data to the GPU
		/*!
		\param name The name of the uniform being uploaded
		\param data The data being uploaded
		*/
		virtual bool uploadData(const std::string& name, void* data) = 0;
		//! Function to upload data to the GPU
		/*!
		\param uniforms The uniform data
		*/
		virtual bool uploadData(const UniformLayout& uniforms) = 0;

		//! Function to get the buffer layout
		/*!
		\return The buffer layout
		*/
		virtual BufferLayout getBufferLayout() const = 0;
		//! Function to get the uniform layout
		/*!
		\return The uniform layout
		*/
		virtual UniformLayout getUniformLayout() const = 0;
		//! Function to get the uniform buffer layouts
		/*!
		\return The uniform buffer layouts
		*/
		virtual UniformBufferLayouts getUniformBufferLayouts() const = 0;

		//! Function to create the shader from one file
		/*!
		\param filepath The filepath for where the shader file is saved
		\return A pointer to the shader
		*/
		static Shader* create(const std::string& filepath);
		//! Function to create the shader from separate vertex and fragment files
		/*!
		\param vertexFilepath The filepath for where the vertex shader file is saved
		\param fragmentFilepath The filepath for where the fragment shader file is saved
		\return A pointer to the shader
		*/
		static Shader* create(const std::string& vertexFilepath, const std::string& fragmentFilepath);
	};
}