/** \file vertexArray.h
*/
#pragma once

#include "vertexBuffer.h"
#include "indexBuffer.h"

namespace Engine
{
	/**
	\class VertexArray
	\brief A base interface class for API specific vertex array to use
	*/
	class VertexArray
	{
	public:
		//! Destructor
		virtual ~VertexArray() {}

		virtual void bind() = 0; //!< Bind the vertex array object
		virtual void unbind() = 0; //!< Unind the vertex array object

		//! Add and set the vertex buffer
		/*!
		\param vertexBuffer A reference to a vertex buffer
		*/
		virtual void setVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
		//! Set the index buffer
		/*!
		\param indexBuffer A reference to an index
		*/
		virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

		//! Used to get the pointer to the vertex buffer
		/*!
		\return The pointer to the vertex buffer
		*/
		virtual std::shared_ptr<VertexBuffer> getVertexBuffer() const = 0;
		//! Used to get the pointer to the index buffer
		/*!
		\return The pointer to the index buffer
		*/
		virtual std::shared_ptr<IndexBuffer> getIndexBuffer() const = 0;

		//! Used to get the draw count
		/*!
		\return The draw count
		*/
		virtual unsigned int getDrawCount() const = 0;

		//! Create a vertex array and return a pointer to it
		/*!
		\return A pointer to the new vertex array instance
		*/
		static VertexArray* create();
	};
}