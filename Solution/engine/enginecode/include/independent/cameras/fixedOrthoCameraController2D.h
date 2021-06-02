/** \file fixedOrthoCameraController2D.h
*/
#pragma once

#include "cameraController.h"
#include "orthographicCamera2D.h"

namespace Engine
{
	/**
	\class FixedOrthoCameraController2D
	\brief Controller for a 2D orthographic camera that can't be moved
	*/
	class FixedOrthoCameraController2D : public CameraController
	{
	private:
		std::shared_ptr<OrthographicCamera2D> m_camera; //!< Pointer to the camera
		glm::vec2 m_position = glm::vec2(0.f); //!< The cameras position, for calculations
	public:
		//! Constructor
		/*!
		\param update If the camera can be updated when the level loads
		*/
		FixedOrthoCameraController2D(bool update = false) {}

		//! Function to initialize the controller
		/*!
		\param left The pixel position of the left of the camera
		\param top The pixel position of the top of the camera
		\param width The width of the camera in pixels
		\param height The height of the camera in pixels
		*/
		void init(float left, float top, float width, float height, glm::vec3, glm::vec2) override;
		//! Function to get the camera
		/*!
		\return A pointer to the camera
		*/
		inline std::shared_ptr<Camera> getCamera() override { return m_camera; }
		//! Function to update the controller every frame
		/*!
		\param timestep The time since the previous frame
		*/
		void onUpdate(float timestep) override {}
		//! Function to handle events
		/*!
		\param e The event
		*/
		void onEvent(Event& e) override;
		//! Function to handle window resize events
		/*!
		\param e The event
		\return If the event was successful or not
		*/
		bool onResize(WindowResizeEvent& e) override { return true; }
	};
}