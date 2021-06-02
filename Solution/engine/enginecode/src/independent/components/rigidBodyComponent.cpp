/** \file rigidBodyComponent.cpp
*/
#include "engine_pch.h"

#include "components/rigidBodyComponent.h"

#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	RigidBodyComponent::RigidBodyComponent(rp3d::DynamicsWorld* world, glm::vec3 initPos, glm::vec3 initRot, 
		float* accumulator, rp3d::BodyType type, bool usesGravity, const float physicsTimestep) : m_fpTimeAccumulator(accumulator),
		m_type(type), m_fTimestep(physicsTimestep)
	{
		// Convert the glm::vec3 pos and rotation to rp3d vec3 pos and rp3d quat rot
		rp3d::Vector3 initPosition = rp3d::Vector3(initPos.x, initPos.y, initPos.z);
		glm::quat glmQuat = glm::quat(initRot);

		rp3d::Quaternion initRotation = rp3d::Quaternion(glmQuat.x, glmQuat.y, glmQuat.z, glmQuat.w);
		// Make a transform
		rp3d::Transform transform = rp3d::Transform(initPosition, initRotation);

		// Create the rigidbody in the physics world
		m_pBody = world->createRigidBody(transform);
		// Set the type of the body and if it uses gravity
		m_pBody->setType(type);
		m_pBody->enableGravity(usesGravity);
		// Get the material of the body
		m_physicsMat = &(m_pBody->getMaterial());
		// Save the transform
		m_prevTransform = transform;
		m_componentType = ComponentType::RigidBody; // Set the component type
	}

	void RigidBodyComponent::onAttach(GameObject* owner)
	{
		// Set the owner to the one passed in
		m_pOwner = owner;
		m_pBody->setUserData((void*)m_pOwner); // Set the pointer to the object as the user data so that it can be accessed from the event listener
		// Send a pointer to the body to all collision components to set them
		rp3d::RigidBody* body = m_pBody;
		ComponentMessage msg(ComponentMessageType::RigidBody, (void*)body);
		sendMessage(msg);
	}

	void RigidBodyComponent::applyForce(glm::vec3 force)
	{
		// Convert glm::vec3 to rp3d::Vector3 and apply the force
		rp3d::Vector3 newForce = rp3d::Vector3(force.x, force.y, force.z);
		m_pBody->applyForceToCenterOfMass(newForce);
	}

	void RigidBodyComponent::applyForce(glm::vec3 force, glm::vec3 point)
	{
		// Convert glm::vec3 to rp3d::Vector3 and apply the force
		rp3d::Vector3 newForce = rp3d::Vector3(force.x, force.y, force.z);
		rp3d::Vector3 newPoint = rp3d::Vector3(point.x, point.y, point.z);
		m_pBody->applyForce(newForce, newPoint);
	}

	void RigidBodyComponent::applyTorque(glm::vec3 torque)
	{
		// Convert glm::vec3 to rp3d::Vector3 and apply the torque
		rp3d::Vector3 newTorque = rp3d::Vector3(torque.x, torque.y, torque.z);
		m_pBody->applyTorque(newTorque);
	}

	void RigidBodyComponent::setLinearVelocity(glm::vec3 newVel)
	{
		m_pBody->setLinearVelocity(rp3d::Vector3(newVel.x, newVel.y, newVel.z));
	}

	void RigidBodyComponent::setLinearVelocityX(float newX)
	{
		m_pBody->setLinearVelocity(rp3d::Vector3(newX, getLinearVelocityY(), getLinearVelocityZ()));
	}

	void RigidBodyComponent::setLinearVelocityY(float newY)
	{
		m_pBody->setLinearVelocity(rp3d::Vector3(getLinearVelocityX(), newY, getLinearVelocityZ()));
	}

	void RigidBodyComponent::setLinearVelocityZ(float newZ)
	{
		m_pBody->setLinearVelocity(rp3d::Vector3(getLinearVelocityX(), getLinearVelocityY(), newZ));
	}

	void RigidBodyComponent::setAngularVelocity(glm::vec3 newVel)
	{
		m_pBody->setAngularVelocity(rp3d::Vector3(newVel.x, newVel.y, newVel.z));
	}

	void RigidBodyComponent::setAngularVelocityX(float newX)
	{
		m_pBody->setAngularVelocity(rp3d::Vector3(newX, getAngularVelocityY(), getAngularVelocityZ()));
	}

	void RigidBodyComponent::setAngularVelocityY(float newY)
	{
		m_pBody->setAngularVelocity(rp3d::Vector3(getAngularVelocityX(), newY, getAngularVelocityZ()));
	}

	void RigidBodyComponent::setAngularVelocityZ(float newZ)
	{
		m_pBody->setAngularVelocity(rp3d::Vector3(getAngularVelocityX(), getAngularVelocityY(), newZ));
	}

	glm::vec3 RigidBodyComponent::getLinearVelocity()
	{
		// Get Velocity
		rp3d::Vector3 velocity = m_pBody->getLinearVelocity();

		// Return as glm::vec3
		return glm::vec3(velocity.x, velocity.y, velocity.z);
	}

	float RigidBodyComponent::getLinearVelocityX()
	{
		// Get Velocity
		rp3d::Vector3 velocity = m_pBody->getLinearVelocity();

		// Return x
		return velocity.x;
	}

	float RigidBodyComponent::getLinearVelocityY()
	{
		// Get Velocity
		rp3d::Vector3 velocity = m_pBody->getLinearVelocity();

		// Return y
		return velocity.y;
	}

	float RigidBodyComponent::getLinearVelocityZ()
	{
		// Get Velocity
		rp3d::Vector3 velocity = m_pBody->getLinearVelocity();

		// Return z
		return velocity.z;
	}

	glm::vec3 RigidBodyComponent::getAngularVelocity()
	{
		// Get Velocity
		rp3d::Vector3 velocity = m_pBody->getAngularVelocity();

		// Return as glm::vec3
		return glm::vec3(velocity.x, velocity.y, velocity.z);
	}

	float RigidBodyComponent::getAngularVelocityX()
	{
		// Get Velocity
		rp3d::Vector3 velocity = m_pBody->getAngularVelocity();

		// Return x
		return velocity.x;
	}

	float RigidBodyComponent::getAngularVelocityY()
	{
		// Get Velocity
		rp3d::Vector3 velocity = m_pBody->getAngularVelocity();

		// Return y
		return velocity.y;
	}

	float RigidBodyComponent::getAngularVelocityZ()
	{
		// Get Velocity
		rp3d::Vector3 velocity = m_pBody->getAngularVelocity();

		// Return z
		return velocity.z;
	}

	void RigidBodyComponent::onUpdate(float timestep)
	{
		// If the body is dynamic
		if (m_type == rp3d::BodyType::DYNAMIC || m_type == rp3d::BodyType::KINEMATIC)
		{
			// Find the interpolation factor using the accumulated time
			rp3d::decimal interpolationFactor = *m_fpTimeAccumulator / m_fTimestep;
			// Get the transform of the rigidbody and interpolate to find the transform to render
			rp3d::Transform currentTransform = m_pBody->getTransform();
			rp3d::Transform interpolatedTransform = rp3d::Transform::interpolateTransforms(
				m_prevTransform, currentTransform, interpolationFactor);
			// Get the position and rotation from the interpolated transfrom
			rp3d::Vector3 pos = interpolatedTransform.getPosition();
			rp3d::Quaternion rot = interpolatedTransform.getOrientation();

			// Get the euler angles of the rotation
			glm::vec3 euler = glm::eulerAngles(glm::quat(rot.w, rot.x, rot.y, rot.z));

			// Send the interpolated transform to the position component
			glm::vec3 posData = glm::vec3(pos.x, pos.y, pos.z);
			ComponentMessage msg(ComponentMessageType::PositionSet, (void*)&posData);
			sendMessage(msg);
			glm::vec3 rotData = glm::vec3(glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z));
			ComponentMessage msg2(ComponentMessageType::RotationSet, (void*)&rotData);
			sendMessage(msg2);
			// Save the current body transform
			m_prevTransform = currentTransform;
		}
	}

	void RigidBodyComponent::sendMessage(const ComponentMessage& msg)
	{
		// Send the message to all components on the object
		for (auto it = m_pOwner->begin(); it != m_pOwner->end(); it++)
		{
			(*it)->receiveMessage(msg);
		}
	}
}