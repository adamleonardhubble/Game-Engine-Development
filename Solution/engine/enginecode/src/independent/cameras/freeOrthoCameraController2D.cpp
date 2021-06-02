/** \file freeOrthoCameraController2D.cpp
*/
#include "engine_pch.h"

#include "cameras/freeOrthoCameraController2D.h"
#include "windows/inputPoller.h"
#include "core/codes.h"

namespace Engine
{
	void FreeOrthoCameraController2D::init(float left, float top, float width, float height, glm::vec3, glm::vec2)
	{
		// Create a new camera
		m_camera.reset(new OrthographicCamera2D(left, left + width, top + height, top));
		m_position = glm::vec2(left, top); // Set the position using values passed in
		m_camera->setPosition(m_position); // Give the position to the camera
	}

	void FreeOrthoCameraController2D::onUpdate(float timestep)
	{
		if (m_bUpdate)
		{
			// If W, A, S or D is pressed, move the camera in the direction
			if (InputPoller::isKeyPressed(ENGINE_KEY_W))
			{
				m_position.x += -sin(glm::radians(m_fRotation) * m_fTranslationSpeed * timestep);
				m_position.y += cos(glm::radians(m_fRotation) * m_fTranslationSpeed * timestep);
			}
			if (InputPoller::isKeyPressed(ENGINE_KEY_S))
			{
				m_position.x -= -sin(glm::radians(m_fRotation) * m_fTranslationSpeed * timestep);
				m_position.y -= cos(glm::radians(m_fRotation) * m_fTranslationSpeed * timestep);
			}
			if (InputPoller::isKeyPressed(ENGINE_KEY_A))
			{
				m_position.x += cos(glm::radians(m_fRotation) * m_fTranslationSpeed * timestep);
				m_position.y += sin(glm::radians(m_fRotation) * m_fTranslationSpeed * timestep);
			}
			if (InputPoller::isKeyPressed(ENGINE_KEY_D))
			{
				m_position.x -= cos(glm::radians(m_fRotation) * m_fTranslationSpeed * timestep);
				m_position.y -= sin(glm::radians(m_fRotation) * m_fTranslationSpeed * timestep);
			}

			// If Q or E is pressed, rotate the camera
			if (InputPoller::isKeyPressed(ENGINE_KEY_Q))
			{
				m_fRotation -= m_fRotationSpeed * timestep;
				if (m_fRotation > 180.f)
					m_fRotation -= 360.f;
				else if (m_fRotation <= -180.f)
					m_fRotation += 360.f;
			}
			if (InputPoller::isKeyPressed(ENGINE_KEY_E))
			{
				m_fRotation += m_fRotationSpeed * timestep;
				if (m_fRotation > 180.f)
					m_fRotation -= 360.f;
				else if (m_fRotation <= -180.f)
					m_fRotation += 360.f;
			}
			// Set the camera position and rotation
			m_camera->setPositionAndRotation(m_position, m_fRotation);
		}
	}

	void FreeOrthoCameraController2D::onEvent(Event& event)
	{
		// Create event dispatcher
		EventDispatcher dispatcher(event);
		// If the Event type matches, call the corresponding function
		dispatcher.dispatch<WindowResizeEvent>(std::bind(&FreeOrthoCameraController2D::onResize, this, std::placeholders::_1));
		dispatcher.dispatch<KeyPressedEvent>(std::bind(&FreeOrthoCameraController2D::onKeyPressed, this, std::placeholders::_1));
	}

	bool FreeOrthoCameraController2D::onKeyPressed(KeyPressedEvent& e)
	{
		// The event was the enter being pressed
		if (e.getKeyCode() == ENGINE_KEY_ENTER)
			m_bUpdate = !m_bUpdate;

		return false;
	}
}