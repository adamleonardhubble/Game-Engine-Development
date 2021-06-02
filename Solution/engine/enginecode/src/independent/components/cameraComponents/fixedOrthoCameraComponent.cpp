/** \file fixedOrthoCameraComponent.cpp
*/
#include "engine_pch.h"

#include "components/cameraComponents/fixedOrthoCameraComponent.h"

namespace Engine
{
	FixedOrthoCameraComponent::FixedOrthoCameraComponent(float left, float top, float width, float height) :
		CameraComponent2D(left, top, width, height)
	{
		m_camera.reset(new OrthographicCamera2D(left, left + width, top + height, top));

		m_position = glm::vec2(left, top); // Set the position using values passed in
		m_fRotation = 0;

		m_camera->setPositionAndRotation(m_position, m_fRotation);
	}

	void FixedOrthoCameraComponent::setPosAndRot(glm::vec3 position, glm::vec2 rotation)
	{
		m_position = glm::vec2(position.x, position.y);

		m_camera->setPositionAndRotation(m_position, m_fRotation);
	}
}