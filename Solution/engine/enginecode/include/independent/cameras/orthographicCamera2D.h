/** \file orthographicCamera2D.h
*/
#pragma once

#include "camera.h"

namespace Engine
{
	/**
	\class OrthographicCamera2D
	\brief 2 dimensional camera to view UI elements
	*/
	class OrthographicCamera2D : public Camera
	{
	private:
		glm::vec2 m_position = glm::vec2(0.f); //!< The position in the window
		float m_fRotation = 0.f; //!< The rotation

		//! Function to update the view of the camera
		void updateView() override;
	public:
		//! Constructor
		/*!
		\param left The pixel position of the left of the camera
		\param right The pixel position of the right of the camera
		\param bottom The pixel position of the bottom of the camera
		\param top The pixel position of the top of the camera
		*/
		OrthographicCamera2D(float left, float right, float bottom, float top);

		//! Function to reset the camera
		/*!
		\param left The pixel position of the left of the camera
		\param right The pixel position of the right of the camera
		\param bottom The pixel position of the bottom of the camera
		\param top The pixel position of the top of the camera
		*/
		void reset(float left, float right, float bottom, float top);

		//! Function to get the position of the camera
		/*!
		\return The position of the camera
		*/
		inline glm::vec3 getPosition() const override { return glm::vec3(m_position, 0.f); }
		//! Function to get the rotation of the camera
		/*!
		\return The rotation of the camera
		*/
		inline float getRotation() const { return m_fRotation; }

		//! Function to set the position of the camera
		/*!
		\param position The new position
		*/
		void setPosition(const glm::vec2& position) { m_position = position; updateView(); }
		//! Function to set the rotation of the camera
		/*!
		\param rotation The new rotation
		*/
		void setRotation(float rotation) { m_fRotation = rotation; updateView(); }
		//! Function to set the position and rotation of the camera
		/*!
		\param position The new position
		\param rotation The new rotation
		*/
		void setPositionAndRotation(const glm::vec2& position, float rotation) 
		{ 
			m_position = position; 
			m_fRotation = rotation; 
			updateView(); 
		}
	};
}