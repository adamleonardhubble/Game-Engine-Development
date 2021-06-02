/** \file 3DCameraComponent.h
*/
#pragma once

#include "components/cameraComponent.h"

#include "cameras/perspectiveEulerCamera3D.h"

namespace Engine
{
	/**
	\class CameraComponent3D
	\brief Base class for all 3D camera components
	*/
	class CameraComponent3D : public CameraComponent
	{
	protected:
		std::shared_ptr<PerspectiveEulerCamera3D> m_camera; //!< Pointer to the camera
		glm::mat4 m_view; //!< The cameras view, for calculations
		glm::vec3 m_position; //!< The cameras position, for calculations
		glm::vec3 m_forward; //!< The cameras forward axis
		glm::vec3 m_up; //!< The cameras up axis
		glm::vec3 m_right; //!< The cameras right axis
		glm::vec3 m_worldUp; //!< The world up axis

		float m_fYaw; //!< The cameras yaw
		float m_fPitch; //!< The cameras pitch
		float m_fTranslationSpeed = 5.f; //!< The speed the camera will move
		float m_fRotationSpeed = 4.f; //!< The speed the camera will rotate
		glm::vec2 m_lastMousePosition; //!< The previous mouse position, for using the mouse to rotate the camera

		float m_fFOV; //!< The field of view
		float m_fAspectRatio; //!< The aspect ratio
		float m_fNearClip; //!< The near clip plane distance
		float m_fFarClip; //!< The far clip plane distance

		bool m_bUpdateOnMousePress; //!< If the camera will rotate when the mouse button is pressed or not

		bool m_bMouseButtonPressed = false; //!< If the mouse button is pressed
	public:
		//! Constructor
		/*!
		\param fov The new field of view
		\param aspectRatio The new aspectRatio
		\param nearClip The new nearClip
		\param farClip The new farClip
		\param position The new position
		\param rotation The new rotation
		\param updateMousePress The new value
		*/
		CameraComponent3D(float fov, float aspectRatio, float nearClip, float farClip, glm::vec3 position, glm::vec2 rotation, bool updateMousePress) : 
			m_fFOV(fov), m_fAspectRatio(aspectRatio), m_fNearClip(nearClip), m_fFarClip(farClip), m_position(position), 
			m_fPitch(rotation.x), m_fYaw(rotation.y), m_bUpdateOnMousePress(updateMousePress) {}

		//! Function to get the camera
		/*!
		\return A pointer to the camera
		*/
		inline std::shared_ptr<Camera> getCamera() override { return m_camera; }

		//! Function to get the camera fov
		/*!
		\return The fov
		*/
		inline float getFOV() override { return m_fFOV; }
		//! Function to get the camera aspect ratio
		/*!
		\return The aspect ratio
		*/
		inline float getAspectRatio() override { return m_fAspectRatio; }
		//! Function to get the camera near clip distance
		/*!
		\return The near clip
		*/
		inline float getNearClip() override { return m_fNearClip; }
		//! Function to get the camera far clip distance
		/*!
		\return The far clip
		*/
		inline float getFarClip() override { return m_fFarClip; }
		
		//! Function to get if the camera will rotate when the mouse button is pressed or not
		/*!
		\reutrn The bool
		*/
		inline bool getUpdateMousePress() override { return m_bUpdateOnMousePress; }

		//! Function to update the projection
		/*!
		\param fov The new fov
		\param nearClip The new near clip
		\param farClip The new far clip
		*/
		void updateProjection(float fov, float nearClip, float farClip) override 
		{ 
			m_fFOV = fov;
			m_fNearClip = nearClip;
			m_fFarClip = farClip;
			m_camera->setNewProjection(fov, m_fAspectRatio, nearClip, farClip); 
		}
		//! Function to set if the camera rotates when the mouse button is pressed
		void setUpdateMousePress(bool update) override { m_bUpdateOnMousePress = update; }
	};
}