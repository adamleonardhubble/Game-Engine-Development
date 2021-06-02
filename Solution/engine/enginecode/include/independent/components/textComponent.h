/** \file textComponent.h
*/
#pragma once

#include "components/CGO.h"

#include "rendering/text/textLabel.h"

namespace Engine
{
	/**
	\class TextComponent
	\brief Component to give a gameobject text
	*/
	class TextComponent : public Component
	{
	private:
		std::shared_ptr<TextLabel> m_pLabel; //!< Pointer to the text label
	public:
		//! Constructor
		/*!
		\param label Pointer to the text label
		*/
		TextComponent(std::shared_ptr<TextLabel> label) : m_pLabel(label) 
		{
			m_componentType = ComponentType::Text; // Set the component type
		}

		//! Function to get the text label
		/*!
		\return A pointer to the label class
		*/
		std::shared_ptr<TextLabel> getLabel() { return m_pLabel; }
		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override {}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::Text; }
	};
}