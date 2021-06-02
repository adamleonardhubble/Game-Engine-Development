/** \file boxCollisionShape.h
*/
#pragma once

#include "components/collisionComponent.h"

namespace Engine
{
	/**
	\class BoxCollisionComponent
	\brief A box shape collision component for the physics
	*/
	class BoxCollisionComponent : public CollisionComponent
	{
	private:
		rp3d::Vector3 m_halfExtents; //!< The half extents of the collider
	public:
		//! Constructor
		/*!
		\param halfExtents The half extents of the collider
		\param mass The mass of the collider
		\param category The collision category the collider is in
		\param canCollideWith The collision categories the collider can collide with
		*/
		BoxCollisionComponent(glm::vec3 halfExtents, float mass, CollisionCategory category, unsigned short canCollideWith) : 
			CollisionComponent(mass, category, canCollideWith)
		{
			// Convert half extents to rp3d::Vector3
			m_halfExtents = rp3d::Vector3(halfExtents.x, halfExtents.y, halfExtents.z);
			// Create the shape
			m_pShape = new rp3d::BoxShape(m_halfExtents);

			m_componentType = ComponentType::BoxCollider; // Set the component type
		}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::BoxCollider; }
	};
}