/** \file indexBuffer.h
*/
#pragma once

namespace Engine
{
	/**
	\class IndexBuffer
	\brief A base interface class for API specific index buffers to use
	*/
	class IndexBuffer
	{
	public:
		//! Destructor
		virtual ~IndexBuffer() {}

		virtual void bind() = 0; //!< Bind the buffer
		virtual void unbind() = 0; //!< Unbind the buffer

		//! Used to get the count
		/*!
		\return The stored count
		*/
		virtual unsigned int getCount() const = 0;

		//! Create an index buffer and return a pointer to it
		/*!
		\param indices The indices of the thing being drawn
		\param count The number of indices
		\return A pointer to the new index buffer instance
		*/
		static IndexBuffer* create(unsigned int* indices, unsigned int count);
	};
}