/** \file positionComponent.h
*/
#pragma once

#include "CGO.h"

#include <glm/glm.hpp>

namespace Engine
{
	/**
	\class PositionComponent
	\brief Component to give a gameobject a position
	*/
	class PositionComponent : public Component
	{
	private:
		glm::mat4 m_model; //!< The object model
		glm::mat4 m_translation; //!< The position to calculate the model
		glm::mat4 m_rotation; //!< The rotation to calculate the model
		glm::mat4 m_scale; //!< The scale to calculate the model
		glm::vec3 m_transVec; //!< The position
		glm::vec3 m_rotVec; //!< The rotation
		glm::vec3 m_scaleVec; //!< The scale

		glm::vec3 m_rotDeg; //!< The rotation in degrees

		//! Function to set a position when receiving a message
		/*!
		\param data The new position
		*/
		void positionSet(void* data);
		//! Function to set a rotation when receiving a message
		/*!
		\param data The new rotation
		*/
		void rotationSet(void* data);
		//! Function to set a scale when receiving a message
		/*!
		\param data The new scale
		*/
		void scaleSet(void* data);
		//! Function to set a position based on velocities received in a message
		/*!
		\param data The velocities
		*/
		void positionIntegrate(void* data);

		//! Function to send a message to another component on the gameobject
		/*!
		\param msg The message being sent
		*/
		void sendMessage(const ComponentMessage& msg) override;
	public:
		//! Constructor
		/*!
		\param trans The translation vector
		\param rot The rotation vector
		\param scale The scale of the text
		*/
		PositionComponent(glm::vec3 trans, glm::vec3 rot, glm::vec3 scale);

		//! Funcion to calculate the model
		inline void calculateModel();

		//! Function to get the objects transform
		/*!
		\return The transform of the object
		*/
		inline std::shared_ptr<glm::mat4> getTransform() { return std::make_shared<glm::mat4>(m_model); }

		//! Function to get the objects position
		/*!
		\return The position of the object
		*/
		inline glm::vec3& getPosition() { return m_transVec; }
		//! Function to get the objects rotation
		/*!
		\return The rotation of the object
		*/
		inline glm::vec3 getRotation() { return m_rotVec; }
		//! Function to get the objects rotation in degrees
		/*!
		\return The rotation of the object
		*/
		inline glm::vec3& getRotationDeg() { return m_rotDeg; }
		//! Function to get the objects scale
		/*!
		\return The scale of the object
		*/
		inline glm::vec3& getScale() { return m_scaleVec; }

		//! Function to set the rotation of the object given values in degrees
		/*!
		\param rot The new rotation values
		*/
		void setRotationDeg(glm::vec3 rot)
		{
			m_rotDeg = rot;
			m_rotVec = glm::vec3(glm::radians(m_rotDeg.x), glm::radians(m_rotDeg.y), glm::radians(m_rotDeg.z));
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
		static ComponentType getStaticComponentType() { return ComponentType::Position; }
	};
}