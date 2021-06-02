/** \file camera.h
*/
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
	/**
	\class Camera
	\brief Base class for all types of camera to inherit from
	*/
	class Camera
	{
	protected:
		glm::mat4 m_projection; //!< The camera projection
		glm::mat4 m_view; //!< The cameras view
		glm::mat4 m_viewProjection; //!< The camera projection multiplied by the view
	public:
		//! Function to update the view of the camera, to be implemented in the camera classes
		virtual void updateView() = 0;
		//! Function to get the projection
		/*!
		\return The camera projection
		*/
		const glm::mat4& getProjection() { return m_projection; }
		//! Function to get the view
		/*!
		\return The cameras view
		*/
		const glm::mat4& getView() { return m_view; }
		//! Function to get the viewProjection
		/*!
		\return The camera view projection matrix
		*/
		const glm::mat4& getViewProjection() { return m_viewProjection; }
		//! Function to get the position of the camera in the 3D world space
		/*!
		\return The position of the camera
		*/
		virtual glm::vec3 getPosition() const = 0;
	};
}