/** \file mouseEvents.h
\brief Contains classes used for mouse events
*/

#pragma once

#include "events/event.h"

namespace Engine
{
	/**
	\class MouseButtonEvent
	\brief Base class inheriting from Event, for all mouse button events

	Contains common attributes for all mouse button events
	*/
	class MouseButtonEvent : public Event
	{
	protected:
		int m_iButton; //!< Identifying number for which button is being used

		//! Constructor, sets stored button to the one passed in. Protected so only classes inheriting from the class can create it
		/*!
		\param button The code of the button pressed/released
		*/
		MouseButtonEvent(int button) : m_iButton(button) {} 
	public:
		//! Used to get the code of the button pressed/released
		/*!
		\return The button code
		*/
		inline int getMouseButton() const { return m_iButton; }

		//! Used to get the category flags of the event
		/*!
		\return The category flags
		*/
		int getCategoryFlags() const override { return EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton; } //!< Returns the category flags
	};


	/**
	\class MouseButtonPressedEvent
	\brief Event for if a mouse button has been pressed
	*/
	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		//! Constructor, constructs the base class
		/*!
		\param button The button code of the button that has been pressed
		*/
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::MouseButtonPressed; }
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::MouseButtonPressed; }
	};

	/**
	\class MouseButtonReleasedEvent
	\brief Event for if a mouse button has been released
	*/
	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		//! Constructor, constructs the base class
		/*!
		\param button The button code of the button that has been pressed
		*/
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::MouseButtonReleased; }
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::MouseButtonReleased; }
	};

	/**
	\class MouseMovedEvent
	\brief Event for if the mouse has been moved
	*/
	class MouseMovedEvent : public Event
	{
	private:
		float m_fMouseX; //!< The new mouse x position
		float m_fMouseY; //!< The new mouse y position
	public:
		//! Constructor, sets the mouse x and y positions to the ones passed in
		/*!
		\param x The new mouse x position
		\param y The new mouse y position
		*/
		MouseMovedEvent(float x, float y) : m_fMouseX(x), m_fMouseY(y) {}

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::MouseMoved; }
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::MouseMoved; }
		//! Used to get the category flags of the event
		/*!
		\return The category flags
		*/
		int getCategoryFlags() const override { return EventCategoryInput | EventCategoryMouse; }

		//! Used to get the new mouse x position
		/*!
		\return The new mouse x position
		*/
		inline float getX() const { return m_fMouseX; } 
		//! Used to get the new mouse y position
		/*!
		\return The new mouse y position
		*/
		inline float getY() const { return m_fMouseY; }
	};

	/**
	\class MouseScrolledEvent
	\brief Event for if the user scrolls with the mouse
	*/
	class MouseScrolledEvent : public Event
	{
	private:
		float m_fXOffset; //!< The x offset
		float m_fYOffset; //!< The y offset
	public:
		//! Constructor, sets the x and y offsets to the ones passed in
		/*!
		\param x The x offset
		\param y The y offset
		*/
		MouseScrolledEvent(float x, float y) : m_fXOffset(x), m_fYOffset(y) {}

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::MouseScrolled; }
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::MouseScrolled; }
		//! Used to get the category flags of the event
		/*!
		\return The category flags
		*/
		int getCategoryFlags() const override { return EventCategoryInput | EventCategoryMouse; }

		//! Used to get the x offset
		/*!
		\return The x offset
		*/
		inline float getXOffset() const { return m_fXOffset; }
		//! Used to get the y offset
		/*!
		\return The y offset
		*/
		inline float getYOffset() const { return m_fYOffset; }
	};
}