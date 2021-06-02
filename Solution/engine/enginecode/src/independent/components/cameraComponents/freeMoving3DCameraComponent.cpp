/** \file freeMoving3DCameraComponent.cpp
*/
#include "engine_pch.h"

#include "components/cameraComponents/freeMoving3DCameraComponent.h"

#include "core/codes.h"
#include "windows/inputPoller.h"

namespace Engine
{
	FreeMoving3DCameraComponent::FreeMoving3DCameraComponent(float fov, float aspectRatio, float nearClip, float farClip, glm::vec3 position, 
		glm::vec2 rotation, bool updateMousePress) : 
		CameraComponent3D(fov, aspectRatio, nearClip, farClip, position, rotation, updateMousePress)
	{
		// Make a new camera
		m_camera.reset(new PerspectiveEulerCamera3D(fov, aspectRatio, nearClip, farClip));
		m_worldUp = glm::vec3(0.f, 1.f, 0.f); // Initialize the world up axis
		updateView(); // Update the camera view

		m_componentType = ComponentType::Free3DCamera; // Set the component type
	}

	void FreeMoving3DCameraComponent::onAttach(GameObject* owner)
	{
		m_pOwner = owner;

		m_lastMousePosition = InputPoller::getMousePosition();
	}

	void FreeMoving3DCameraComponent::onUpdate(float timestep)
	{
		glm::vec3 pos = m_position;

		// If W, A, S or D is pressed, move forward, left, back or right
		if (InputPoller::isKeyPressed(ENGINE_KEY_W))
			pos += m_forward * m_fTranslationSpeed * timestep;
		if (InputPoller::isKeyPressed(ENGINE_KEY_S))
			pos -= m_forward * m_fTranslationSpeed * timestep;
		if (InputPoller::isKeyPressed(ENGINE_KEY_A))
			pos -= m_right * m_fTranslationSpeed * timestep;
		if (InputPoller::isKeyPressed(ENGINE_KEY_D))
			pos += m_right * m_fTranslationSpeed * timestep;

		m_position = pos;

		// If the left mouse button is pressed, rotate the camera with the mouse movement
		if (m_bMouseButtonPressed || !m_bUpdateOnMousePress)
		{
			glm::vec2 currentMousePosition = InputPoller::getMousePosition();
			glm::vec2 mouseDelta = currentMousePosition - m_lastMousePosition;

			m_fYaw -= mouseDelta.x * m_fRotationSpeed * timestep;
			m_fPitch -= mouseDelta.y * m_fRotationSpeed * timestep;

			// Prevent the camera rotating too far up or down
			if (m_fPitch > 89.f)
				m_fPitch = 89.f;
			if (m_fPitch < -89.f)
				m_fPitch = -89.f;

			m_lastMousePosition = currentMousePosition;
		}

		// Update the camera view
		updateView();

		ComponentMessage msg(ComponentMessageType::PositionSet, (void*)&m_position); // Create Message
		sendMessage(msg); // Send Message
		ComponentMessage msg2(ComponentMessageType::RotationSet, (void*) new glm::vec3(m_fYaw, m_fPitch, 0)); // Create Message
		sendMessage(msg2); // Send Message
	}

	void FreeMoving3DCameraComponent::onEvent(Event& event)
	{
		// Create event dispatcher
		EventDispatcher dispatcher(event);
		// If the Event type matches, call the corresponding function
		dispatcher.dispatch<MouseButtonPressedEvent>(std::bind(&FreeMoving3DCameraComponent::onMouseButtonPressed, this, std::placeholders::_1));
		dispatcher.dispatch<MouseButtonReleasedEvent>(std::bind(&FreeMoving3DCameraComponent::onMouseButtonReleased, this, std::placeholders::_1));
	}

	bool FreeMoving3DCameraComponent::onMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		// The event was the left mouse button being pressed
		if (e.getMouseButton() == ENGINE_MOUSE_BUTTON_LEFT)
		{
			// Set the last mouse position to the current mouse position
			m_lastMousePosition = InputPoller::getMousePosition();
			m_bMouseButtonPressed = true;
			return true;
		}
		else
			return false;
	}

	bool FreeMoving3DCameraComponent::onMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		// The event was the left mouse button being pressed
		if (e.getMouseButton() == ENGINE_MOUSE_BUTTON_LEFT)
		{
			m_bMouseButtonPressed = false;
			return true;
		}
		else
			return false;
	}

	void FreeMoving3DCameraComponent::updateView()
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

	void FreeMoving3DCameraComponent::setPosAndRot(glm::vec3 position, glm::vec2 rotation)
	{
		m_position = position;
		m_fYaw = rotation.y;
		m_fPitch = rotation.x;
	}
}