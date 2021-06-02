/** \file GLFWInputPoller.h
*/
#pragma once

#include "windows/inputPoller.h"

namespace Engine
{
	/**
	\class GLFWInputPoller
	\brief GLFW API specific input polling
	*/
	class GLFWInputPoller : public InputPoller
	{
	private:
		//! Windows API specific implementation of key press
		/*!
		\param keycode The code for the key being checked
		\return A boolean representing if the key is currently being pressed
		*/
		virtual bool isKeyPressedImpl(int keycode) override;
		//! Windows API specific implementation of mouse button press
		/*!
		\param button The code for the button being checked
		\return A boolean representing if the button is currently being pressed
		*/
		virtual bool isMouseButtonPressedImpl(int button) override;
		//! Windows API specific implementation of mouse position
		/*!
		\return The mouse position
		*/
		virtual glm::vec2 getMousePositionImpl() override;
		//! Windows API specific implementation of mouse x position
		/*!
		\return The mouse x position
		*/
		virtual float getMouseXImpl() override;
		//! Windows API specific implementation of mouse y postion
		/*!
		\return The mouse y position
		*/
		virtual float getMouseYImpl() override;
	};
}