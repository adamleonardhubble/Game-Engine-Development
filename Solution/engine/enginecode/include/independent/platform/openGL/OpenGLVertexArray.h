/** \file OpenGLVertexArray.h
*/
#pragma once

#include "rendering/vertexArray.h"

#include "systems/timer.h"

namespace Engine
{
	/**
	\class OpenGLVertexArray
	\brief Open GL specific VertexArray
	*/
	class OpenGLVertexArray : public VertexArray
	{
	private:
		unsigned int m_iRendererID; //!< Name for vertex array creation and binding

		std::shared_ptr<VertexBuffer> m_pVertexBuffer; //!< Pointer to a VertexBuffer class
		std::shared_ptr<IndexBuffer> m_pIndexBuffer; //!< Pointer to an IndexBuffer class
	public:
		OpenGLVertexArray(); //!< Constructor
		~OpenGLVertexArray(); //!< Destructor

		void bind() override; //!< Bind the vertex array object
		void unbind() override; //!< Unind the vertex array object

		//! Add and set the vertex buffer
		/*!
		\param vertexBuffer A reference to a vertex buffer which will be stored with m_pVertexBuffer
		*/
		void setVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override; 
		//! Set the index buffer
		/*!
		\param indexBuffer A reference to an index buffer which will be stored with m_pIndexBuffer
		*/
		void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override; 

		//! Used to get the pointer to the vertex buffer
		/*!
		\return The pointer to the vertex buffer
		*/
		inline std::shared_ptr<VertexBuffer> getVertexBuffer() const override { return m_pVertexBuffer; } 
		//! Used to get the pointer to the index buffer
		/*!
		\return The pointer to the index buffer
		*/
		inline std::shared_ptr<IndexBuffer> getIndexBuffer() const override { return m_pIndexBuffer; }

		//! Used to get the draw count
		/*!
		\return The draw count
		*/
		inline unsigned int getDrawCount() const override { return m_pIndexBuffer->getCount(); } 
	};
}