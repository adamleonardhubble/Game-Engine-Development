/** \file OpenGLUniformBuffer.h
*/
#pragma once

#include "rendering/uniformBuffer.h"

#include "systems/timer.h"

namespace Engine
{
	/** 
	\class OpenGLUniformBuffer
	\brief OpenGL specific uniform buffer, for setting scene data
	*/
	class OpenGLUniformBuffer : public UniformBuffer
	{
	private:
		unsigned int m_id; //!< The ID for the buffer
		unsigned int m_iBindingPoint; //!< The binding point to use
		UniformBufferLayout m_layout; //!< The buffer layout

		static unsigned int s_bindingPoint; //!< The next biding point for a new uniform buffer to use
	public:
		//! Constructor
		/*!
		\param size The size of the data in the buffer
		\param layout The buffer layout
		*/
		OpenGLUniformBuffer(unsigned int size, UniformBufferLayout& layout);
		//! Constructor
		/*!
		\param size The size of the data in the buffer
		\param rangeStart
		\param rangeEnd
		\param layout The buffer layout
		*/
		OpenGLUniformBuffer(unsigned int size, unsigned int rangeStart, unsigned int rangeEnd, UniformBufferLayout& layout);
		~OpenGLUniformBuffer(); //!< Destructor

		void bind() override; //!< Bind the buffer
		void unbind() override; //!< Unbind the buffer

		//! Function to attach the block to the shader
		/*!
		\param pointer to the shader
		\param blockName The name of the block
		*/
		void attachShaderBlock(const std::shared_ptr<Shader>& shader, const std::string& blockName) override;
		//! Function to set the uniform data
		/*!
		\param offset The offset of the data
		\param size The size of the data being set
		\param data Void pointer to the data to be set
		*/
		void setData(unsigned int offset, unsigned int size, void* data) override;

		//! Function to get the buffer layout
		/*!
		\return The buffer layout
		*/
		inline const UniformBufferLayout& getLayout() const override { return m_layout; }
	};
}