/** \file log.h
*/
#pragma once

#include "system.h"
#include "spdlog/spdlog.h"

namespace Engine
{
	/**
	\class Log
	\brief A system class to be used for logging information, errors etc. 
	
	This class inherits fron System and provides access to the 3rd party library 'spdlog'
	*/
	class Log : public System
	{
	private:
		static bool s_bInstanceFlag; //!< A boolean for if the system is running
		static std::shared_ptr<Log> s_pThis; //!< Pointer to Log object

		static std::shared_ptr<spdlog::logger> s_pLogger; //!< Pointer to the logger
	public:
		//! Create instance of object and return a pointer to it
		/*!
		\return A pointer to the instance of the object
		*/
		static std::shared_ptr<Log> getInstance(); 

		void start(SystemSignal init = SystemSignal::None, ...) override; //!< A function to start the logger
		void stop(SystemSignal close = SystemSignal::None, ...) override; //!< A function to stop the logger

		//! Used to access the logger
		/*!
		\return A pointer to the spdlog logger
		*/
		inline static std::shared_ptr<spdlog::logger> getLogger() { return s_pLogger; } 
	};
}

//! Macro function to log info
#define LOG_INFO(...)      Engine::Log::getLogger()->info(__VA_ARGS__) 

//! Macro function to log trace
#define LOG_TRACE(...)     Engine::Log::getLogger()->trace(__VA_ARGS__) 

//! Macro function to log a warning
#define LOG_WARN(...)      Engine::Log::getLogger()->warn(__VA_ARGS__)

//! Macro function to log an error
#define LOG_ERROR(...)     Engine::Log::getLogger()->error(__VA_ARGS__)

//! Macro function to log a critical error
#define LOG_CRITICAL(...)  Engine::Log::getLogger()->critical(__VA_ARGS__)

