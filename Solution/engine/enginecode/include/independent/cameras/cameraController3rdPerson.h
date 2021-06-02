/** \file cameraController3rdPerson.h
*/
#pragma once

#include "cameraController.h"
#include "perspectiveEulerCamera3D.h"

#include "events/mouseEvents.h"
#include "events/keyEvents.h"

namespace Engine
{
	/**
	\class CameraController3rdPerson
	\brief Controller for a 3rd person camera
	*/
	class CameraController3rdPerson : public CameraController
	{
		std::shared_ptr<PerspectiveEulerCamera3D> m_camera; //!< Pointer to the camera
		glm::mat4 m_view; //!< The cameras view, for calculations
		glm::vec3 m_position; //!< The cameras position, for calculations
		glm::vec3 m_forward; //!< The cameras forward axis
		glm::vec3 m_up; //!< The cameras up axis
		glm::vec3 m_right; //!< The cameras right axis
		glm::vec3 m_worldUp; //!< The world up axis

		glm::vec3 m_playerPosition; //!< The position of the player object
		glm::vec3 m_playerOffset; //!< The offset of the camera from the player

		float m_fYaw; //!< The cameras yaw
		float m_fPitch; //!< The cameras pitch
		float m_fTranslationSpeed = 5.f; //!< The speed the camera will move
		float m_fRotationSpeed = 4.f; //!< The speed the camera will rotate
		glm::vec2 m_lastMousePosition; //!< The previous mouse position, for using the mouse to rotate the camera

		bool m_bUpdate = true; //!< If the camera is updating

		//! Function to update the cameras view
		void updateView();
	public:
		//! Constructor
		/*!
		\param update If the camera can be updated when the level loads
		*/
		CameraController3rdPerson(bool update) : m_bUpdate(update) {}

		//! Function to initialize the controller
		/*!
		\param fov The field of view
		\param aspectRatio The cameras aspect ratio
		\param nearClip Closest something can be drawn
		\param farClip Furthest something can be drawn
		\param position The position for the camera to start in
		\param rotation The yaw and pitch of the camera
		*/
		void init(float fov, float aspectRatio, float nearClip, float farClip, glm::vec3 position, glm::vec2 rotation) override;
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
		//! Function to handle mouse button press events
		/*!
		\param e The event
		\return If the event was successful or not
		*/
		bool onMouseButtonPressed(MouseButtonPressedEvent& e);
		//! Function to handle key press events
		/*!
		\param e The event
		\return If the event was successful or not
		*/
		bool onKeyPressed(KeyPressedEvent& e);
	};
}