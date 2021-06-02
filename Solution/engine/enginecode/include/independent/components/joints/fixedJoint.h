/** \file fixedJoint.h
*/
#pragma once

#include "jointComponent.h"

namespace Engine
{
	/**
	\class FixedJointComponent
	\brief Class for a fixed joint component
	*/
	class FixedJointComponent : public JointComponent
	{
	public:
		//! Constructor
		/**
		\param world The physics world the joint is being added to
		\param anchorPoint The point to anchor the two bodies
		\param bodyA The first body
		\param bodyB the second body
		\param bodiesCanCollide If the bodies on the joint can collide with each other
		*/
		FixedJointComponent(rp3d::DynamicsWorld* world, glm::vec3 anchorPoint, rp3d::RigidBody* bodyA, rp3d::RigidBody* bodyB, bool bodiesCanCollide) :
			JointComponent(anchorPoint, bodyA, bodyB)
		{
			// Create the joint info
			m_pJointInfo = new rp3d::FixedJointInfo(m_pBodyA, m_pBodyB, m_anchorPoint);
			// Set if the bodies on the joint can collide
			m_pJointInfo->isCollisionEnabled = bodiesCanCollide;
			// Create the joint
			m_pJoint = world->createJoint(*m_pJointInfo);

			m_componentType = ComponentType::FixedJoint; // Set the component type
		}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::FixedJoint; }
	};
}