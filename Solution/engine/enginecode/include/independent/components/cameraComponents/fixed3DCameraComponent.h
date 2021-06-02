/** \file fixed3DCameraComponent.h
*/
#pragma once

#include "3DCameraComponent.h"

#include "events/mouseEvents.h"

namespace Engine
{
	/**
	\class Fixed3DCameraComponent
	\brief Controller component for a 3D camera that doesn't move
	*/
	class Fixed3DCameraComponent : public CameraComponent3D
	{
	private:
		void setNewTransform(void* data);

	public:
		//! Constructor
		/*!
		\param fov The new field of view
		\param aspectRatio The new aspectRatio
		\param nearClip The new nearClip
		\param farClip The new farClip
		\param position The new position
		\param rotation The new rotation
		*/
		Fixed3DCameraComponent(float fov, float aspectRatio, float nearClip, float farClip, glm::vec3 position, glm::vec2 rotation);

		//! Function to update the cameras view
		void updateView() override;
		//! Function to set the camera position and rotation
		/*!
		\param position The new position
		\param rotation The new rotation
		*/
		void setPosAndRot(glm::vec3 position, glm::vec2 rotation);

		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override;

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::Fixed3DCamera; }
	};
}