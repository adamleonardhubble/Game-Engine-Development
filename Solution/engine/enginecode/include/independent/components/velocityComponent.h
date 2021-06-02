/** \file velocityComponent.h
*/
#pragma once

#include "CGO.h"

#include <glm/glm.hpp>

namespace Engine
{
	/**
	\class VelocityComponent
	\brief Component to give a gameobject velocity
	*/
	class VelocityComponent : public Component
	{
	private:
		glm::vec3 m_linear; //!< Linear velocity
		glm::vec3 m_angular; //!< Angular velocity

		//! Function to set linear velocity
		/*!
		\param data The new velocity
		*/
		void setLinear(void* data);
		//! Function to set angular velocity
		/*!
		\param data The new velocity
		*/
		void setAngular(void* data);

		//! Function to send a message to another component on the gameobject
		/*!
		\param msg The message being sent
		*/
		void sendMessage(const ComponentMessage& msg) override;
	public:
		//! Constructor
		/*!
		\param linear Starting linear velocity
		\param angular Starting angular velocity
		*/
		VelocityComponent(glm::vec3 linear, glm::vec3 angular) : m_linear(linear), m_angular(angular) 
		{
			m_componentType = ComponentType::Velocity; // Set the component type
		}
		//! Function to update the component every frame
		/*!
		\param timestep The time since the previous frame
		*/
		void onUpdate(float timestep) override;
		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override;

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::Velocity; }
	};
}