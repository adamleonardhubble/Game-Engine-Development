/** \file materialComponent.h
*/
#pragma once

#include "CGO.h"
#include "rendering/material.h"
#include "systems/resourceManager.h"

namespace Engine
{
	/**
	\class MaterialComponent
	\brief Component to give a gameobject a material
	*/
	class MaterialComponent : public Component
	{
	private:
		std::shared_ptr<Material> m_pMaterial; //!< Pointer to the material
		glm::vec3 m_diffMat = glm::vec3(0.f); //!< The diffuse material reflection values
		glm::vec3 m_specMat = glm::vec3(0.f); //!< The specular material reflection values
		float m_fShininess = 0.f; //!< The shininess of the material

		//! Function to set a data element when receiving a UniformUpload message
		/*!
		\param data The uniform data to be set
		*/
		void uniformUpload(void* data)
		{
			// Get the data from the message
			std::pair<std::string, void*> castData = *(std::pair<std::string, void*>*)data;
			m_pMaterial->setDataElement(castData.first, castData.second); // Set the data element in the material
		}
		//! Function to set the model of the object
		/*!
		\param data The filepath of the new model
		*/
		void modelSet(void* data)
		{
			// Get the data from the message
			std::pair<std::string, std::shared_ptr<Model>> castData = *(std::pair<std::string, std::shared_ptr<Model>>*)data;
			// Get/make VAO for the new model
			std::shared_ptr<VertexArray> VAO = ResourceManager::addVAO(castData.first);
			VAO->setVertexBuffer(
				ResourceManager::addVBO(castData.first,
				castData.second->vertices, sizeof(float) * castData.second->verticesSize,
				m_pMaterial->getShader()->getBufferLayout()));
			VAO->setIndexBuffer(ResourceManager::addIndexBuffer(castData.first, castData.second->indices, castData.second->indicesSize));

			void* geometry = (void*)&VAO;
			// Set the new geometry of the material
			m_pMaterial->setGeometry(geometry);
		}
	public:
		//! Constructor
		/*!
		\param material A pointer to the material
		*/
		MaterialComponent(const std::shared_ptr<Material>& material) : m_pMaterial(material) 
		{
			m_componentType = ComponentType::Material; // Set the component type
		}

		//! Function to set the material of the component
		/*!
		\param newMaterial The new material
		*/
		void setMaterial(std::shared_ptr<Material> newMaterial) { m_pMaterial = newMaterial; }

		//! Function to get the materaial
		/*!
		\return The material
		*/
		inline std::shared_ptr<Material> getMaterial() { return m_pMaterial; }

		//! Function to set the diffuse values of the material
		/*!
		\param newDiffuse The new values
		*/
		void setDiffuse(glm::vec3 newDiffuse)
		{
			m_diffMat = newDiffuse;
			m_pMaterial->setDataElement("matDiffuse", (void*)&m_diffMat);
		}
		//! Function to set the specular values of the material
		/*!
		\param newSpecular The new values
		*/
		void setSpecular(glm::vec3 newSpecular)
		{
			m_specMat = newSpecular;
			m_pMaterial->setDataElement("matSpecular", (void*)&m_specMat);
		}
		//! Function to set the shininess value of the material
		/*!
		\param newShininess The new value
		*/
		void setShininess(float newShininess)
		{
			m_fShininess = newShininess;
			m_pMaterial->setDataElement("matShininess", (void*)&m_fShininess);
		}
		//! Function to get the diffuse values of the material
		/*!
		\return The values
		*/
		inline glm::vec3 getDiffuse() { return m_diffMat; }
		//! Function to get the specular values of the material
		/*!
		\return The values
		*/
		inline glm::vec3 getSpecular() { return m_specMat; }
		//! Function to get the shininess value of the material
		/*!
		\return The value
		*/
		inline float getShininess() { return m_fShininess; }

		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override
		{
			// Create message dispatcher
			MessageDispatcher dispatcher(msg);
			// If the message type matches, call the corresponding function
			dispatcher.dispatch(ComponentMessageType::UniformSet, std::bind(&MaterialComponent::uniformUpload, this, std::placeholders::_1));
			dispatcher.dispatch(ComponentMessageType::ModelSet, std::bind(&MaterialComponent::modelSet, this, std::placeholders::_1));
		}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::Material; }
	};
}
