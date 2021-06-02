/** \file oscilateComponent.h
*/
#pragma once

#include "controllerComponent.h"

namespace Engine
{
	/**
	\class OscilateComponent
	\brief Component to make a gameobject move up and down
	*/
	class OscilateComponent : public ControllerComponent
	{
	public:
		//! Enum for whether the object is moving up or down or not moving
		enum state { DOWN = -1, STOPPED = 0, UP = 1 };
	private:
		state m_state = UP; //!< The current state of the object
		float m_fTimeAccumulated; //!< The time passed
	public:
		//! Constructor
		/*!
		\param initialState The state it starts in
		\param timeAccumulated The time to start on
		*/
		OscilateComponent(OscilateComponent::state initialState, float timeAccumulated) : 
			m_state(initialState), m_fTimeAccumulated(timeAccumulated) 
		{
			m_componentType = ComponentType::Oscilate; // Set the component type
		}

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
		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override {}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::Oscilate; }
	};
}