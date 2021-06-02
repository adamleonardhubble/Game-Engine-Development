/** \file FPSCameraComponent.cpp
*/
#include "engine_pch.h"

#include "components/cameraComponents/fixed3DCameraComponent.h"

namespace Engine
{
	Fixed3DCameraComponent::Fixed3DCameraComponent(float fov, float aspectRatio, float nearClip, float farClip, glm::vec3 position, glm::vec2 rotation) :
		CameraComponent3D(fov, aspectRatio, nearClip, farClip, position, rotation, false)
	{
		// Make a new camera
		m_camera.reset(new PerspectiveEulerCamera3D(fov, aspectRatio, nearClip, farClip));
		m_worldUp = glm::vec3(0.f, 1.f, 0.f); // Initialize the world up axis
		updateView(); // Update the camera view

		m_componentType = ComponentType::Fixed3DCamera; // Set the component type
	}

	void Fixed3DCameraComponent::updateView()
	{
		// Calculate a new forward vector from the yaw and the pitch
		glm::vec3 forward;
		forward.x = sin(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
		forward.y = sin(glm::radians(m_fPitch));
		forward.z = cos(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));

		m_forward = glm::normalize(forward);

		// Calculate right and up vectors
		m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
		m_up = glm::normalize(glm::cross(m_right, m_forward));

		// Calculate the view
		m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
		// Set the view in the camera
		m_camera->setViewAndPos(m_view, m_position);
	}

	void Fixed3DCameraComponent::setPosAndRot(glm::vec3 position, glm::vec2 rotation)
	{
		m_position = position;
		m_fYaw = rotation.y;
		m_fPitch = rotation.x;
	}

	void Fixed3DCameraComponent::receiveMessage(const ComponentMessage& msg)
	{
		// Create message dispatcher
		MessageDispatcher dispatcher(msg);
		// If the message type matches, call the corresponding function
		dispatcher.dispatch(ComponentMessageType::TransformSend, std::bind(&Fixed3DCameraComponent::setNewTransform, this, std::placeholders::_1));
	}

	void Fixed3DCameraComponent::setNewTransform(void* data)
	{
		std::pair<glm::vec3, std::pair<glm::vec3, glm::vec3>> trans = *(std::pair<glm::vec3, std::pair<glm::vec3, glm::vec3>>*)data;
		setPosAndRot(trans.first, glm::vec2(trans.second.first.x, trans.second.first.y));

		updateView();
	}
}