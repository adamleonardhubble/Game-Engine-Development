/** \file hingeJoint.h
*/
#pragma once

#include "jointComponent.h"

namespace Engine
{
	/**
	\class HingeJointComponent
	\brief Class for a hinge joint component
	*/
	class HingeJointComponent : public JointComponent
	{
	public:
		//! Constructor
		/**
		\param world The physics world the joint is being added to
		\param anchorPoint The point to anchor the two bodies
		\param axis The axis that the joint can rotate in
		\param bodyA The first body
		\param bodyB the second body
		\param isLimited Is the joint rotation limited
		\param minLimit The minimum limit in degrees, only used if isLimited is true
		\param maxLimit The maximum limit in degrees, only used if isLimited is true
		\param bodiesCanCollide If the bodies on the joint can collide with each other
		*/
		HingeJointComponent(rp3d::DynamicsWorld* world, glm::vec3 anchorPoint, glm::vec3 axis, rp3d::RigidBody* bodyA, rp3d::RigidBody* bodyB,
			bool isLimited, float minLimit, float maxLimit, bool bodiesCanCollide) : JointComponent(anchorPoint, bodyA, bodyB)
		{
			// Create the joint info
			rp3d::HingeJointInfo* jointInfo = new rp3d::HingeJointInfo(m_pBodyA, m_pBodyB, m_anchorPoint, rp3d::Vector3(axis.x, axis.y, axis.z));
			// If the joint rotation is limited
			if (isLimited)
			{
				// Set the information needed for a limit
				jointInfo->isLimitEnabled = true;
				jointInfo->minAngleLimit = glm::radians(minLimit);
				jointInfo->maxAngleLimit = glm::radians(maxLimit);
			}
			// Set the joint info pointer to the info made
			m_pJointInfo = jointInfo;
			// Set if the bodies on the joint can collide
			m_pJointInfo->isCollisionEnabled = bodiesCanCollide;
			// Create the joint
			m_pJoint = world->createJoint(*m_pJointInfo);

			m_componentType = ComponentType::HingeJoint; // Set the component type
		}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::HingeJoint; }
	};
}