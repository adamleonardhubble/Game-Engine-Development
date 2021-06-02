/** \file oscilateComponent.cpp
*/
#include "engine_pch.h"

#include "components/oscilateComponent.h"

#include <glm/glm.hpp>

namespace Engine
{
	void OscilateComponent::onAttach(GameObject* owner)
	{
		// Set the owner
		m_pOwner = owner;
		
		// If the state is moving down
		if (m_state == DOWN)
		{
			// Send a message with a new downward linear velocity
			glm::vec3 data(0.f, -0.2f, 0.f);
			sendMessage(ComponentMessage(ComponentMessageType::VelocitySetLinear, (void*)&data));
		}
		else if (m_state == UP) // If the state is moving up
		{
			// Send a message with a new upward linear velocity
			glm::vec3 data(0.f, 0.2f, 0.f);
			sendMessage(ComponentMessage(ComponentMessageType::VelocitySetLinear, (void*)&data));
		}
		// Send a message with the state to set the correct texture
		sendMessage(ComponentMessage(ComponentMessageType::TextureSet, (void*)&m_state));
	}

	void OscilateComponent::onUpdate(float timestep)
	{
		// Add the timestep to the time accumulated
		m_fTimeAccumulated += timestep;

		// If the time accumulated is bigger than 20 seconds
		if (m_fTimeAccumulated >= 20.f)
		{
			// If the state is moving down
			if (m_state == DOWN)
			{
				m_state = UP; // Change the state
				// Send a message with a new downward linear velocity
				glm::vec3 data(0.f, 0.2f, 0.f);
				sendMessage(ComponentMessage(ComponentMessageType::VelocitySetLinear, (void*)&data));
			}
			else if (m_state == UP) // If the state is moving up
			{
				m_state = DOWN; // Change the state
				// Send a message with a new downward linear velocity
				glm::vec3 data(0.f, -0.2f, 0.f);
				sendMessage(ComponentMessage(ComponentMessageType::VelocitySetLinear, (void*)&data));
			}
			// Send a message with the state to set the correct texture
			sendMessage(ComponentMessage(ComponentMessageType::TextureSet, (void*)&m_state));

			m_fTimeAccumulated = 0; // Reset the time accumulated
		}
	}
}