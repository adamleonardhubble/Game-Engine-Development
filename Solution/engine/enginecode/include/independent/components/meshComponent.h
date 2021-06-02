/** \file meshComponent.h
*/
#pragma once

#include "CGO.h"

#include "rendering/model.h"
#include "systems/resourceManager.h"

namespace Engine
{
	/**
	\class MeshComponent
	\brief Component to give a gameobject a mesh
	*/
	class MeshComponent : public Component
	{
	private:
		std::shared_ptr<Model> m_pModel; //!< The model

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
	public:
		//! Constructor
		/*!
		\param model The model
		*/
		MeshComponent(std::shared_ptr<Model> model) : m_pModel(model) 
		{
			m_componentType = ComponentType::Mesh; // Set the component type
		}

		//! Deconstructor
		~MeshComponent() {}

		//! Function to set the model to show
		/*!
		\param filepath The filepath of the new model
		*/
		void setModel(const std::string& filepath) 
		{
			m_pModel = ResourceManager::addModel(filepath);
			std::pair<std::string, std::shared_ptr<Model>> data = std::make_pair(filepath, m_pModel);
			ComponentMessage msg(ComponentMessageType::ModelSet, (void*)&data);
			sendMessage(msg);
		}
		//! Function to get the current model
		/*!
		\return A pointer to the model
		*/
		std::shared_ptr<Model> getModel() { return m_pModel; }
		
		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override {}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::Mesh; }
	};
}
