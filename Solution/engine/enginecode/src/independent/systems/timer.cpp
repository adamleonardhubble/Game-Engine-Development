/** \file timer.cpp
*/

#include "engine_pch.h"

#include "systems/timer.h"

namespace Engine
{
	bool Timer::s_bInstanceFlag = false; // Initialise instance flag as false
	std::shared_ptr<Timer> Timer::s_pThis = nullptr; // Initialise pointer to this object as null

	// Initialize the frame timer
	std::chrono::high_resolution_clock::time_point Timer::s_frameStart;
	std::chrono::high_resolution_clock::time_point Timer::s_frameEnd;
	std::chrono::duration<float> Timer::s_frameDuration;

	std::chrono::high_resolution_clock::time_point Timer::s_physicsStart;
	std::chrono::high_resolution_clock::time_point Timer::s_physicsEnd;
	std::chrono::duration<float> Timer::s_physicsDuration;

	// Initialize the other timer
	std::map<std::string, std::pair<std::chrono::high_resolution_clock::time_point, std::chrono::high_resolution_clock::time_point>> Timer::s_sectionTimer;
	std::chrono::duration<float> Timer::s_sectionTimeDuration;

#ifdef NG_DEBUG // Only in debug mode
	std::map<std::string, std::vector<float>> Timer::s_debugProcessTimer; // Intialise the process profiling timer map
#endif

	std::shared_ptr<Timer> Timer::getInstance()
	{
		// If there is no instance of the object
		if (!s_bInstanceFlag)
		{
			// Create a new object
			s_pThis = std::shared_ptr<Timer>(new Timer());
			s_bInstanceFlag = true; // There is an instance of the object

			return s_pThis; // Return the pointer to the object
		}
		else // If the object already exists
			return s_pThis; // Return the pointer to the object
	}

	void Timer::start(SystemSignal init, ...)
	{
		// Get the time now for calculating the time for a frame
		s_frameStart = std::chrono::high_resolution_clock::now();
	}

	void Timer::stop(SystemSignal close, ...)
	{
		// No longer an instance of the object
		s_bInstanceFlag = false;

		s_pThis = nullptr; // Set the pointer to itself to null
	}

	float Timer::newFrame()
	{
		s_frameEnd = std::chrono::high_resolution_clock::now(); // Get the time for the end of the frame
		s_frameDuration = s_frameEnd - s_frameStart; // Calculate the time taken for the frame
		s_frameStart = std::chrono::high_resolution_clock::now(); // Get the time for the start of the next frame

		return s_frameDuration.count();
	}

	float Timer::newPhysics()
	{
		s_physicsEnd = std::chrono::high_resolution_clock::now(); // Get the time for the end of the physics update
		s_physicsDuration = s_physicsEnd - s_physicsStart; // Calculate the time taken for the physics update
		s_physicsStart = std::chrono::high_resolution_clock::now(); // Get the time for the start of the next physics update

		return s_physicsDuration.count();
	}

	void Timer::startTimer(const std::string& tag)
	{
		// If the map doesn't contain the new tag
		if (s_sectionTimer.find(tag) == s_sectionTimer.end())
		{
			s_sectionTimer.insert(std::make_pair(tag, std::make_pair(std::chrono::high_resolution_clock::now(), std::chrono::high_resolution_clock::now())));

#ifdef NG_DEBUG // Only in debug mode
			s_debugProcessTimer.insert(std::make_pair(tag, std::vector<float>())); // Insert a process-related vector container, within the process profiling map
#endif
		}
		else // If the map already contains the tag
		{
			s_sectionTimer[tag].first = std::chrono::high_resolution_clock::now();
			s_sectionTimer[tag].second = std::chrono::high_resolution_clock::now();
		}
	}

	float Timer::endTimer(const std::string& tag)
	{
		// If the map doesn't contain the tag
		if (s_sectionTimer.find(tag) == s_sectionTimer.end())
		{
			// Display error using the logger
			LOG_ERROR("Timer with tag: '{0}' not found, please ensure the same tag is used in start and end timer functions", tag);
			return 0.f; // Return nothing
		}
		else // If the map contains the tag
		{
			// Set the end time point for the timer
			s_sectionTimer[tag].second = std::chrono::high_resolution_clock::now();

			// Calculate the timer duration
			s_sectionTimeDuration = (s_sectionTimer[tag].second - s_sectionTimer[tag].first) * 1000;

#ifdef NG_DEBUG // Only in debug mode
			std::vector<float>::iterator iterator = s_debugProcessTimer[tag].begin(); // Create and set 'iterator' to the position of the first element in the vector container
			s_debugProcessTimer[tag].insert(iterator, s_sectionTimeDuration.count()); // Insert the duration into the vector container

			if (s_debugProcessTimer[tag].size() > 10)
			{
				// If more than 10 times in the vector then remove the back one
				s_debugProcessTimer[tag].pop_back();
			}
#endif
			return s_sectionTimeDuration.count(); // Return the time taken
		}
	}
}