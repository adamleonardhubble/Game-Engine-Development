/** \file log.cpp
*/

#include "engine_pch.h"
#include "systems/log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Engine
{
	bool Log::s_bInstanceFlag = false; // Initialise the instance flag as false
	std::shared_ptr<Log> Log::s_pThis = nullptr; // Initialise the pointer to this as null

	std::shared_ptr<spdlog::logger> Log::s_pLogger = nullptr; // Initialise the pointer to the spdlog logger as null

	std::shared_ptr<Log> Log::getInstance()
	{
		// If there is no instance of the object
		if (!s_bInstanceFlag)
		{
			// Create a new object
			s_pThis = std::shared_ptr<Log>(new Log());
			s_bInstanceFlag = true; // There is an instance of the object

			return s_pThis; // Return the pointer to the object
		}
		else // If the object already exists
			return s_pThis; // Return the pointer to the object
	}

	void Log::start(SystemSignal init, ...)
	{
		// Set up the logger
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_pLogger = spdlog::stdout_color_mt("Log");
		s_pLogger->set_level(spdlog::level::trace);
	}

	void Log::stop(SystemSignal close, ...)
	{
		s_pLogger = nullptr; // Set the pointer to the spdlog logger to null
		s_bInstanceFlag = false; // No longer an instance to the object
		s_pThis = nullptr; // Set pointer to itself to null
	}
}