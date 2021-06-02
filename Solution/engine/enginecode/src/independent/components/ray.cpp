/** \file ray.cpp
*/
#include "engine_pch.h"

#include "components/ray.h"

namespace Engine
{
	Ray::Ray(glm::vec3 startPoint, glm::vec3 distance) : m_bObjectPos(false),
		m_startPoint(rp3d::Vector3(startPoint.x, startPoint.y, startPoint.z)), m_bRayOn(true)
	{
		// Set the distance
		m_distanceVector = rp3d::Vector3(distance.x, distance.y, distance.z);
		m_fDistance = glm::sqrt((m_distanceVector.x * m_distanceVector.x) + (m_distanceVector.y * m_distanceVector.y) + (m_distanceVector.z * m_distanceVector.z));

		// Find the end point
		rp3d::Vector3 endPoint = m_startPoint + m_distanceVector;
		// Make the ray
		m_ray = new rp3d::Ray(m_startPoint, endPoint);
	}

	Ray::Ray(GameObject* object, glm::vec3 offset, glm::vec3 distance) : m_bObjectPos(true),
		m_objectOffset(rp3d::Vector3(offset.x, offset.y, offset.z)), m_bRayOn(true)
	{
		// Set the distance
		m_distanceVector = rp3d::Vector3(distance.x, distance.y, distance.z);
		m_fDistance = glm::sqrt((m_distanceVector.x * m_distanceVector.x) + (m_distanceVector.y * m_distanceVector.y) + (m_distanceVector.z * m_distanceVector.z));

		// Get the position component of the object
		auto pos = object->getComponent<PositionComponent>();
		m_position = std::dynamic_pointer_cast<PositionComponent>(*pos);

		// Get the start point for the ray
		glm::vec3 startPoint = m_position->getPosition();
		m_startPoint = rp3d::Vector3(startPoint.x, startPoint.y, startPoint.z) + m_objectOffset;

		// Find the end point
		rp3d::Vector3 endPoint = m_startPoint + m_distanceVector;
		// Make the ray
		m_ray = new rp3d::Ray(m_startPoint, endPoint);
	}

	Ray::~Ray()
	{
		//delete m_ray;
		//m_ray = nullptr;
	}

	void Ray::onUpdate(rp3d::DynamicsWorld* world, rp3d::RaycastCallback* callback)
	{
		// Reset hit info
		m_hitInfo = RayHitInfo();
		// If the ray is linked to an object
		if (m_bObjectPos)
		{
			// Get the new start position for the ray
			glm::vec3 startPoint = m_position->getPosition();
			m_startPoint = rp3d::Vector3(startPoint.x, startPoint.y, startPoint.z) + m_objectOffset;

			// Find the end point
			rp3d::Vector3 endPoint = m_startPoint + m_distanceVector;
			// Set the new positions
			m_ray->point1.setAllValues(m_startPoint.x, m_startPoint.y, m_startPoint.z);
			m_ray->point2.setAllValues(endPoint.x, endPoint.y, endPoint.z);
		}
		// Perform the raycast test
		world->raycast(*m_ray, callback);
	}

	void Ray::changeStartPoint(glm::vec3 newStartPoint)
	{
		// If not linked to an object
		if (!m_bObjectPos)
		{
			// Set the start point
			m_startPoint = rp3d::Vector3(newStartPoint.x, newStartPoint.y, newStartPoint.z);
			// Find the end point
			rp3d::Vector3 endPoint = m_startPoint + m_distanceVector;
			// Set the new positions
			m_ray->point1.setAllValues(m_startPoint.x, m_startPoint.y, m_startPoint.z);
			m_ray->point2.setAllValues(endPoint.x, endPoint.y, endPoint.z);
		}
	}

	void Ray::changeDistance(glm::vec3 newDistance)
	{
		// Set the new direction
		m_distanceVector = rp3d::Vector3(newDistance.x, newDistance.y, newDistance.z);
		// Find the end point
		rp3d::Vector3 endPoint = m_startPoint + m_distanceVector;
		// Set the new end point
		m_ray->point2.setAllValues(endPoint.x, endPoint.y, endPoint.z);
		// Get New Float Distance
		m_fDistance = glm::sqrt((m_distanceVector.x * m_distanceVector.x) + (m_distanceVector.y * m_distanceVector.y) + (m_distanceVector.z * m_distanceVector.z));
	}

	glm::vec3 Ray::getDistance()
	{
		return glm::vec3(m_distanceVector.x, m_distanceVector.y, m_distanceVector.z);
	}

	float Ray::getLength()
	{
		return m_fDistance;
	}

	RayHitInfo Ray::getHitInfo()
	{
		// If the ray is on
		if (m_bRayOn)
			return m_hitInfo; // Return the hit info
		else // If the ray is off
		{
			return RayHitInfo(); // Return default values
		}
	}

	void Ray::rayHit(const rp3d::RaycastInfo& info)
	{
		// Make hit info struct with the new info
		m_hitInfo = RayHitInfo();
		m_hitInfo.hit = true;
		m_hitInfo.worldPointHit = info.worldPoint;

		m_hitInfo.hitDistance = glm::distance(glm::vec3(m_startPoint.x, m_startPoint.y, m_startPoint.z), 
			glm::vec3(m_hitInfo.worldPointHit.x, m_hitInfo.worldPointHit.y, m_hitInfo.worldPointHit.z));

		GameObject* objectHit = (GameObject*)info.body->getUserData();

		m_hitInfo.tagHit = objectHit->tag();
		m_hitInfo.objectNameHit = objectHit->name();
	}
}