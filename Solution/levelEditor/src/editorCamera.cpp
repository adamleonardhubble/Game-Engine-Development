/** \file editorCamera.cpp
*/
#include <Engine.h>

#include "editorCamera.h"

void EditorCamera::init(float fov, float aspectRatio, float nearClip, float farClip, glm::vec3 position, glm::vec2 rotation)
{
	// Make a new camera
	m_camera.reset(new Engine::PerspectiveEulerCamera3D(fov, aspectRatio, nearClip, farClip));
	m_position = position; // Initialize the position
	m_worldUp = glm::vec3(0.f, 1.f, 0.f); // Initialize the world up axis
	m_fYaw = rotation.x; // Initialize the yaw
	m_fPitch = rotation.y; // Initialize the pitch
	updateView(); // Update the camera view
}

void EditorCamera::onUpdate(float timestep)
{
	// If the left mouse button is pressed
	if (m_bLeftPressed)
	{
		glm::vec2 currentMousePosition = Engine::InputPoller::getMousePosition();
		glm::vec2 mouseDelta = currentMousePosition - m_lastMousePosition;
		// If the left alt button is pressed, rotate the camera with the mouse movement
		if (Engine::InputPoller::isKeyPressed(ENGINE_KEY_LEFT_ALT))
		{
			m_fYaw -= mouseDelta.x * m_fRotationSpeed * timestep;
			m_fPitch -= mouseDelta.y * m_fRotationSpeed * timestep;

			// Prevent the camera rotating too far up or down
			if (m_fPitch > 89.f)
				m_fPitch = 89.f;
			if (m_fPitch < -89.f)
				m_fPitch = -89.f;
		}

		m_lastMousePosition = currentMousePosition;
	}
	// If the right mouse button is pressed
	else if (m_bRightPressed)
	{
		// Move the camera with the mouse movement in the up and right axis
		glm::vec2 currentMousePosition = Engine::InputPoller::getMousePosition();
		glm::vec2 mouseDelta = currentMousePosition - m_lastMousePosition;

		m_position -= m_right * mouseDelta.x * m_fTranslationSpeed * timestep;
		m_position += m_up * mouseDelta.y * m_fTranslationSpeed * timestep;

		m_lastMousePosition = currentMousePosition;
	}

	// Update the camera view
	updateView();
}

void EditorCamera::onEvent(Engine::Event& event)
{
	// Create event dispatcher
	Engine::EventDispatcher dispatcher(event);
	// If the Event type matches, call the corresponding function
	dispatcher.dispatch<Engine::WindowResizeEvent>(std::bind(&EditorCamera::onResize, this, std::placeholders::_1));
	dispatcher.dispatch<Engine::MouseButtonPressedEvent>(std::bind(&EditorCamera::onMouseButtonPressed, this, std::placeholders::_1));
	dispatcher.dispatch<Engine::MouseButtonReleasedEvent>(std::bind(&EditorCamera::onMouseButtonReleased, this, std::placeholders::_1));
	dispatcher.dispatch<Engine::MouseScrolledEvent>(std::bind(&EditorCamera::onMouseScrolled, this, std::placeholders::_1));
}

bool EditorCamera::onMouseButtonPressed(Engine::MouseButtonPressedEvent& e)
{
	// The event was the left mouse button being pressed
	if (e.getMouseButton() == ENGINE_MOUSE_BUTTON_LEFT)
	{
		// Set the last mouse position to the current mouse position
		m_lastMousePosition = Engine::InputPoller::getMousePosition();
		m_bLeftPressed = true;
	}
	if (e.getMouseButton() == ENGINE_MOUSE_BUTTON_RIGHT)
	{
		// Set the last mouse position to the current mouse position
		m_lastMousePosition = Engine::InputPoller::getMousePosition();
		m_bRightPressed = true;
	}
	
	return false;
}

bool EditorCamera::onMouseButtonReleased(Engine::MouseButtonReleasedEvent& e)
{
	// The event was the left mouse button being pressed
	if (e.getMouseButton() == ENGINE_MOUSE_BUTTON_LEFT)
	{
		m_bLeftPressed = false;
	}
	if (e.getMouseButton() == ENGINE_MOUSE_BUTTON_RIGHT)
	{
		m_bRightPressed = false;
	}

	return false;
}

bool EditorCamera::onMouseScrolled(Engine::MouseScrolledEvent& e)
{
	// Move the camera in the forward axis based on the scroll offset
	m_position += m_forward * e.getYOffset() * m_fTranslationSpeed;
	updateView();

	return false;
}

glm::vec3 EditorCamera::getPosition()
{
	return m_position;
}

glm::vec2 EditorCamera::getYawPitch()
{
	return glm::vec2(m_fYaw, m_fPitch);
}

void EditorCamera::updateView()
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