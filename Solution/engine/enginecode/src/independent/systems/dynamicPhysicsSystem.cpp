/** \file dynamicPhysicsSystem.cpp
*/
#include "engine_pch.h"

#include "systems/dynamicPhysicsSystem.h"
#include "components/LuaScriptComponent.h"

#include "systems/timer.h"

namespace Engine
{
	void PhysicsEventListener::newContact(const rp3d::CollisionCallback::CollisionCallbackInfo& collisionInfo)
	{
		// Get pointers to the gameobject involved in the collision from the user data
		GameObject* objectA = (GameObject*)collisionInfo.body1->getUserData();
		GameObject* objectB = (GameObject*)collisionInfo.body2->getUserData();

		// Create collision info for one of the objects
		CollisionInfo infoA;
		infoA.thisObjectName = objectA->name();
		infoA.thisObjectTag = objectA->tag();
		infoA.otherObjectName = objectB->name();
		infoA.otherObjectTag = objectB->tag();
		
		// Get the script component if there is one
		auto script = objectA->getComponent<LuaScriptComponent<DynamicPhysicsSystem>>();
		// If there is one
		if (script != objectA->end())
		{
			// Add the component and info to the vector for the physics object to handle the collision on the main thread
			m_physicsSystem->addCollisionToVector(script, infoA);
		}

		// Create collision info for the other object
		CollisionInfo infoB;
		infoB.thisObjectName = objectB->name();
		infoB.thisObjectTag = objectB->tag();
		infoB.otherObjectName = objectA->name();
		infoB.otherObjectTag = objectA->tag();

		// Get the script component if there is one
		auto script2 = objectB->getComponent<LuaScriptComponent<DynamicPhysicsSystem>>();
		// If there is one
		if (script2 != objectB->end())
		{
			// Add the component and info to the vector for the physics object to handle the collision on the main thread
			m_physicsSystem->addCollisionToVector(script2, infoB);
		}
	}

	rp3d::decimal PhysicsCallbackClass::notifyRaycastHit(const rp3d::RaycastInfo& info)
	{
		m_physicsSystem->rayHitSomething(info);
		
		// Return a fraction of 1.0 to gather all hits
		return rp3d::decimal(1.0);
	}

	bool DynamicPhysicsSystem::s_bExists = false;
	bool DynamicPhysicsSystem::s_bSeparateThread = false;
	std::vector<Ray> DynamicPhysicsSystem::m_rays;
	DynamicPhysicsSystem* DynamicPhysicsSystem::m_pThis = nullptr;
	int DynamicPhysicsSystem::m_iCurrentRay = 0;

	void DynamicPhysicsSystem::start(SystemSignal init, ...)
	{
		if (!s_bExists)
		{
			// Gravity vector
			rp3d::Vector3 gravity(0.0, -9.81, 0.0);
			// Create the dynamics world
			m_pWorld = new rp3d::DynamicsWorld(gravity);
			m_fTimeAccumulator = 0; // Reset the accumulator
			m_fUsedTimeAccumulator = 0;
			s_bExists = true;
			
			m_pThis = this;

			m_callbackClass = PhysicsCallbackClass(this);
			m_eventListener = PhysicsEventListener(this);

			m_pWorld->setEventListener(&m_eventListener); // Set the event listener
		}
	}

	void DynamicPhysicsSystem::stop(SystemSignal close, ...)
	{
		// Delete the dynamics world and destroy all physics objects
		delete m_pWorld;
		s_bExists = false;
	}

	void DynamicPhysicsSystem::updateWorld(bool* running)
	{
		float time = TIMER_NEWPHYSICS;

		while (*running)
		{
			time = TIMER_NEWPHYSICS;
			
			if (!s_bExists)
				break;

			// Add the time differrence to the accumulator
			m_fTimeAccumulator += time;

			if (m_fTimeAccumulator > m_fPhysicsTimestep)
			{
				// While there is enough accumulated time, take one or more physics steps
				while (m_fTimeAccumulator > m_fPhysicsTimestep)
				{
					// Update the physics world with a constant timestep
					if (systemExists())
						m_pWorld->update(m_fPhysicsTimestep);
					// Decrease the accumulated time
					m_fTimeAccumulator -= m_fPhysicsTimestep;
				}
			}
			else // When not updating physics, use the thread to update objects
			{
				std::pair<std::shared_ptr<GameObject>, float> object;
				bool updating = false;

				{
					// Lock the mutex for accessing the object vector
					std::lock_guard<std::mutex> lock(m_objectMutex);
					if (!m_objectVector.empty()) // If the vector is not empty
					{
						object = m_objectVector.back();
						// Get and remove the back item in the vector
						m_objectVector.pop_back();

						updating = true; // There is an object to update
					}
				} // Release the mutex

				if (updating) // If there was an object in the vector
				{
					object.first->onUpdate(object.second); // Update it
				}
			}
			m_fUsedTimeAccumulator = m_fTimeAccumulator;
		}
	}

	void DynamicPhysicsSystem::update(float timestep)
	{
		if (!s_bSeparateThread) // If not on a separate thread, update the physics
		{
			m_fTimeAccumulator += timestep;

			// While there is enough accumulated time, take one or more physics steps
			while (m_fTimeAccumulator > m_fPhysicsTimestep)
			{
				// Update the physics world with a constant timestep
				m_pWorld->update(m_fPhysicsTimestep);
				// Decrease the accumulated time
				m_fTimeAccumulator -= m_fPhysicsTimestep;
			}

			m_fUsedTimeAccumulator = m_fTimeAccumulator;
		}

		std::vector<std::pair<std::shared_ptr<Component>, CollisionInfo>> collisions;
		{
			// Lock the mutex for accessing the collision vector
			std::lock_guard<std::mutex> lock(m_collisionMutex);
			while (!m_collisions.empty()) // If the vector is not empty
			{
				collisions.push_back(m_collisions.back());
				// Get and remove the back item in the vector
				m_collisions.pop_back();
			}
		} // Release the mutex

		if (!collisions.empty()) // If there was a collision in the vector
		{
			for (auto& collision : collisions)
			{
				ComponentMessage msg(ComponentMessageType::CollisionDetected, (void*)&collision.second);
				collision.first->receiveMessage(msg); // Send it to the component
			}
		}

		m_iCurrentRay = 0;
		for (auto& it : m_rays)
		{
			// Update all raycasts
			it.onUpdate(m_pWorld, &m_callbackClass);

			m_iCurrentRay++;
		}
	}

	void DynamicPhysicsSystem::addCollisionToVector(std::vector<std::shared_ptr<Component>>::iterator& compPointer, CollisionInfo info)
	{
		// Lock the mutex for accessing the collision vector
		std::lock_guard<std::mutex> lock(m_collisionMutex);
		// Add the component pointer and collision info to the vector
		m_collisions.insert(m_collisions.begin(), std::make_pair(*compPointer, info));
	}

	void DynamicPhysicsSystem::addObjectToUpdate(std::shared_ptr<GameObject> object, float timestep)
	{
		// Lock the mutex for accessing the object vector
		std::lock_guard<std::mutex> lock(m_objectMutex);
		// Add the object pointer and timestep to the vector
		m_objectVector.insert(m_objectVector.begin(), std::make_pair(object, timestep));
	}

	int DynamicPhysicsSystem::addRay(glm::vec3 startPoint, glm::vec3 distance)
	{
		// Add a ray to the vector
		m_rays.push_back(Ray(startPoint, distance));

		return m_rays.size() - 1;
	}

	int DynamicPhysicsSystem::addRay(GameObject* object, glm::vec3 offset, glm::vec3 distance)
	{
		// Add a ray to the vector
		m_rays.push_back(Ray(object, offset, distance));

		return m_rays.size() - 1;
	}

	RayHitInfo DynamicPhysicsSystem::castRay(glm::vec3 startPoint, glm::vec3 distance)
	{
		m_iCurrentRay = m_rays.size();
		m_rays.push_back(Ray(startPoint, distance));

		// Test ray cast
		m_rays.back().onUpdate(m_pThis->m_pWorld, &m_pThis->m_callbackClass);

		RayHitInfo hitInfo = m_rays.back().getHitInfo();

		m_rays.pop_back();

		// Return the info
		return hitInfo;
	}

	RayHitInfo DynamicPhysicsSystem::castRay(GameObject* object, glm::vec3 offset, glm::vec3 distance)
	{
		m_iCurrentRay = m_rays.size();
		m_rays.push_back(Ray(object, offset, distance));

		// Test ray cast
		m_rays.back().onUpdate(m_pThis->m_pWorld, &m_pThis->m_callbackClass);

		RayHitInfo hitInfo = m_rays.back().getHitInfo();

		m_rays.pop_back();

		// Return the info
		return hitInfo;
	}

	void DynamicPhysicsSystem::rayHitSomething(const rp3d::RaycastInfo& info)
	{
		// Call ray hit class on the ray that just hit something
		m_rays.at(m_iCurrentRay).rayHit(info);
	}
}