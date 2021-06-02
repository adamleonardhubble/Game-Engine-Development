/** \file objectUpdateThread.cpp
*/
#include "engine_pch.h"

#include "systems/objectUpdateThread.h"

#include "systems/dynamicPhysicsSystem.h"

namespace Engine
{
	bool ObjectUpdateThread::s_bInstanceFlag = false; // Initialise the instance flag as false
	std::shared_ptr<ObjectUpdateThread> ObjectUpdateThread::s_pThis = nullptr; // Initialise the pointer to this as null

	std::shared_ptr<ObjectUpdateThread> ObjectUpdateThread::getInstance()
	{
		// If there is no instance of the object
		if (!s_bInstanceFlag)
		{
			// Create a new object
			s_pThis = std::shared_ptr<ObjectUpdateThread>(new ObjectUpdateThread());
			s_bInstanceFlag = true; // There is an instance of the object

			return s_pThis; // Return the pointer to the object
		}
		else // If the object already exists
			return s_pThis; // Return the pointer to the object
	}

	void ObjectUpdateThread::start(SystemSignal init, ...)
	{
		va_list args;
		va_start(args, init);
		// Get the number of threads from variadic arguments
		int threads = va_arg(args, int);
		// If more than 4 threads, objects are updated on a separate thread
		m_bHasThread = threads > 4;

		// If using a separate thread
		if (m_bHasThread)
			m_thread = std::thread(&ObjectUpdateThread::run, s_pThis); // Create the thread, using the run function
	}

	void ObjectUpdateThread::stop(SystemSignal close, ...)
	{
		s_bInstanceFlag = false; // No longer an instance to the object

		if (m_bHasThread)
			m_thread.join();

		s_pThis = nullptr; // Set pointer to itself to null
	}

	void ObjectUpdateThread::run()
	{
		// While the system is running, all threads continuously run this loop
		while (s_bInstanceFlag)
		{
			std::pair<std::shared_ptr<GameObject>, float> object;
			bool updating = false;

			{
				// Lock the mutex for accessing the object vector
				std::lock_guard<std::mutex> lock(m_vectorMutex);
				if (!m_objectVector.empty()) // If the vector is not empty
				{
					object = m_objectVector.back();
					// Get and remove the back item in the vector
					m_objectVector.pop_back();

					updating = true; // There is an object to update
				}
			} // Release the mutex

			if (updating) // If there was an object in the vector
			{
				object.first->onUpdate(object.second); // Update it
			}
		}
	}

	void ObjectUpdateThread::addObject(std::shared_ptr<GameObject> object, float timestep)
	{
		// Lock the mutex for accessing the object vector
		std::lock_guard<std::mutex> lock(m_vectorMutex);
		// Add the object pointer and timestep to the vector
		m_objectVector.insert(m_objectVector.begin(), std::make_pair(object, timestep));
	}
}