/** \file velocityComponent.cpp
*/
#include "engine_pch.h"

#include "components/velocityComponent.h"

namespace Engine
{
	void VelocityComponent::onUpdate(float timestep)
	{
		// Send the velocities, normalized with the timestep, to update the position
		std::pair<glm::vec3, glm::vec3> data(m_linear * timestep, m_angular * timestep);
		sendMessage(ComponentMessage(ComponentMessageType::PositionIntegrate, (void*)&data));
	}

	void VelocityComponent::receiveMessage(const ComponentMessage& msg)
	{
		// Create message dispatcher
		MessageDispatcher dispatcher(msg);
		// If the message type matches, call the corresponding function
		dispatcher.dispatch(ComponentMessageType::VelocitySetLinear, std::bind(&VelocityComponent::setLinear, this, std::placeholders::_1));
		dispatcher.dispatch(ComponentMessageType::VelocitySetAngular, std::bind(&VelocityComponent::setAngular, this, std::placeholders::_1));
	}

	void VelocityComponent::setLinear(void* data)
	{
		glm::vec3 linear = *(glm::vec3*)data; // Get the data from the message
		m_linear = linear; // Set the linear velocity
	}
	
	void VelocityComponent::setAngular(void* data)
	{
		glm::vec3 angular = *(glm::vec3*)data; // Get the data from the message
		m_angular = angular; // Set the angular velocity
	}

	void VelocityComponent::sendMessage(const ComponentMessage& msg)
	{
		// Send the message to all components on the object
		for (auto it = m_pOwner->begin(); it != m_pOwner->end(); it++)
		{
			(*it)->receiveMessage(msg);
		}
	}
}