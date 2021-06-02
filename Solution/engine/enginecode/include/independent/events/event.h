/** \file event.h
\brief Contains classes and enums used for events
*/

#pragma once

#include <functional>

namespace Engine
{
	/**
	\class EventType
	\brief Enum for all the types of event
	*/
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory //! Categories for events. Bit shifting to give each category a bit pattern
	{
		None = 0,                          // Bit patterns
		EventCategoryWindow = 1 << 0,      // 00000001
		EventCategoryInput = 1 << 1,       // 00000010
		EventCategoryKeyboard = 1 << 2,    // 00000100
		EventCategoryMouse = 1 << 3,       // 00001000
		EventCategoryMouseButton = 1 << 4  // 00010000
	};

	/**
	\class Event
	\brief Base class for all events.

	All events will inherit from this class, an abstract class with pure virtual functions which have to be implemented
	in the classes which inherit from it
	*/
	class Event
	{
	private:
		bool m_bHandled = false; //!< If the event has been handled

		bool m_bOverGame = false; //!< If the game should receive the event even though the mouse is over an ImGui window
	public:
		//! To be overridden, used to get the type of the event
		/*!
		\return The event type
		*/
		virtual EventType getEventType() const = 0; 
		//! To be overridden, used to get the categories of the event
		/*!
		\return The event category
		*/
		virtual int getCategoryFlags() const = 0; 
		//! Returns a boolean representing if the event has been handled or not
		/*!
		\return If the event has been handled
		*/
		inline bool handled() const { return m_bHandled; } 
		//! Set if the event has been handled
		/*!
		\param isHandled Boolean to set the stored handled bool to
		*/
		inline void handle(bool isHandled) { m_bHandled = isHandled; } 

		//! Function to set the bool
		/*!
		\param overGame The value
		*/
		void setOverGame(bool overGame) { m_bOverGame = overGame; }
		//! Function to get the bool
		/*!
		\return The bool
		*/
		inline bool getOverGame() { return m_bOverGame; }

		//! Check if an event category is one of the categories of the event
		/*!
		\param category The category being checked
		\return A boolean representing if it is or not
		*/
		inline bool isInCategory(EventCategory category) { return getCategoryFlags()& category; } 
	};

	/**
	\class EventDispatcher
	\brief Dispatches events to boolean functions which take a reference to the event
	*/
	class EventDispatcher
	{
		template<typename T>
		using EventFunc = std::function<bool(T&)>; //!< EventFunc is a function which returns a boolean and takes a reference to template type T
	private:
		Event& m_event; //!< Reference to the Event
	public:
		//! Constructor, set the stored event reference to the one passed in
		/*!
		\param event A reference to the Event class being dispatched
		*/
		EventDispatcher(Event& event) : m_event(event) {} 

		//! Function comparing the type of the stored event with the type of the event passed in
		/*!
		\param func The function to call when handling the event
		*/
		template<typename T>
		bool dispatch(EventFunc<T> func)
		{
			// If the event types match
			if (m_event.getEventType() == T::getStaticType())
			{
				// Handle the event
				m_event.handle(func(*((T*)&m_event)));
				return true;
			}
			// The event types didn't match
			return false;
		}
	};
}