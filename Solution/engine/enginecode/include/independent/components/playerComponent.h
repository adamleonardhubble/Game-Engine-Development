/** \file playerComponent.h
*/
#pragma once

#include "controllerComponent.h"
#include "cameras/cameraController.h"

#include "events/mouseEvents.h"
#include "events/keyEvents.h"

#include <glm/glm.hpp>

namespace Engine
{
	/**
	\class PlayerComponent
	\brief Component which makes the gameobject controllable as a player object
	*/
	class PlayerComponent : public ControllerComponent
	{
	private:
		float m_fTranslationSpeed = 5.f; //!< The speed the player will move
		float m_fRotationSpeed = 4.f; //!< The speed the player will rotate
		glm::vec3 m_position; //!< The player position
		glm::vec2 m_lastMousePosition; //!< The previous mouse position, for using the mouse to rotate the player object

		glm::vec3 m_forward; //!< The forward axis
		glm::vec3 m_right; //!< The right axis
		float m_fYaw = 0; //!< The yaw
		float m_fPitch = 0; //!< The pitch (in order to move at the same speed as the cameras)
		glm::vec3 m_worldUp = glm::vec3(0.f, 1.f, 0.f); //!< The world up axis

		bool m_bUpdate; //!< If the component is being updated

		std::pair<std::shared_ptr<CameraController>, std::shared_ptr<CameraController>> m_cameras; //!< The cameras being used
		bool m_bCam1; //!< Which camera is being used

		//! Function to set the forward and right axis
		void updateAxis();
	public:
		//! Constructor
		/*!
		\param update If the component will be updated or not
		\param position The position of the object
		\param cameras A pair of cameras to be switched between
		\param cam1 If the first or second camera in the pair is being used
		*/
		PlayerComponent(bool update, glm::vec3 position, std::pair<std::shared_ptr<CameraController>, std::shared_ptr<CameraController>> cameras, bool cam1) : 
			m_bUpdate(update), m_position(position), m_cameras(cameras), m_bCam1(cam1) 
		{
			m_componentType = ComponentType::Player; // Set the component type
		}

		//! Function to get both cameras
		/*!
		\return A pair containing pointers to both cameras
		*/
		inline std::pair<std::shared_ptr<CameraController>, std::shared_ptr<CameraController>> getCameras() { return m_cameras; }

		//! Function to get the current camera being used
		/*!
		\return A pointer to the camera currently being used
		*/
		inline std::shared_ptr<CameraController> getCurrentCamera() 
		{ if (m_bCam1) return m_cameras.first; else return m_cameras.second; }

		//! Function to update the component every frame
		/*!
		\param timestep The time since the previous frame
		*/
		void onUpdate(float timestep) override;
		//! Function to handle events
		/*!
		\param e The event
		*/
		void onEvent(Event& e) override;

		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override {}

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

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::Player; }
	};
}