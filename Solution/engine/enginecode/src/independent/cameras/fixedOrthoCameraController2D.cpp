/** \file fixedOrthoCameraController2D.cpp
*/
#include "engine_pch.h"

#include "cameras/fixedOrthoCameraController2D.h"

namespace Engine
{
	void FixedOrthoCameraController2D::init(float left, float top, float width, float height, glm::vec3, glm::vec2)
	{
		// Create a new camera
		m_camera.reset(new OrthographicCamera2D(left, left + width, top + height, top));
		m_position = glm::vec2(left, top); // Set the position using values passed in
		m_camera->setPosition(m_position); // Give the position to the camera
	}

	void FixedOrthoCameraController2D::onEvent(Event& event)
	{
		// Create event dispatcher
		EventDispatcher dispatcher(event);
		// If the Event type matches, call the corresponding function
		dispatcher.dispatch<WindowResizeEvent>(std::bind(&FixedOrthoCameraController2D::onResize, this, std::placeholders::_1));
	}
}