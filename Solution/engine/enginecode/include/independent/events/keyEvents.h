/** \file keyEvents.h
\brief Contains classes used for key events
*/

#pragma once

#include "events/event.h"

namespace Engine
{
	/**
	\class KeyEvent
	\brief Base class inheriting from Event, for all key events

	Contains common attributes for all key events
	*/
	class KeyEvent : public Event
	{
	protected:
		int m_iKeyCode; //!< Identifying number for which key is being used

		//! Constructor, sets stored key code to the one passed in. Protected so only classes inheriting from the class can create it
		/*!
		\param keyCode The code of the key pressed/released
		*/
		KeyEvent(int keyCode) : m_iKeyCode(keyCode) {} 
	public:
		//! Used to get the code of the key pressed/released
		/*!
		\return The key code
		*/
		inline int getKeyCode() const { return m_iKeyCode; } 

		//! Used to get the category flags of the event
		/*!
		\return The category flags
		*/
		int getCategoryFlags() const override { return EventCategoryInput | EventCategoryKeyboard; } 
	};


	/**
	\class KeyPressedEvent
	\brief Event for if a key has been pressed
	*/
	class KeyPressedEvent : public KeyEvent
	{
	private:
		int m_iRepeated; //!< If the key press is a repeat and if so how many times has it been pressed
	public:
		//! Constructor, constructs the base class and sets the repeat count
		/*!
		\param keyCode The key code of the key that has been pressed
		\param repeatCount If the key press is a repeat
		*/
		KeyPressedEvent(int keyCode, int repeatCount) : KeyEvent(keyCode), m_iRepeated(repeatCount) {} 

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::KeyPressed; } 
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::KeyPressed; }

		//! Used to see if the key has been repeatedly pressed
		/*!
		\return A boolean representing if the key press is a repeat press or not
		*/
		inline bool getRepeated() const { return m_iRepeated; } 
	};

	/**
	\class KeyReleasedEvent
	\brief Event for if a key has been released
	*/
	class KeyReleasedEvent : public KeyEvent
	{
	public:
		//! Constructor, constructs the base class
		/*!
		\param keyCode The key code of the key that has been pressed
		*/
		KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::KeyReleased; }
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::KeyReleased; }
	};

	/**
	\class KeyTypedEvent
	\brief Event for if a key has been typed
	*/
	class KeyTypedEvent : public KeyEvent
	{
	public:
		//! Constructor, constructs the base class
		/*!
		\param keyCode The key code of the key that has been pressed
		*/
		KeyTypedEvent(int keyCode) : KeyEvent(keyCode) {}

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::KeyTyped; }
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::KeyTyped; }
	};
}