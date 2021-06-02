/** \file playerComponent.cpp
*/
#include "engine_pch.h"

#include "components/playerComponent.h"
#include "core/codes.h"
#include "windows/inputPoller.h"

namespace Engine
{
	void PlayerComponent::onUpdate(float timestep)
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

			// If the left mouse button is pressed, rotate the object with the mouse movement
			if (InputPoller::isMouseButtonPressed(ENGINE_MOUSE_BUTTON_LEFT))
			{
				glm::vec2 currentMousePosition = InputPoller::getMousePosition();
				glm::vec2 mouseDelta = currentMousePosition - m_lastMousePosition;

				m_fYaw -= mouseDelta.x * m_fRotationSpeed * timestep;
				m_fPitch -= mouseDelta.y * m_fRotationSpeed * timestep;

				m_lastMousePosition = currentMousePosition;
			}
			// Update the movement axis
			updateAxis();
			// Send the position and rotation to the position component
			ComponentMessage msg(ComponentMessageType::PositionSet, (void*)&m_position);
			sendMessage(msg);
			ComponentMessage msg2(ComponentMessageType::RotationSet, (void*)&glm::vec3(0, m_fYaw, 0));
			sendMessage(msg2);
		}
	}

	void PlayerComponent::onEvent(Event& e)
	{
		// Create event dispatcher
		EventDispatcher dispatcher(e);
		// If the Event type matches, call the corresponding function
		dispatcher.dispatch<MouseButtonPressedEvent>(std::bind(&PlayerComponent::onMouseButtonPressed, this, std::placeholders::_1));
		dispatcher.dispatch<KeyPressedEvent>(std::bind(&PlayerComponent::onKeyPressed, this, std::placeholders::_1));
	}

	bool PlayerComponent::onMouseButtonPressed(MouseButtonPressedEvent& e)
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

	bool PlayerComponent::onKeyPressed(KeyPressedEvent& e)
	{
		// The event was enter being pressed
		if (e.getKeyCode() == ENGINE_KEY_ENTER)
		{
			m_bUpdate = !m_bUpdate;
			return true;
		}
		else if (e.getKeyCode() == ENGINE_KEY_RIGHT_SHIFT)
		{
			// If currently using the first camera
			if (m_bCam1)
			{
				// If there is a second camera
				if (m_cameras.second != nullptr)
					m_bCam1 = !m_bCam1; // Change which camera is being used
			}
			else // If currently using the second camera
				m_bCam1 = !m_bCam1; // Change which camera is being used

			return true;
		}
		else
			return false;
	}

	void PlayerComponent::updateAxis()
	{
		// Calculate a new forward vector from the yaw and the pitch (always 0)
		glm::vec3 forward;
		forward.x = sin(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
		forward.y = sin(glm::radians(m_fPitch));
		forward.z = cos(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));

		m_forward = glm::normalize(forward);

		// Calculate right vector
		m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
	}
}