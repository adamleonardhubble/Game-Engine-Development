/** \file ray.h
*/
#pragma once

#include "components/positionComponent.h"

#include <reactphysics3d.h>
#include <glm/glm.hpp>

namespace Engine
{
	//! Info from a raycast hit
	struct RayHitInfo
	{
		bool hit = false;
		float hitDistance = 0.f;
		rp3d::Vector3 worldPointHit = rp3d::Vector3(0, 0, 0);

		std::string tagHit = "";
		std::string objectNameHit = "";
	};

	/**
	\class Ray
	\brief Used for raycasting from a lua script
	*/
	class Ray
	{
	private:
		rp3d::Ray* m_ray; //!< Pointer to a rp3d ray object

		bool m_bObjectPos; //!< If the ray is linked to an object position
		rp3d::Vector3 m_startPoint; //!< The start point of a ray if it is not linked to an object position
		rp3d::Vector3 m_distanceVector; //!< The direction the ray goes in

		std::shared_ptr<PositionComponent> m_position; //!< A pointer to the position component if the ray is linked to an object
		rp3d::Vector3 m_objectOffset; //!< The offset of the ray start point if the ray is linked to an object
		float m_fDistance;
		bool m_bRayOn; //!< If the ray is on

		RayHitInfo m_hitInfo; //!< The info from the ray hit
	public:
		//! Constructor for a raycast not linked to an object, the ray will be static
		/*!
		\param startPoint The start position of the ray
		\param distance The distance in all 3 axis for the ray to go
		*/
		Ray(glm::vec3 startPoint, glm::vec3 distance);
		//! Constructor for a raycast linked to an object, the ray start point will always be the objects position
		/*!
		\param object A pointer to the object the ray is linked to
		\param offset The offset from the object to the start point of the ray
		\param distance The distance in all 3 axis for the ray to go
		*/
		Ray(GameObject* object, glm::vec3 offset, glm::vec3 distance);

		//! Destructor
		~Ray();

		//! Called after every physics update to check if the ray has hit anything, and to move the ray if the object it is linked to has moved
		/*!
		\param world A pointer to the physics world
		\param callback The raycast callback class
		*/
		void onUpdate(rp3d::DynamicsWorld* world, rp3d::RaycastCallback* callback);

		//! Function to change the start position of the ray if it is not linked to an object
		/*!
		\param newStartPosition The new start position of the ray
		*/
		void changeStartPoint(glm::vec3 newStartPoint);
		//! Function to get the x component of the ray start point
		inline float getStartX() { return m_startPoint.x; }
		//! Function to get the y component of the ray start point
		inline float getStartY() { return m_startPoint.y; }
		//! Function to get the z component of the ray start point
		inline float getStartZ() { return m_startPoint.z; }
		//! Function to change the distance of the ray
		/*!
		\param newDistance The new distance of the ray
		*/
		void changeDistance(glm::vec3 newDistance);
		//! Function to get the distance of the ray
		glm::vec3 getDistance();
		//! Function to get the x component of the ray distance
		inline float getDistanceX() { return m_distanceVector.x; }
		//! Function to get the y component of the ray distance
		inline float getDistanceY() { return m_distanceVector.y; }
		//! Function to get the z component of the ray distance
		inline float getDistanceZ() { return m_distanceVector.z; }
		//! Function to get the length of the ray
		float getLength();

		//! Function to set if the ray is enabled or not
		/*!
		\param isEnabled If the ray is being turned on or off
		*/
		void enabled(bool isEnabled) { m_bRayOn = isEnabled; }
		//! Function to get the info for that ray
		/*!
		\return A struct with hit info, default values if the ray is off
		*/
		RayHitInfo getHitInfo();

		//! Fucntion called from the raycast callback class when a ray has hit something
		/*!
		\param info The info from the hit
		*/
		void rayHit(const rp3d::RaycastInfo& info);
	};
}