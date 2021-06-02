/** \file jointComponent.h
*/
#pragma once

#include "components/CGO.h"

#include <reactphysics3d.h>
#include <glm/glm.hpp>

namespace Engine
{
	/**
	\class JointComponent
	\brief Base component class for all joints
	*/
	class JointComponent : public Component
	{
	protected:
		rp3d::Vector3 m_anchorPoint; //!< The anchor point of the joint
		rp3d::RigidBody* m_pBodyA; //!< The first bodt connected
		rp3d::RigidBody* m_pBodyB; //!< The second body connected

		rp3d::JointInfo* m_pJointInfo; //!< The joint information to make the joint

		rp3d::Joint* m_pJoint; //!< The joint
	public:
		//! Constructor
		/*!
		\param anchorPoint The point to anchor the two bodies
		\param bodyA The first body
		\param bodyB the second body
		*/
		JointComponent(glm::vec3 anchorPoint, rp3d::RigidBody* bodyA, rp3d::RigidBody* bodyB) : 
			m_anchorPoint(anchorPoint.x, anchorPoint.y, anchorPoint.z), m_pBodyA(bodyA), m_pBodyB(bodyB) {}

		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override {}
	};
}