/** \file OpenGLIndexBuffer.cpp
*/
#include "engine_pch.h"

#include "platform/openGL/OpenGLIndexBuffer.h"
#include <glad/glad.h>

namespace Engine
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indices, unsigned int count) : m_iCount(count)
	{
#ifdef NG_DEBUG	// Only in debug mode
		TIMER_START("InitialiseIndexBuffers"); // Start 'InitialiseIndexBuffers' timer
#endif

		glGenBuffers(1, &m_iRendererID);
		//glCreateBuffers(1, &m_iRendererID); // Create the index buffer using the render ID as a name
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iRendererID); // Bind the buffer
		// Initialize the data
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_iCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

#ifdef NG_DEBUG	// Only in debug mode
		TIMER_END("InitialiseIndexBuffers"); // Stop 'InitialiseIndexBuffers' timer
#endif
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		// Delete the buffer
		glDeleteBuffers(1, &m_iRendererID);
	}

	void OpenGLIndexBuffer::bind()
	{
		// Bind the buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iRendererID);
	}

	void OpenGLIndexBuffer::unbind()
	{
		// Unbind the buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}