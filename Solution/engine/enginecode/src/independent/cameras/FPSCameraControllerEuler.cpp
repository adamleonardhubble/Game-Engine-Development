/** \file FPSCameraControllerEuler.cpp
*/
#include "engine_pch.h"

#include "cameras/FPSCameraControllerEuler.h"
#include "windows/inputPoller.h"
#include "core/codes.h"

namespace Engine
{
	void FPSCameraControllerEuler::init(float fov, float aspectRatio, float nearClip, float farClip, glm::vec3 position, glm::vec2 rotation)
	{
		// Make a new camera
		m_camera.reset(new PerspectiveEulerCamera3D(fov, aspectRatio, nearClip, farClip));
		m_position = position; // Initialize the position
		m_worldUp = glm::vec3(0.f, 1.f, 0.f); // Initialize the world up axis
		m_fYaw = rotation.x; // Initialize the yaw
		m_fPitch = rotation.y; // Initialize the pitch
		updateView(); // Update the camera view
	}

	void FPSCameraControllerEuler::onUpdate(float timestep)
	{
		if (m_bUpdate)
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

			// Keep the y position the same
			pos.y = m_position.y;
			m_position = pos;

			// If the left mouse button is pressed, rotate the camera with the mouse movement
			if (InputPoller::isMouseButtonPressed(ENGINE_MOUSE_BUTTON_LEFT))
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
		}
	}

	void FPSCameraControllerEuler::onEvent(Event& event)
	{
		// Create event dispatcher
		EventDispatcher dispatcher(event);
		// If the Event type matches, call the corresponding function
		dispatcher.dispatch<WindowResizeEvent>(std::bind(&FPSCameraControllerEuler::onResize, this, std::placeholders::_1));
		dispatcher.dispatch<MouseButtonPressedEvent>(std::bind(&FPSCameraControllerEuler::onMouseButtonPressed, this, std::placeholders::_1));
		dispatcher.dispatch<KeyPressedEvent>(std::bind(&FPSCameraControllerEuler::onKeyPressed, this, std::placeholders::_1));
	}

	bool FPSCameraControllerEuler::onMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		// The event was the left mouse button being pressed
		if (e.getMouseButton() == ENGINE_MOUSE_BUTTON_LEFT)
		{
			// Set the last mouse position to the current mouse position
			m_lastMousePosition = InputPoller::getMousePosition();
			return true;
		}
		else
			return false;
	}

	bool FPSCameraControllerEuler::onKeyPressed(KeyPressedEvent& e)
	{
		// The event was the enter being pressed
		if (e.getKeyCode() == ENGINE_KEY_ENTER)
		{
			m_bUpdate = !m_bUpdate;
			return true;
		}
		else
			return false;
	}

	void FPSCameraControllerEuler::updateView()
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
}
