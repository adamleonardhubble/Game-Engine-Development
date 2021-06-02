/** \file imGuiLayer.h
*/
#pragma once

#include "core/layer.h"

#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>

#include "events/mouseEvents.h"
#include "events/keyEvents.h"
#include "events/windowEvents.h"

namespace Engine
{
	/**
	\class ImGuiLayer
	\brief Base class for all layers that use ImGui
	*/
	class ImGuiLayer : public Layer
	{
	private:
		//! Function called when the event happens
		/*
		\param e The event
		*/
		bool onMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		//! Function called when the event happens
		/*
		\param e The event
		*/
		bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		//! Function called when the event happens
		/*
		\param e The event
		*/
		bool onMouseMovedEvent(MouseMovedEvent& e);
		//! Function called when the event happens
		/*
		\param e The event
		*/
		bool onMouseScrolledEvent(MouseScrolledEvent& e);
		//! Function called when the event happens
		/*
		\param e The event
		*/
		bool onKeyPressedEvent(KeyPressedEvent& e);
		//! Function called when the event happens
		/*
		\param e The event
		*/
		bool onKeyReleasedEvent(KeyReleasedEvent& e);
		//! Function called when the event happens
		/*
		\param e The event
		*/
		bool onKeyTypedEvent(KeyTypedEvent& e);
		//! Function called when the event happens
		/*
		\param e The event
		*/
		bool onWindowResizeEvent(WindowResizeEvent& e);
	protected:
		//! Constructor
		/*!
		\param name The name of the layer
		*/
		ImGuiLayer(const std::string& name) : Layer(name) {}

		//! Function to start a new IMGui frame
		void BeginFrame();
		//! Function to render IMGui
		void EndFrame();
	public:
		//! Function to handle events
		/*!
		\param event The event
		*/
		void onEvent(Event& event) override;
	};
}
