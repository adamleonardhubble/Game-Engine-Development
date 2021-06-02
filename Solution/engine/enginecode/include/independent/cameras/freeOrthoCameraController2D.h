/** \file freeOrthoCameraController2D.h
*/
#pragma once

#include "cameraController.h"
#include "orthographicCamera2D.h"

#include "events/keyEvents.h"

namespace Engine
{
	/**
	\class FreeOrthoCameraController2D
	\brief Controller for a 2D orthographic camera that can be moved and rotated freely
	*/
	class FreeOrthoCameraController2D : public CameraController
	{
	private:
		std::shared_ptr<OrthographicCamera2D> m_camera; //!< Pointer to the camera
		glm::vec2 m_position = glm::vec2(0.f); //!< The cameras position, for calculations
		float m_fRotation = 0.f; //!< The cameras rotation, for calculations
		float m_fTranslationSpeed = 100.f; //!< The speed the camera will move
		float m_fRotationSpeed = 10.f; //!< The speed the camera will rotate

		bool m_bUpdate = true; //!< If the camera is being updated
	public:
		//! Constructor
		/*!
		\param update If the camera can be updated when the level loads
		*/
		FreeOrthoCameraController2D(bool update) : m_bUpdate(update) {}

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
		void onUpdate(float timestep) override;
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
		//! Function to handle key press events
		/*!
		\param e The event
		\return If the event was successful or not
		*/
		bool onKeyPressed(KeyPressedEvent& e);
	};
}