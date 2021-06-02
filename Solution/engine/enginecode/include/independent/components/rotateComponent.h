/** \file rotateComponent.h
*/
#pragma once

#include "controllerComponent.h"

#include "events/mouseEvents.h"

#include <glm/glm.hpp>

namespace Engine
{
	/**
	\class RotateComponent
	\brief Component to make a gameobject rotate with mouse movement
	*/
	class RotateComponent : public ControllerComponent
	{
	private:
		glm::vec2 m_lastMousePosition; //!< For rotation with the mouse
		glm::vec3 m_velocity = glm::vec3(0); //!< New velocity

		//! Function to handle mouse button press events
		/*!
		\param e The event
		\return If the event was successful or not
		*/
		bool onMouseButtonPressed(MouseButtonPressedEvent& e);
	public:
		//! Constructor
		RotateComponent() 
		{
			m_componentType = ComponentType::Rotate; // Set the component type
		}

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

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::Rotate; }
	};
}