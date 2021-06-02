/** \file controllerComponent.h
*/
#pragma once

#include "CGO.h"

namespace Engine
{
	/**
	\class ControllerComponent
	\brief Component that allows control of the object, using components inheriting from this
	*/
	class ControllerComponent : public Component
	{
	protected:
		//! Function to send a message to another component on the gameobject
		/*!
		\param msg The message being sent
		*/
		void sendMessage(const ComponentMessage& msg) override
		{
			// Send the message to all components on the object
			for (auto it = m_pOwner->begin(); it != m_pOwner->end(); it++)
			{
				(*it)->receiveMessage(msg);
			}
		}
	};
}