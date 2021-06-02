/** \file rotateComponent.cpp
*/
#include "engine_pch.h"

#include "components/rotateComponent.h"
#include "core/codes.h"
#include "windows/inputPoller.h"

namespace Engine
{
	void RotateComponent::onUpdate(float timestep)
	{
		// If the left mouse button is pressed
		if (InputPoller::isMouseButtonPressed(ENGINE_MOUSE_BUTTON_RIGHT))
		{
			// Set a angular velocity for the object based on the mouse movement
			glm::vec2 currentMousePos = InputPoller::getMousePosition();
			glm::vec2 mouseDelta = currentMousePos - m_lastMousePosition;

			m_velocity = glm::vec3(mouseDelta.y * -10, mouseDelta.x * 10, 0.f);

			m_lastMousePosition = currentMousePos;
		}
		else
		{
			// Slow the spinning down
			m_velocity.x = m_velocity.x * 0.99f;
			m_velocity.y = m_velocity.y * 0.99f;
			m_velocity.z = m_velocity.z * 0.99f;
		}
		// Send the velocity as a message to another component
		glm::vec3 data(m_velocity);
		sendMessage(ComponentMessage(ComponentMessageType::VelocitySetAngular, (void*)&data));
	}

	void RotateComponent::onEvent(Event& e)
	{
		// Create event dispatcher
		EventDispatcher dispatcher(e);
		// If the Event type matches, call the corresponding function
		dispatcher.dispatch<MouseButtonPressedEvent>(std::bind(&RotateComponent::onMouseButtonPressed, this, std::placeholders::_1));
	}

	bool RotateComponent::onMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		// If the button pressed is the right mouse button
		if (e.getMouseButton() == ENGINE_MOUSE_BUTTON_RIGHT)
		{
			// Set the last mouse position to the current mouse position
			m_lastMousePosition = InputPoller::getMousePosition();
			return true;
		}
		return false;
	}
}
