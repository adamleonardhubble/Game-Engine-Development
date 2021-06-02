/** \file perspectiveEulerCamera3D.cpp
*/
#include "engine_pch.h"

#include "cameras/perspectiveEulerCamera3D.h"

namespace Engine
{
	PerspectiveEulerCamera3D::PerspectiveEulerCamera3D(float fov, float aspectRatio, float nearClip, float farClip)
	{
		// Call reset function
		reset(fov, aspectRatio, nearClip, farClip);
	}

	void PerspectiveEulerCamera3D::reset(float fov, float aspectRatio, float nearClip, float farClip)
	{
		// Set the projection to perspective with the values passed in
		m_projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
		m_view = glm::mat4(1.f); // Initialize the view
		m_viewProjection = m_projection * m_view; // Set the viewProjection
	}
}