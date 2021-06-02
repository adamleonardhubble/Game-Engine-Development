/** \file OpenGLVertexBuffer.h
*/
#pragma once

#include "rendering/vertexBuffer.h"

#include "systems/timer.h"

namespace Engine
{
	/**
	\class OpenGLVertexBuffer
	\brief Open GL specific VertexBuffer
	*/
	class OpenGLVertexBuffer : public VertexBuffer
	{
	private:
		unsigned int m_iRendererID; //!< Name for vertex array creation and binding
		BufferLayout m_layout; //!< The layout of the buffer elements
	public:
		//! Constructor
		/*!
		\param vertices The vertices of the object being drawn
		\param size The size of the vertices in bytes
		\param layout A reference to the BufferLayout object for the object being drawn
		*/
		OpenGLVertexBuffer(float* vertices, unsigned int size, BufferLayout& layout);
		~OpenGLVertexBuffer(); //!< Destructor

		void bind() override; //!< Bind the vertex buffer
		void unbind() override; //!< Unbind the vertex buffer

		//void edit(float* vertices, unsigned int size, unsigned int offset) override;

		//! Used to get the layout of the object being drawn
		/*!
		\return A reference to the buffer layout object
		*/
		inline const BufferLayout& getLayout() const override { return m_layout; }
	};
}