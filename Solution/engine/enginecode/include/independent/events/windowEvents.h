/** \file windowEvents.h
\brief Contains classes used for window events
*/

#pragma once

#include "events/event.h"

namespace Engine
{
	/**
	\class WindowCloseEvent
	\brief Event for if the window has been closed
	*/
	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {} //!< Constructor

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::WindowClose; }
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::WindowClose; }
		//! Used to get the category flags of the event
		/*!
		\return The category flags
		*/
		int getCategoryFlags() const override { return EventCategoryWindow; }
	};

	/**
	\class WindowResizeEvent
	\brief Event for if the size of the window has been changed
	*/
	class WindowResizeEvent : public Event
	{
	private:
		int m_iWidth; //!< Width of the window
		int m_iHeight; //!< Height of the window
	public:
		//! Constructor, sets the width and height to the ones passed in
		/*!
		\param width The new width of the window
		\param height The new height of the window
		*/
		WindowResizeEvent(int width, int height) : m_iWidth(width), m_iHeight(height) {}

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::WindowResize; }
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::WindowResize; }
		//! Used to get the category flags of the event
		/*!
		\return The category flags
		*/
		int getCategoryFlags() const override { return EventCategoryWindow; }

		//! Used to get the width
		/*!
		\return The width of the window
		*/
		inline int getWidth() const { return m_iWidth; }
		//! Used to get the height
		/*!
		\return The height of the window
		*/
		inline int getHeight() const { return m_iHeight; }
	};

	/**
	\class WindowFocusEvent
	\brief Event for if the window is being focused on 
	*/
	class WindowFocusEvent : public Event
	{
	private:
		float m_fXPos; //!< The x position
		float m_fYPos; //!< The y position
	public:
		//! Constructor, sets the x and y positions to the ones passed in
		/*!
		\param xPos The new x position
		\param yPos The new y position
		*/
		WindowFocusEvent(float xPos, float yPos) : m_fXPos(xPos), m_fYPos(yPos) {}

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::WindowFocus; }
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::WindowFocus; }
		//! Used to get the category flags of the event
		/*!
		\return The category flags
		*/
		int getCategoryFlags() const override { return EventCategoryWindow; }

		//! Used to get the x position
		/*!
		\return The x position
		*/
		float getXPos() { return m_fXPos; }
		//! Used to get the y position
		/*!
		\return The y position
		*/
		float getYPos() { return m_fYPos; }
	};

	/**
	\class WindowLostFocusEvent
	\brief Event for if the window is not being focused on
	*/
	class WindowLostFocusEvent : public Event
	{
	private:
		float m_fXPos; //!< The x position
		float m_fYPos; //!< The y position
	public:
		//! Constructor, sets the x and y positions to the ones passed in
		/*!
		\param xPos The new x position
		\param yPos The new y position
		*/
		WindowLostFocusEvent(float xPos, float yPos) : m_fXPos(xPos), m_fYPos(yPos) {}

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::WindowLostFocus; }
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::WindowLostFocus; }
		//! Used to get the category flags of the event
		/*!
		\return The category flags
		*/
		int getCategoryFlags() const override { return EventCategoryWindow; }

		//! Used to get the x position
		/*!
		\return The x position
		*/
		float getXPos() { return m_fXPos; }
		//! Used to get the y position
		/*!
		\return The y position
		*/
		float getYPos() { return m_fYPos; }
	};

	/**
	\class WindowMovedEvent
	\brief Event for if the window is being moved
	*/
	class WindowMovedEvent : public Event
	{
	private:
		int m_iXPos; //!< The new x position
		int m_iYPos; //!< The new y position
	public:
		//! Constructor, sets the x and y positions to the ones passed in
		/*!
		\param xPos The new x position of the window
		\param yPos The new y position of the window
		*/
		WindowMovedEvent(int xPos, int yPos) : m_iXPos(xPos), m_iYPos(yPos) {}

		//! Used to get the event type even if the event has not been created
		/*!
		\return The event type
		*/
		static EventType getStaticType() { return EventType::WindowMoved; }
		//! Used to get the event type
		/*!
		\return The event type
		*/
		EventType getEventType() const override { return EventType::WindowMoved; }
		//! Used to get the category flags of the event
		/*!
		\return The category flags
		*/
		int getCategoryFlags() const override { return EventCategoryWindow; }

		//! Used to get the x position of the window
		/*!
		\return The x position of the window
		*/
		int getXPos() { return m_iXPos; }
		//! Used to get the y position of the window
		/*!
		\return The y position of the window
		*/
		int getYPos() { return m_iYPos; }
	};
}