/** \file IMGuiSystem.cpp
*/
#include "engine_pch.h"

#include "systems/IMGuiSystem.h"

#include "core/codes.h"

namespace Engine
{
	void IMGuiSystem::start(SystemSignal init, ...)
	{
        // Start ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); 
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::StyleColorsDark(); // Dark style

        // Setup key map
        io.KeyMap[ImGuiKey_Tab] = ENGINE_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = ENGINE_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = ENGINE_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = ENGINE_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = ENGINE_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = ENGINE_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = ENGINE_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = ENGINE_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = ENGINE_KEY_END;
        io.KeyMap[ImGuiKey_Insert] = ENGINE_KEY_INSERT;
        io.KeyMap[ImGuiKey_Delete] = ENGINE_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = ENGINE_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = ENGINE_KEY_SPACE;
        io.KeyMap[ImGuiKey_Enter] = ENGINE_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = ENGINE_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_A] = ENGINE_KEY_A;
        io.KeyMap[ImGuiKey_C] = ENGINE_KEY_C;
        io.KeyMap[ImGuiKey_V] = ENGINE_KEY_V;
        io.KeyMap[ImGuiKey_X] = ENGINE_KEY_X;
        io.KeyMap[ImGuiKey_Y] = ENGINE_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = ENGINE_KEY_Z;
	}

	void IMGuiSystem::stop(SystemSignal close, ...)
	{
        // Stop ImGui
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}
}