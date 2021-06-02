/** \file OpenGLIndexBuffer.h
*/
#pragma once

#include "rendering/indexBuffer.h"

#include "systems/timer.h"

namespace Engine
{
	/**
	\class OpenGLIndexBuffer
	\brief Open GL specific IndexBuffer
	*/
	class OpenGLIndexBuffer : public IndexBuffer
	{
	private:
		unsigned int m_iRendererID; //!< Name for buffer creation and binding
		unsigned int m_iCount; //!< The number of indices
	public:
		//! Constructor
		/*!
		\param indices The indices of the thing being drawn
		\param count The count of the thing being drawn
		*/
		OpenGLIndexBuffer(unsigned int* indices, unsigned int count); 
		~OpenGLIndexBuffer(); //!< Destructor

		void bind() override; //!< Bind the buffer
		void unbind() override; //!< Unbind the buffer

		//! Used to get the count
		/*!
		\return The stored count
		*/
		inline unsigned int getCount() const override { return m_iCount; } 
	};
}