/** \file IMGuiSystem.h
*/
#pragma once

#include "system.h"

#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>

namespace Engine
{
	/**
	\class IMGuiSystem
	\brief System to start and stop ImGui
	*/
	class IMGuiSystem : public System
	{
	public:
		void start(SystemSignal init = SystemSignal::None, ...) override; //!< A function to start the layerstack
		void stop(SystemSignal close = SystemSignal::None, ...) override; //!< A function to stop the layerstack
	};
}