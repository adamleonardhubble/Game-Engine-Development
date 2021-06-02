/** \file inputPoller.h
*/

#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	/**
	\class InputPoller
	\brief Base class for finding the state of inputs at any point in the program
	*/
	class InputPoller
	{
	private:
		static InputPoller* s_pInstance; //!< Pointer for singleton instance
	protected:
		//! API specific implementation of key press
		/*!
		\param keycode The code for the key being checked
		\return A boolean representing if the key is currently being pressed
		*/
		virtual bool isKeyPressedImpl(int keycode) = 0; 
		//! API specific implementation of mouse button press
		/*!
		\param button The code for the button being checked
		\return A boolean representing if the button is currently being pressed
		*/
		virtual bool isMouseButtonPressedImpl(int button) = 0; 
		//! API specific implementation of mouse position
		/*!
		\return The mouse position
		*/
		virtual glm::vec2 getMousePositionImpl() = 0; 
		//! API specific implementation of mouse x position
		/*!
		\return The mouse x position
		*/
		virtual float getMouseXImpl() = 0; 
		//! API specific implementation of mouse y postion
		/*!
		\return The mouse y position
		*/
		virtual float getMouseYImpl() = 0; 
	public:
		//! Return if the key with the key code is pressed
		/*!
		\param keycode The code for the key being checked
		\return A boolean representing if the key is currently being pressed
		*/
		inline static bool isKeyPressed(int keycode) { return s_pInstance->isKeyPressedImpl(keycode); } 
		//! Return if the mouse button is pressed
		/*!
		\param button The code for the button being checked
		\return A boolean representing if the button is currently being pressed
		*/
		inline static bool isMouseButtonPressed(int button) { return s_pInstance->isMouseButtonPressedImpl(button); } 
		//! Return the current position of the mouse
		/*!
		\return The mouse position
		*/
		inline static glm::vec2 getMousePosition() { return s_pInstance->getMousePositionImpl(); } 
		//! Return the current x position of the mouse
		/*!
		\return The mouse x position
		*/
		inline static float getMouseX() { return s_pInstance->getMouseXImpl(); } 
		//! Return the current y position of the mouse
		/*!
		\return The mouse y position
		*/
		inline static float getMouseY() { return s_pInstance->getMouseYImpl(); } 
	};
}