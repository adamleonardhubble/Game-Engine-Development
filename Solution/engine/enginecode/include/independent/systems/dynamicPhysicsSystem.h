/** \file dynamicPhysicsSystem.h
*/
#pragma once

#include "system.h"
#include "components/CGO.h"

#include "components/ray.h"

#include <reactphysics3d.h>

namespace Engine
{
	class DynamicPhysicsSystem;

	/**
	\class PhysicsEventListener
	\brief Event listener for the physics, inherits from the rp3d event listener to override the function to get collisions
	*/
	class PhysicsEventListener : public rp3d::EventListener
	{
	private:
		DynamicPhysicsSystem* m_physicsSystem; //!< A pointer to the physics system
	public:
		//! Default constructor
		PhysicsEventListener() {}
		//! Constructor
		/*!
		\param system A pointer to the physics system
		*/
		PhysicsEventListener(DynamicPhysicsSystem* system) : m_physicsSystem(system) {}

		//! Function called when a collision occurs
		/*!
		\param collisionInfo A struct containing the contact information
		*/
		void newContact(const rp3d::CollisionCallback::CollisionCallbackInfo& collisionInfo) override;
	};

	/**
	\class PhysicsCallbackClass
	\brief Callback class for raycatsing, inherits from the rp3d raycast callback class to override the function called when a ray has hit something
	*/
	class PhysicsCallbackClass : public rp3d::RaycastCallback
	{
	private:
		DynamicPhysicsSystem* m_physicsSystem; //!< A pointer to the physics system
	public:
		//! Default constructor
		PhysicsCallbackClass() {}
		//! Constructor
		/*!
		\param system A pointer to the physics system
		*/
		PhysicsCallbackClass(DynamicPhysicsSystem* system) : m_physicsSystem(system) {}

		//! The function called when a ray has hit something
		/*!
		\param info The hit info
		\return
		*/
		rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& info) override;
	};

	//! Info from a collision
	struct CollisionInfo
	{
		// Collision Data
		std::string thisObjectName;
		std::string thisObjectTag;
		std::string otherObjectName;
		std::string otherObjectTag;
	};

	/**
	\class DynamicPhysicsSystem
	\brief A system controlling a dynamic physics world, only made if a gameobject in the layer has a rigidbody component

	This system controls the physics world. In order to be more efficient, all physics updates are run on a separate thread using the updateWorld function. 
	Since this does not take much of the threads time, it also has the functionality of the ObjectUpdateThread class in order to update objects in the same way 
	when it doesn't need to update the physics.
	*/
	class DynamicPhysicsSystem : public System
	{
	private:
		rp3d::DynamicsWorld* m_pWorld; //!< Pointer to the world

		const float m_fPhysicsTimestep = 1.f/60.f; //!< Constant timestep to update the physics

		static DynamicPhysicsSystem* m_pThis; //!< A pointer to the physics system

		float m_fTimeAccumulator; //!< Time accumulator to control updating the world according to the physics timestep
		float m_fUsedTimeAccumulator; //!< Time accumulator used to render object positions
		static bool s_bExists; //!< If the system has been made

		PhysicsEventListener m_eventListener; //!< The event listener for the physics
		PhysicsCallbackClass m_callbackClass; //!< The callback class for raycasting

		static std::vector<Ray> m_rays; //!< A vector of all rays
		static int m_iCurrentRay; //!< The current ray being updated, to know which objects ray is the one that hits something

		std::vector<std::pair<std::shared_ptr<GameObject>, float>> m_objectVector; //!< Vector of pointers to objects to update, paired with the timestep to update with

		std::mutex m_objectMutex; //!< A mutex to control access to the object vector

		std::mutex m_collisionMutex; //!< A mutex controlling access to the collision vector
		std::vector<std::pair<std::shared_ptr<Component>, CollisionInfo>> m_collisions; //!< A vector with pointers to components that have collided and the info of the collision, so that the lua collision function can happen on the main thread not the physics thread

		static bool s_bSeparateThread; //!< If the physics is being updated on a separate thread
	public:
		//! Constructor
		/*!
		\param threads The number of threads
		*/
		DynamicPhysicsSystem(int threads) { s_bSeparateThread = threads > 6; } // If number of threads is more than 6, update physics on a separate thread

		void start(SystemSignal init = SystemSignal::None, ...) override; //!< A function to start the layerstack
		void stop(SystemSignal close = SystemSignal::None, ...) override; //!< A function to stop the layerstack

		//! Run on the physics thread with a loop that periodically updates the physics world and any objects given
		/*!
		\param running A pointer to the running bool to stop the update loop when the game stops running
		*/
		void updateWorld(bool* running);

		//! Update the physics (if not on a separate thread, update all physics, if on  a separate thread, check collisions and raycasts)
		/*!
		\param timestep The timestep to update with
		*/
		void update(float timestep);

		//! Function to get a pointer to the world
		/*!
		\return Pointer to the world
		*/
		inline rp3d::DynamicsWorld* getWorld() { return m_pWorld; }
		//! Function to get the time accumulator
		/*!
		\return Pointer to the float time accumulator
		*/
		inline float* getTimeAccumulator() { return &m_fUsedTimeAccumulator; }
		//! Function to get the timestep
		/*!
		\return Pointer to the float timestep
		*/
		inline const float getTimestep() { return m_fPhysicsTimestep; }

		//! Called to check if the system exists
		/*!
		\return The bool for if the system exists or not
		*/
		static bool systemExists() { return s_bExists; }

		//! Function to add to the collision vector
		/*!
		\param compPointer 
		\param info The collision info struct
		*/
		void addCollisionToVector(std::vector<std::shared_ptr<Component>>::iterator& compPointer, CollisionInfo info);

		//! Function to add an object and timestep to the vector
		/*!
		\param object A pointer to the object needing to be updated
		\param timestep The timestep to update the object with
		*/
		void addObjectToUpdate(std::shared_ptr<GameObject> object, float timestep);

		//! Function to get if the physics is updating on a separate thread or not
		/*!
		\return A bool
		*/
		static bool separateThread() { return s_bSeparateThread; }

		//! Function to add a raycast to stay and update every frame, not linked to an object
		/*!
		\param startPoint The start position of the ray
		\param distance The distance in all 3 axis for the ray to go
		\return The index value in the vector of the ray
		*/
		static int addRay(glm::vec3 startPoint, glm::vec3 distance);
		//! Function to add a raycast to stay and update every frame, linked to an object
		/*!
		\param object A pointer to the object the ray is linked to
		\param offset The offset from the object to the start point of the ray
		\param distance The distance in all 3 axis for the ray to go
		\return The index value in the vector of the ray
		*/
		static int addRay(GameObject* object, glm::vec3 offset, glm::vec3 distance);
		//! Function to cast a ray once, test it and remove it, not relative to an object
		/*!
		\param startPoint The start position of the ray
		\param distance The distance in all 3 axis for the ray to go
		\return The ray hit info from the cast
		*/
		static RayHitInfo castRay(glm::vec3 startPoint, glm::vec3 distance);
		//! Function to cast a ray once, test it and remove it, relative to an object
		/*!
		\param object A pointer to the object the ray is linked to
		\param offset The offset from the object to the start point of the ray
		\param distance The distance in all 3 axis for the ray to go
		\return The ray hit info from the cast
		*/
		static RayHitInfo castRay(GameObject* object, glm::vec3 offset, glm::vec3 distance);

		//! Function to get a pointer to a ray
		/*!
		\param i The vector index value of the ray
		\return A pointer to the ray
		*/
		static Ray* getRay(int i) { return &m_rays.at(i); }

		//! Function called when a raycast hits something
		/*!
		\param info The hit info
		*/
		void rayHitSomething(const rp3d::RaycastInfo& info);
	};
}