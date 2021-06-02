/** \file sliderJoint.h
*/
#pragma once

#include "jointComponent.h"

namespace Engine
{
	/**
	\class SliderJointComponent
	\brief Class for a slider joint component
	*/
	class SliderJointComponent : public JointComponent
	{
	public:
		//! Constructor
		/**
		\param world The physics world the joint is being added to
		\param anchorPoint The point to anchor the two bodies
		\param bodyA The first body
		\param bodyB the second body
		\param axis The axis that the joint can move in
		\param isLimited Is the joint rotation limited
		\param minLimit The minimum limit, only used if isLimited is true
		\param maxLimit The maximum limit, only used if isLimited is true
		\param bodiesCanCollide If the bodies on the joint can collide with each other
		*/
		SliderJointComponent(rp3d::DynamicsWorld* world, glm::vec3 anchorPoint, rp3d::RigidBody* bodyA, rp3d::RigidBody* bodyB, rp3d::Vector3 axis, 
			bool isLimited, float minLimit, float maxLimit, bool bodiesCanCollide) : JointComponent(anchorPoint, bodyA, bodyB)
		{
			// Create the joint info
			rp3d::SliderJointInfo* jointInfo = new rp3d::SliderJointInfo(m_pBodyA, m_pBodyB, m_anchorPoint, axis);
			// If the joint rotation is limited
			if (isLimited)
			{
				// Set the information needed for a limit
				jointInfo->isLimitEnabled = true;
				jointInfo->minTranslationLimit = minLimit;
				jointInfo->maxTranslationLimit = maxLimit;
			}
			// Set the joint info pointer to the info made
			m_pJointInfo = jointInfo;
			// Set if the bodies on the joint can collide
			m_pJointInfo->isCollisionEnabled = bodiesCanCollide;
			// Create the joint
			m_pJoint = world->createJoint(*m_pJointInfo);

			m_componentType = ComponentType::SliderJoint; // Set the component type
		}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::SliderJoint; }
	};
}