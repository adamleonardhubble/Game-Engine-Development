/** \file vertexBuffer.h
*/
#pragma once

#include "bufferLayout.h"

namespace Engine
{
	/**
	\class VertexBuffer
	\brief A base interface class for API specific vertex buffer to use
	*/
	class VertexBuffer
	{
	public:
		//! Destructor
		virtual ~VertexBuffer() {}

		virtual void bind() = 0; //!< Bind the vertex buffer object
		virtual void unbind() = 0; //!< Unind the vertex buffer object

		//virtual void edit(float* vertices, unsigned int size, unsigned int offset) = 0;

		//! Used to get the layout of the object being drawn
		/*!
		\return A reference to the buffer layout object
		*/
		virtual const BufferLayout& getLayout() const = 0;

		//! Create a vertex buffer and return a pointer to it
		/*!
		\return A pointer to the new vertex buffer instance
		*/
		static VertexBuffer* create(float* vertices, unsigned int size, BufferLayout& layout);
	};
}