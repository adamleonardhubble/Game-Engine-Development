/** \file uniformBuffer.h
*/
#pragma once

#include "rendering/shader.h"
#include "rendering/uniformBufferLayout.h"

namespace Engine
{
	/**
	\class UniformBuffer
	\brief Base class for uniform buffer objects
	*/
	class UniformBuffer
	{
	public:
		//! Destructor
		virtual ~UniformBuffer() {}

		virtual void bind() = 0; //!< Bind the buffer
		virtual void unbind() = 0; //!< Unbind the buffer;

		//! Function to attach the block to the shader
		/*!
		\param pointer to the shader
		\param blockName The name of the block
		*/
		virtual void attachShaderBlock(const std::shared_ptr<Shader>& shader, const std::string& blockName) = 0;
		//! Function to set the uniform data
		/*!
		\param offset The offset of the data
		\param size The size of the data being set
		\param data Void pointer to the data to be set
		*/
		virtual void setData(unsigned int offset, unsigned int size, void* data) = 0;
		//! Function to get the buffer layout
		/*!
		\return The buffer layout
		*/
		virtual const UniformBufferLayout& getLayout() const = 0;

		//! Function to create a uniform buffer object
		/*!
		\param size The size of the data in the buffer
		\param layout The buffer layout
		*/
		static UniformBuffer* create(unsigned int size, UniformBufferLayout& layout);
		//! Function to create a uniform buffer object
		/*!
		\param size The size of the data in the buffer
		\param rangeStart
		\param rangeEnd
		\param layout The buffer layout
		*/
		static UniformBuffer* create(unsigned int size, unsigned int rangeStart, unsigned int rangeEnd, UniformBufferLayout& layout);
	};
}