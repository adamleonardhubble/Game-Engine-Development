/** \file cameraController.h
*/
#pragma once

#include "camera.h"
#include "events/event.h"
#include "events/windowEvents.h"

namespace Engine
{
	/**
	\class CameraController
	\brief Base class for all camera controllers to inherit from
	*/
	class CameraController
	{
	public:
		//! Function to initialize the controller
		virtual void init(float, float, float, float, glm::vec3 = glm::vec3(0), glm::vec2 = glm::vec2(0)) = 0;
		//! Function to get the camera
		/*!
		\return A pointer to the camera
		*/
		virtual std::shared_ptr<Camera> getCamera() = 0;
		//! Function to update the controller every frame
		/*!
		\param timestep The time since the previous frame
		*/
		virtual void onUpdate(float timestep) = 0;
		//! Function to handle events
		/*!
		\param e The event
		*/
		virtual void onEvent(Event& e) = 0;
		//! Function to handle window resize events
		/*!
		\param e The event
		*/
		virtual bool onResize(WindowResizeEvent& e) = 0;

		//! Function to get the position of the camera
		/*!
		\return The position of the camera
		*/
		virtual glm::vec3 getPosition() { return glm::vec3(0); }
		//! Function to get the yaw and the pitch of the camera
		/*!
		\return The yaw and the pitch of the camera
		*/
		virtual glm::vec2 getYawPitch() { return glm::vec2(0); }
	};
}