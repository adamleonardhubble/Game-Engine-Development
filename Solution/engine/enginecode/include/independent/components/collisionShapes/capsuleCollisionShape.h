/** \file capsuleCollisionShape.h
*/
#pragma once

#include "components/collisionComponent.h"

namespace Engine
{
	/**
	\class CapsuleCollisionComponent
	\brief A capsule shape collision component for the physics
	*/
	class CapsuleCollisionComponent : public CollisionComponent
	{
	private:
		rp3d::decimal m_radius; //!< The radius of the collider
		rp3d::decimal m_height; //!< The height of the collider
	public:
		//! Constructor
		/*!
		\param radius The radius of the collider
		\param height The height of the collider
		\param mass The mass of the collider
		\param category The collision category the collider is in
		\param canCollideWith The collision categories the collider can collide with
		*/
		CapsuleCollisionComponent(float radius, float height, float mass, CollisionCategory category, unsigned short canCollideWith) :
			CollisionComponent(mass, category, canCollideWith), m_radius(radius), m_height(height)
		{
			// Create the shape
			m_pShape = new rp3d::CapsuleShape(m_radius, m_height);

			m_componentType = ComponentType::CapsuleCollider; // Set the component type
		}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::CapsuleCollider; }
	};
}