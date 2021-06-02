/** \file imGuiLayer.cpp
*/
#include "engine_pch.h"

#include "platform/openGL/imGui/imGuiLayer.h"

#include "core/codes.h"
#include "core/application.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	void ImGuiLayer::BeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::EndFrame()
	{
		ImGuiIO& io = ImGui::GetIO();
		glm::vec2 screenResolution = Application::getScreenResolution();
		io.DisplaySize = ImVec2((float)screenResolution.x, (float)screenResolution.y);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::onEvent(Event& event)
	{
		// Create event dispatcher
		Engine::EventDispatcher dispatcher(event);
		// If the Event type matches, call the corresponding function
		dispatcher.dispatch<MouseButtonPressedEvent>(std::bind(&ImGuiLayer::onMouseButtonPressedEvent, this, std::placeholders::_1));
		dispatcher.dispatch<MouseButtonReleasedEvent>(std::bind(&ImGuiLayer::onMouseButtonReleasedEvent, this, std::placeholders::_1));
		dispatcher.dispatch<MouseMovedEvent>(std::bind(&ImGuiLayer::onMouseMovedEvent, this, std::placeholders::_1));
		dispatcher.dispatch<MouseScrolledEvent>(std::bind(&ImGuiLayer::onMouseScrolledEvent, this, std::placeholders::_1));
		dispatcher.dispatch<KeyPressedEvent>(std::bind(&ImGuiLayer::onKeyPressedEvent, this, std::placeholders::_1));
		dispatcher.dispatch<KeyReleasedEvent>(std::bind(&ImGuiLayer::onKeyReleasedEvent, this, std::placeholders::_1));
		dispatcher.dispatch<KeyTypedEvent>(std::bind(&ImGuiLayer::onKeyTypedEvent, this, std::placeholders::_1));
		dispatcher.dispatch<WindowResizeEvent>(std::bind(&ImGuiLayer::onWindowResizeEvent, this, std::placeholders::_1));
	}

	bool ImGuiLayer::onMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getMouseButton()] = true;

		if (io.WantCaptureMouse && !e.getOverGame())
			return true;

		return false;
	}

	bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getMouseButton()] = false;

		if (io.WantCaptureMouse && !e.getOverGame())
			return true;

		return false;
	}

	bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.getX(), e.getY());

		return false;
	}

	bool ImGuiLayer::onMouseScrolledEvent(MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += e.getXOffset();
		io.MouseWheel += e.getYOffset();

		if (io.WantCaptureMouse)
			return true;

		return false;
	}

	bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = true;

		io.KeyCtrl = io.KeysDown[ENGINE_KEY_LEFT_CONTROL || ENGINE_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[ENGINE_KEY_LEFT_SHIFT || ENGINE_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[ENGINE_KEY_LEFT_ALT || ENGINE_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[ENGINE_KEY_LEFT_SUPER || ENGINE_KEY_RIGHT_SUPER];

		return false;
	}

	bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = false;

		return false;
	}

	bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int keycode = e.getKeyCode();
		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);

		return false;
	}

	bool ImGuiLayer::onWindowResizeEvent(WindowResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.getWidth(), e.getHeight());
		io.DisplayFramebufferScale = ImVec2(1.f, 1.f);

		return false;
	}
}

