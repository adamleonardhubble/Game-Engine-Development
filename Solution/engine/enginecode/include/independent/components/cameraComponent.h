/** \file cameraComponent.h
*/
#pragma once

#include "CGO.h"

#include "cameras/camera.h"

namespace Engine
{
	/**
	\class CameraComponent
	\brief Base class for all the different camera components that can be added to a gameobject
	*/
	class CameraComponent : public Component
	{
	private:
		std::vector<std::string> m_layersSeen; //!< A vector containing names of all the layers that the camera can see
	protected:
		//! Function to send a message to another component on the gameobject
		/*!
		\param msg The message being sent
		*/
		void sendMessage(const ComponentMessage& msg) override
		{
			// Send the message to all components on the object
			for (auto it = m_pOwner->begin(); it != m_pOwner->end(); it++)
			{
				(*it)->receiveMessage(msg);
			}
		}
	public:
		//! Function to get the camera
		/*!
		\return A pointer to the camera
		*/
		virtual std::shared_ptr<Camera> getCamera() = 0;
		//! Function to update the cameras view
		virtual void updateView() {}
		//! Function to set the camera position and rotation
		/*!
		\param position The new position
		\param rotation The new rotation
		*/
		virtual void setPosAndRot(glm::vec3 position, glm::vec2 rotation) = 0;

		//! Function to get the camera fov
		/*!
		\return The fov
		*/
		virtual float getFOV() { return 0; }
		//! Function to get the camera aspect ratio
		/*!
		\return The aspect ratio
		*/
		virtual float getAspectRatio() { return 0; }
		//! Function to get the camera near clip distance
		/*!
		\return The near clip
		*/
		virtual float getNearClip() { return 0; }
		//! Function to get the camera far clip distance
		/*!
		\return The far clip
		*/
		virtual float getFarClip() { return 0; }

		//! Function to get if the camera will rotate when the mouse button is pressed or not
		/*!
		\reutrn The bool
		*/
		virtual bool getUpdateMousePress() { return false; }

		//! Function to update the projection
		/*!
		\param fov The new fov
		\param nearClip The new near clip
		\param farClip The new far clip
		*/
		virtual void updateProjection(float fov, float nearClip, float farClip) {}

		//! Function to update the projection
		/*!
		\param left The pixel position of the left of the camera
		\param top The pixel position of the top of the camera
		\param width The pixel width of the camera
		\param height The pixel height of the camera
		*/
		virtual void updateProjection(float left, float top, float width, float height) {}

		//! Function to set if the camera rotates when the mouse button is pressed
		virtual void setUpdateMousePress(bool update) {}

		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		virtual void receiveMessage(const ComponentMessage& msg) override {}

		//! Function to set which layers the camera can see
		/*!
		\param layers A new vector with the layers that the camera can now see
		*/
		void setLayersSeen(const std::vector<std::string>& layers) { m_layersSeen = layers; }
		//! Function to get which layers the camera can see
		/*!
		\return A vector with all the layers the camera can see
		*/
		std::vector<std::string>& getLayersSeen() { return m_layersSeen; }
	};
}