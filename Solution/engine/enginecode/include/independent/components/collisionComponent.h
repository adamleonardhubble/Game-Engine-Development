/** \file collisionComponent.h
*/
#pragma once

#include "CGO.h"

#include <reactphysics3d.h>
#include <glm/glm.hpp>

namespace Engine
{
	enum CollisionCategory {
		CATEGORY1 = 0x0001,
		CATEGORY2 = 0x0002,
		CATEGORY3 = 0x0004,
		CATEGORY4 = 0x0008
	};
	/**
	\class CollisionComponent
	\brief Base class for all collision shape components
	*/
	class CollisionComponent : public Component
	{
	private:
		rp3d::Transform m_transform; //!< The transform of the collider
		rp3d::decimal m_mass; //!< The mass of the collider
		rp3d::ProxyShape* m_proxyShape; //!< A pointer to the collider for editing once it has been attached to a body

		CollisionCategory m_category; //!< The collision category the collider is in
		unsigned short m_canCollideWith; //!< The collision categories the collider can collide with

		//! Function to add the collision shape to the rigidboy received in a message
		/*!
		\param data The rigidbody
		*/
		void bodySet(void* data)
		{
			// Get the body from the data
			rp3d::RigidBody* body = (rp3d::RigidBody*)data;
			// Add the collider to the body
			m_proxyShape = body->addCollisionShape(m_pShape, m_transform, m_mass);

			m_proxyShape->setCollisionCategoryBits(m_category);
			m_proxyShape->setCollideWithMaskBits(m_canCollideWith);
		}
	protected:
		rp3d::CollisionShape* m_pShape; //!< A pointer to the collision shape

		//! Constructor
		/*!
		\param mass The mass of the collider
		\param category The collision category the collider is in
		\param canCollideWith The collision categories the collider can collide with
		*/
		CollisionComponent(rp3d::decimal mass, CollisionCategory category, unsigned short canCollideWith) : 
			m_mass(mass), m_category(category), m_canCollideWith(canCollideWith)
		{
			// Create a transform
			m_transform = rp3d::Transform::identity();
		}
	public:
		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override
		{
			// Create message dispatcher
			MessageDispatcher dispatcher(msg);
			// If the message type matches, call the corresponding function
			dispatcher.dispatch(ComponentMessageType::RigidBody, std::bind(&CollisionComponent::bodySet, this, std::placeholders::_1));
		}
	};
}