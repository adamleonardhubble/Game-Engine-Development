/** \file orthographicCamera2D.cpp
*/
#include "engine_pch.h"

#include "cameras/orthographicCamera2D.h"

namespace Engine
{
	OrthographicCamera2D::OrthographicCamera2D(float left, float right, float bottom, float top)
	{
		// Call the reset function
		reset(left, right, bottom, top);
	}

	void OrthographicCamera2D::reset(float left, float right, float bottom, float top)
	{
		// Set the projection to orthographic using the values passed in
		m_projection = glm::ortho(left, right, bottom, top);
		m_view = glm::mat4(1.f); // Initialize the view
		m_viewProjection = m_projection * m_view; // Set the viewProjection
	}

	void OrthographicCamera2D::updateView()
	{
		// Calculate translation and rotation matrices
		glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(m_position, 1.f));
		glm::mat4 rotate = glm::rotate(glm::mat4(1.f), glm::radians(m_fRotation), glm::vec3(0.f, 0.f, 1.f));
		m_view = translate * rotate; // Set the view

		m_viewProjection = m_projection * m_view; // Calculate the viewProjection
	}
}