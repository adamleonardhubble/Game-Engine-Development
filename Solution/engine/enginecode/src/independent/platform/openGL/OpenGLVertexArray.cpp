/** \file OpenGLVertexArray.cpp
*/
#include "engine_pch.h"

#include "platform/openGL/OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Engine
{
	//! Used to get the openGL type of the data type
	/*!
	\param type The type of data being used
	\return The type of data for openGL
	*/
	static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type)
	{
		switch (type) // Which type is it
		{
		case ShaderDataType::Int:        return GL_INT;
		case ShaderDataType::Int2:       return GL_INT;
		case ShaderDataType::Int3:       return GL_INT;
		case ShaderDataType::Int4:       return GL_INT;
		case ShaderDataType::Float:      return GL_FLOAT;
		case ShaderDataType::Float2:     return GL_FLOAT;
		case ShaderDataType::Float3:     return GL_FLOAT;
		case ShaderDataType::Float4:     return GL_FLOAT;
		case ShaderDataType::Mat3:       return GL_FLOAT;
		case ShaderDataType::Mat4:       return GL_FLOAT;
		case ShaderDataType::Bool:       return GL_BOOL;
		}
		return 0;
	}


	OpenGLVertexArray::OpenGLVertexArray()
	{
		// Create the vertex array using the render ID as a name
		glGenVertexArrays(1, &m_iRendererID);
		//glCreateVertexArrays(1, &m_iRendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		// Delete the vertex array
		glDeleteVertexArrays(1, &m_iRendererID);
	}

	void OpenGLVertexArray::bind()
	{
		// Bind the vertex array
		glBindVertexArray(m_iRendererID);
	}

	void OpenGLVertexArray::unbind()
	{
		// Unbind the vertex array
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::setVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
#ifdef NG_DEBUG	// Only in debug mode
		TIMER_START("SetVertexBufferData"); // Start 'SetVertexBufferData' timer
#endif

		// Bind the vertex array
		glBindVertexArray(m_iRendererID);
		vertexBuffer->bind(); // Bind the vertex buffer

		// Get the vertex buffer layout
		BufferLayout layout = vertexBuffer->getLayout();

		// Used as an index for which attributes are being set
		unsigned int index = 0;
		// For each buffer element in the layout
		for (const auto& element : layout)
		{
			// Enable the vertex attribute array specified by index
			glEnableVertexAttribArray(index);
			// Specify the location and data format of the vertex attributes
			glVertexAttribPointer(
				index,
				ShaderDataTypeComponentCount(element.m_dataType),
				ShaderDataTypeToOpenGLType(element.m_dataType),
				element.m_bNormalized ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(const void*)element.m_iOffset);
			index++; // Increment the index
		}
		// Set the vertex buffer
		m_pVertexBuffer = vertexBuffer;

#ifdef NG_DEBUG	// Only in debug mode
		TIMER_END("SetVertexBufferData"); // Stop 'SetVertexBufferData' timer
#endif
	}

	void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
#ifdef NG_DEBUG	// Only in debug mode
		TIMER_START("SetIndexBufferData"); // Start 'SetIndexBufferData' timer
#endif

		// Bind the vertex array
		glBindVertexArray(m_iRendererID);
		indexBuffer->bind(); // Bind the index buffer

		// Set the index buffer
		m_pIndexBuffer = indexBuffer;

#ifdef NG_DEBUG	// Only in debug mode
		TIMER_END("SetIndexBufferData"); // Stop 'SetIndexBufferData' timer
#endif
	}
}