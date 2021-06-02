/** \file rigidBodyComponent.h
*/
#pragma once

#include "CGO.h"

#include <reactphysics3d.h>
#include <glm/glm.hpp>

namespace Engine
{
	/**
	\class RigidBodyComponent
	\brief Component to go on a gameobject to give it physics in the dynamic physics world
	*/
	class RigidBodyComponent : public Component
	{
	private:
		rp3d::RigidBody* m_pBody; //!< Pointer to the physics body
		rp3d::Transform m_prevTransform; //!< The transform used for rendering last frame

		rp3d::BodyType m_type; //!< The type of the body (static, kinematic, dynamic)

		float* m_fpTimeAccumulator; //!< Pointer to the time accumulator from the physics system for smoothly updating the transform
		float m_fTimestep; //!< The timestep to update the physics

		rp3d::Material* m_physicsMat; //!< Poiter to the material of the body

		//! Function to send a message to another component on the gameobject
		/*!
		\param msg The message being sent
		*/
		void sendMessage(const ComponentMessage& msg) override;
	public:
		//! Constructor
		/*!
		\param world A pointer to the dynamics world the rigidbody will be part of
		\param initPos The start position of the rigidbody
		\param initRot The start rotation of the rigidbody
		\param accumulator Pointer to the physics system time accumulator
		\param type The type of the rigidbody
		\param usesGravity If the rigidbody uses gravity
		\param physicsTimestep The timestep for updating the physics
		*/
		RigidBodyComponent(rp3d::DynamicsWorld* world, glm::vec3 initPos, glm::vec3 initRot,
			float* accumulator, rp3d::BodyType type, bool usesGravity, const float physicsTimestep);

		//! Function to get the material for changing
		/*!
		\return A pointer to the material
		*/
		rp3d::Material* getPhysicsMaterial() { return m_physicsMat; }

		//! Function called when the component is attached to a gameobject
		/*!
		\param owner Pointer to the gameobject the component is being attached to
		*/
		virtual void onAttach(GameObject* owner) override;

		//! Function to apply force to the center of mass of the rigidbody
		/*!
		\param force The force (in newtons)
		*/
		void applyForce(glm::vec3 force);
		//! Function to apply force to a point on the rigidbody
		/*!
		\param force The force (in newtons)
		\param point The point to apply the force to
		*/
		void applyForce(glm::vec3 force, glm::vec3 point);
		//! Function to apply torque to the rigidbody
		/*!
		\param force The torque
		*/
		void applyTorque(glm::vec3 torque);
		//! Function to give the body a linear velocity
		/*!
		\param newVel The linear velocity to set the body to
		*/
		void setLinearVelocity(glm::vec3 newVel);
		//! Function to set the bodies linear x velocity
		/*!
		\param newX the new x value
		*/
		void setLinearVelocityX(float newX);
		//! Function to set the bodies linear y velocity
		/*!
		\param newY the new y value
		*/
		void setLinearVelocityY(float newY);
		//! Function to set the bodies linear z velocity
		/*!
		\param newZ the new z value
		*/
		void setLinearVelocityZ(float newZ);
		//! Function to give the body a angular velocity
		/*!
		\param newVel The angular velocity to set the body to
		*/
		void setAngularVelocity(glm::vec3 newVel);
		//! Function to set the bodies linear x velocity
		/*!
		\param newX the new x value
		*/
		void setAngularVelocityX(float newX);
		//! Function to set the bodies linear y velocity
		/*!
		\param newY the new y value
		*/
		void setAngularVelocityY(float newY);
		//! Function to set the bodies linear z velocity
		/*!
		\param newZ the new z value
		*/
		void setAngularVelocityZ(float newZ);

		//! Function to get a pointer to the rigidbody
		/*!
		\return The pointer
		*/
		rp3d::RigidBody* getBody() { return m_pBody; }

		//! Function to return the linear velocity of the body
		/*!
		\return The linear velocity
		*/
		glm::vec3 getLinearVelocity();
		// ! Function to return the x component of the angular velocity of the body
		/*!
		\return The angular velocities x component
		*/
		float getLinearVelocityX();
		// ! Function to return the y component of the angular velocity of the body
		/*!
		\return The angular velocities y component
		*/
		float getLinearVelocityY();
		// ! Function to return the z component of the angular velocity of the body
		/*!
		\return The angular velocities z component
		*/
		float getLinearVelocityZ();
		//! Function to return the angular velocity of the body
		/*!
		\return The angular velocity
		*/
		glm::vec3 getAngularVelocity();
		// ! Function to return the x component of the angular velocity of the body
		/*!
		\return The angular velocities x component
		*/
		float getAngularVelocityX();
		// ! Function to return the y component of the angular velocity of the body
		/*!
		\return The angular velocities y component
		*/
		float getAngularVelocityY();
		// ! Function to return the z component of the angular velocity of the body
		/*!
		\return The angular velocities z component
		*/
		float getAngularVelocityZ();

		//! Function to update the component every frame
		/*!
		\param timestep The time since the previous frame
		*/
		void onUpdate(float timestep) override;
		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override {}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::RigidBody; }
	};
}