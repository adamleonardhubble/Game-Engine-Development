/** \file FPSCameraComponent.h
*/
#pragma once

#include "3DCameraComponent.h"

#include "events/mouseEvents.h"

namespace Engine
{
	/**
	\class FPSCameraComponent
	\brief Controller component for a first person 3D camera
	*/
	class FPSCameraComponent : public CameraComponent3D
	{
	public:
		//! Constructor
		/*!
		\param fov The new field of view
		\param aspectRatio The new aspectRatio
		\param nearClip The new nearClip
		\param farClip The new farClip
		\param position The new position
		\param rotation The new rotation
		\param updateMousePress The new value
		*/
		FPSCameraComponent(float fov, float aspectRatio, float nearClip, float farClip, glm::vec3 position, glm::vec2 rotation, bool updateMousePress);

		//! Function called when the component is attached to a gameobject
		/*!
		\param owner Pointer to the gameobject the component is being attached to
		*/
		void onAttach(GameObject* owner) override;
		//! Function to update the component every frame
		/*!
		\param timestep The time since the previous frame
		*/
		void onUpdate(float timestep) override;
		//! Function to handle events
		/*!
		\param e The event
		*/
		void onEvent(Event& event) override;
		//! Function for mouse button press events
		/*!
		\param e The event
		\return If the event is handled or not
		*/
		bool onMouseButtonPressed(MouseButtonPressedEvent& e);
		//! Function for mouse button release events
		/*!
		\param e The event
		\return If the event is handled or not
		*/
		bool onMouseButtonReleased(MouseButtonReleasedEvent& e);
		//! Function to update the cameras view
		void updateView() override;
		//! Function to set the camera position and rotation
		/*!
		\param position The new position
		\param rotation The new rotation
		*/
		void setPosAndRot(glm::vec3 position, glm::vec2 rotation);

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::FPSCamera; }
	};
}