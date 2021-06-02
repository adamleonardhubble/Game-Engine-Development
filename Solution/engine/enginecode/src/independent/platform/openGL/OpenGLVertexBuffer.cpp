/** \file OpenGLVertexBuffer.cpp
*/
#include "engine_pch.h"

#include "platform/openGL/OpenGLVertexBuffer.h"

#include <glad/glad.h>

namespace Engine
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, unsigned int size, BufferLayout& layout) : m_layout(layout)
	{
#ifdef NG_DEBUG	// Only in debug mode
		TIMER_START("InitialiseVertexBuffers"); // Start 'InitialiseVertexBuffers' timer
#endif

		// Create the vertex buffer using the render ID as a name
		glGenBuffers(1, &m_iRendererID);
		//glCreateBuffers(1, &m_iRendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_iRendererID); // Bind the buffer
		// Initialize the data
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

#ifdef NG_DEBUG	// Only in debug mode
		TIMER_END("InitialiseVertexBuffers"); // Stop 'InitialiseVertexBuffers' timer
#endif
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		// Delete the buffer
		glDeleteBuffers(1, &m_iRendererID);
	}

	void OpenGLVertexBuffer::bind()
	{
		// Bind the buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_iRendererID);
	}

	void OpenGLVertexBuffer::unbind()
	{
		// Unbind the buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//void OpenGLVertexBuffer::edit(float * vertices, unsigned int size, unsigned int offset)
	//{
	//
	//}
}