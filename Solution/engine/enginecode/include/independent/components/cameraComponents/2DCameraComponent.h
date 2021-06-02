/** \file 2DCameraComponent.h
*/
#pragma once

#include "components/cameraComponent.h"

#include "cameras/orthographicCamera2D.h"

namespace Engine
{
	/**
	\class CameraComponent 2D
	\brief Base class for all 2D camera components
	*/
	class CameraComponent2D : public CameraComponent
	{
	protected:
		std::shared_ptr<OrthographicCamera2D> m_camera; //!< Pointer to the camera
		glm::vec2 m_position = glm::vec2(0.f); //!< The cameras position, for calculations
		float m_fRotation; //!< The cameras rotation

		float m_fLeft; //!< The pixel position of the left of the camera
		float m_fTop; //!< The pixel position of the top of the camera
		float m_fWidth; //!< The width of the camera in pixels
		float m_fHeight; //!< The height of the camera in pixels
	public:
		//! Constructor
		/*!
		\param left The pixel position of the left of the camera
		\param top The pixel position of the top of the camera
		\param width The width of the camera in pixels
		\param height The height of the camera in pixels
		*/
		CameraComponent2D(float left, float top, float width, float height) :
			m_fLeft(left), m_fTop(top), m_fWidth(width), m_fHeight(height), m_position(glm::vec2(left, top)), m_fRotation(0) {}

		//! Function to get the camera
		/*!
		\return A pointer to the camera
		*/
		inline std::shared_ptr<Camera> getCamera() override { return m_camera; }

		//! Function to update the projection
		/*!
		\param left The pixel position of the left of the camera
		\param top The pixel position of the top of the camera
		\param width The pixel width of the camera
		\param height The pixel height of the camera
		*/
		void updateProjection(float left, float top, float width, float height) override
		{
			m_fLeft = left;
			m_fTop = top;
			m_fWidth = width;
			m_fHeight = height;

			m_position = glm::vec2(left, top);

			m_camera->reset(left, left + width, top + height, top);
			m_camera->setPositionAndRotation(m_position, m_fRotation);
		}
	};
}