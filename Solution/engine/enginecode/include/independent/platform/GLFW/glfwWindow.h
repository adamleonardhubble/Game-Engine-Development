/** \file glfwWindow.h
*/
#pragma once

#include "windows/window.h"
#include "systems/log.h"
#include "platform/GLFW/GLFWGraphicsContext.h"

namespace Engine
{
	/**
	\class GLFWWindowImpl
	\brief Class inheriting from Window to create a window using glfw
	*/
	class GLFWWindowImpl : public Window
	{
	private:
		GLFWwindow* m_pNativeWindow; //!< Pointer to GLFWwindow (the window being created)
		GLFWmonitor* m_pMonitor; //!< Pointer to GLFWmonitor (used to get the screen size)

		int m_iViewportWidth; //!< The new width of the viewport
		int m_iViewportHeight; //!< The new height of the viewport

		bool m_isViewportActive; //!< Determine whether a viewport is active

		int m_iWindowWidth;	//!< Integer representing the current width of the window
		int m_iWindowHeight; //!< Integer representing the current height of the window

		//! Initialize the window, called from the constructor when making a new window
		/*!
		\param properties The properties to give the new window
		*/
		void init(const WindowProperties& properties) override;
		WindowProperties m_properties; //!< Stores the properties of the window
		std::function<void(Event&)> m_callback; //!< Callback for events, bound to Application::onEvent
	public:
		//! Constructor
		/*!
		\param properties The properties to give the new window
		*/
		GLFWWindowImpl(const WindowProperties& properties);
		//! Destructor
		~GLFWWindowImpl();

		//! Called to close the window
		void close() override; 

		//! Called every frame to update the window
		/*!
		\param timestep The time passed after previous onUpdate function call
		\param viewport If the window will use pillarbox/letterbox effect
		*/
		void onUpdate(float timestep, bool viewport = true) override;
		
		//! Called when the window size is changed
		/*!
		\param width The new width of the window
		\param height The new height of the window
		*/
		void onResize(unsigned int width, unsigned int height) override;
		
		//! Called to enter or exit fullscreen mode
		/*!
		\param fullscreen Whether the window is being made fullscreen or not
		*/
		void setFullscreen(bool fullscreen) override;
		
		//! Set if vSync is on
		/*!
		\param VSync True if turning vSync on
		*/
		void setVSync(bool VSync) override;
		
		//! Set the event callback for the window
		/*!
		\param callback Callback for events
		*/
		void setEventCallback(const std::function<void(Event&)>& callback) override;
		
		//! Function to get the width of the window
		/*!
		\return The width of the window in pixels
		*/
		inline unsigned int getWidth() const override { return m_properties.m_width; }
		
		//! Function to get the height of the window
		/*!
		\return The height of the window in pixels
		*/
		inline unsigned int getHeight() const override { return m_properties.m_height; }
		
		//! Function to get a pointer to the window
		/*!
		\return The stored pointer to the native window
		*/
		inline void* getNativeWindow() const override { return m_pNativeWindow; }
		
		//! Function to check if the window is fullscreen
		/*!
		\return A bool for if the window is fullscreen
		*/
		inline bool isFullScreenMode() const override { return m_properties.m_isFullScreen; }
		
		//! Function to check if vSync is on
		/*!
		\return A bool for if vSync is on
		*/
		inline bool isVSync() const override { return m_properties.m_isVSync; }

		//! Function to get the width of the viewport
		/*!
		\return The width of the viewport
		*/
		inline int getViewportWidth() const override { return m_iViewportWidth; }
		
		//! Function to get the height of the viewport
		/*!
		\return The height of the viewport
		*/
		inline int getViewportHeight() const override { return m_iViewportHeight; }

		//! Function to get the viewport activeness
		/*!
		\return The viewport activeness
		*/
		inline bool isViewportActive() const override { return m_isViewportActive; }

		//! Function to get the aspect ratio of the window instance
		/*!
		\return The aspect ratio of the window instance
		*/
		inline float getAspectRatio() const override { return m_properties.m_fAspect; }
	};
}