/** \file sphereCollisionShape.h
*/
#pragma once

#include "components/collisionComponent.h"

namespace Engine
{
	/**
	\class SphereCollisionComponent
	\brief A sphere shape collision component for the physics
	*/
	class SphereCollisionComponent : public CollisionComponent
	{
	private:
		rp3d::decimal m_radius; //!< The radius of the collider
	public:
		//! Constructor
		/*!
		\param radius The radius of the collider
		\param mass The mass of the collider
		\param category The collision category the collider is in
		\param canCollideWith The collision categories the collider can collide with
		*/
		SphereCollisionComponent(float radius, float mass, CollisionCategory category, unsigned short canCollideWith) : 
			CollisionComponent(mass, category, canCollideWith), m_radius(radius)
		{
			// Create the shape
			m_pShape = new rp3d::SphereShape(m_radius);

			m_componentType = ComponentType::SphereCollider; // Set the component type
		}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::SphereCollider; }
	};
}