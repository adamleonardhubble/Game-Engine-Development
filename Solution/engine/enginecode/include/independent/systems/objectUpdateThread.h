/** \file objectUpdateThread.h
*/
#pragma once

#include "system.h"
#include "components/CGO.h"

#include <thread>
#include <mutex>

namespace Engine
{
	/**
	\class ObjectUpdateThread
	\brief System controlling multiple threads used to update objects faster
	*/
	class ObjectUpdateThread : public System
	{
	private:
		std::vector<std::pair<std::shared_ptr<GameObject>, float>> m_objectVector; //!< Vector of pointers to objects to update, paired with the timestep to update with

		std::thread m_thread; //!< The threads to use
		std::mutex m_vectorMutex; //!< A mutex to control access to the object vector

		static bool s_bInstanceFlag; //!< A boolean for if the system is running
		static std::shared_ptr<ObjectUpdateThread> s_pThis; //!< Pointer to thread object

		bool m_bHasThread; //!< If a separate thread is being used

		void run(); //!< The function for the threads to run
	public:
		//! Create instance of object and return a pointer to it
		/*!
		\return A pointer to the instance of the object
		*/
		static std::shared_ptr<ObjectUpdateThread> getInstance();

		void start(SystemSignal init = SystemSignal::None, ...) override; //!< A function to start the logger
		void stop(SystemSignal close = SystemSignal::None, ...) override; //!< A function to stop the logger

		//! Function to add an object and timestep to the vector
		/*!
		\param object A pointer to the object needing to be updated
		\param timestep The timestep to update the object with
		*/
		void addObject(std::shared_ptr<GameObject> object, float timestep);

		//! Function to check if a separate thread has been made for updating objects
		/*!
		\return A bool for if there is a separate thread
		*/
		inline bool separateThreadForObjectUpdates() { return m_bHasThread; }

		//! Called to check if the system exists
		/*!
		\return The bool for if the system exists or not
		*/
		static bool systemExists() { return s_bInstanceFlag; }
	};
}