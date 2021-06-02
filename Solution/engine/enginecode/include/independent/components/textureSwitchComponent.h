/** \file textureSwitchComponent.h
*/
#pragma once

#include "controllerComponent.h"
#include "rendering/texture.h"

namespace Engine
{
	/**
	\class TextureSwitchComponent
	\brief Component to make a gameobject change between two textures
	*/
	class TextureSwitchComponent : public ControllerComponent
	{
	private:
		std::shared_ptr<Texture> m_tex1; //!< Pointer to the texture
		std::shared_ptr<Texture> m_tex2; //!< Pointer to the texture

		unsigned int m_iTexSlot; //!< The current texture slot

		//! Function to set a texture
		/*!
		\param data Which texture to set to
		*/
		void textureSet(void* data);
	public:
		//! Constructor
		/*!
		\param tex1 Pointer to one texture
		\param tex2 Pointer to the other texture
		*/
		TextureSwitchComponent(std::shared_ptr<Texture> tex1, std::shared_ptr<Texture> tex2) : m_tex1(tex1), m_tex2(tex2) 
		{
			m_componentType = ComponentType::TextureSwitch; // Set the component type
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
		void receiveMessage(const ComponentMessage& msg) override;

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::TextureSwitch; }
	};
}