/** \file windows.h
*/
#pragma once

#include "system.h"

namespace Engine
{
	/**
	\class WindowsSystem
	\brief API agnostic windows system
	*/
	class WindowsSystem : public System
	{
	public:
		virtual void start(SystemSignal init = SystemSignal::None, ...) override = 0; //!< Inherited from System, to be implemented somewhere else
		virtual void stop(SystemSignal close = SystemSignal::None, ...) override = 0; //!< Inherited from System, to be implemented somewhere else
	};
}