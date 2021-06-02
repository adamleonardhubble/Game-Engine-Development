/** \file fixedOrthoCameraComponent.h
*/
#pragma once

#include "2DCameraComponent.h"

namespace Engine
{
	/**
	\class FixedOrthoCameraComponent
	\brief A fixed 2D camera component
	*/
	class FixedOrthoCameraComponent : public CameraComponent2D
	{
	public:
		//! Constructor
		/*!
		\param left The pixel position of the left of the camera
		\param top The pixel position of the top of the camera
		\param width The width of the camera in pixels
		\param height The height of the camera in pixels
		*/
		FixedOrthoCameraComponent(float left, float top, float width, float height);

		//! Function to set the camera position and rotation
		/*!
		\param position The new position
		\param rotation The new rotation
		*/
		void setPosAndRot(glm::vec3 position, glm::vec2 rotation) override;
	};
}