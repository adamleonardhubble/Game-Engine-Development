/** \file application.h
*/

#pragma once // Compile once

#include "systems/log.h"
#include "systems/timer.h"
#include "systems/windows.h"
#include "systems/resourceManager.h"
#include "systems/layerstack.h"
#include "systems/dynamicPhysicsSystem.h"
#include "systems/IMGuiSystem.h"
#include "systems/objectUpdateThread.h"

#include "platform/openGL/imGui/debugLayer.h"

#include "windows/window.h"

#include <thread>

namespace Engine
{
	//! A map with layer information, level name and the key code of the key to press to load the level
	using Levels = std::map<std::string, std::pair<std::map<std::string, std::string>, int>>; 

	/**
	\class Application
	\brief Fundemental class of the engine. A singleton which runs the game loop infinitely.
	*/
	class Application
	{
	protected:
		Application(); //!< Constructor

		std::shared_ptr<Log> m_pLogger; //!< Pointer to the Log system object
		std::shared_ptr<Timer> m_pTimer; //!< Pointer to the Timer system object
		std::shared_ptr<WindowsSystem> m_pWindows; //!< Pointer to the Windows system
		std::shared_ptr<ResourceManager> m_pResources; //!< Pointer to the resource manager system
		std::shared_ptr<Layerstack> m_pLayerStack; //!< Pointer to the layerstack
		std::shared_ptr<DynamicPhysicsSystem> m_pDynamicsSystem; //!< Pointer to the physics system
		std::shared_ptr<IMGuiSystem> m_pIMGui; //!< Pointer to the IMGui system
		std::shared_ptr<ObjectUpdateThread> m_pObjectUpdateThread; //!< Pointer to the object update thread system

		Levels m_levels; //!< Info for level loading
		int m_iCurrentLevel = 0; //!< Key code to load current level

		int m_iHardwareConcurrency; //!< The number of threads

		bool m_bRunning; //!< If the application is running

		std::thread m_physicsThread; //!< The thread used for physics updates;

#ifdef NG_DEBUG
		std::shared_ptr<DebugLayer> m_pDebugLayer; //!< The debug layer
#endif

		//! Function to empty the layer stack
		void clearLayerStack();
	private:
		static Application* s_instance; //!< Singleton instance of the application
		
		static glm::vec2 m_screenResolution; //!< Vector2(floats), representing the resolution of the IMGui window
		
		float m_fTotalTimeElapsed; //!< The total time the program has been active in seconds
		float m_fTimestep; //!< Time in seconds for the last frame
		
		std::unique_ptr<Window> m_pWindow; //!< Unique pointer to the window
	public:
		virtual ~Application(); //!< Deconstructor

		// Create a Lua Script Component
		//LuaScriptComponent cLuaScriptComponent = LuaScriptComponent();

		//! Instance getter from singleton pattern
		/*!
		\return A reference to the application instance
		*/
		inline static Application& getInstance() { return *s_instance; } 
		//! When an event happens
		/*!
		\param e A reference to the event
		*/
		virtual void onEvent(Event& e); 
		//! Called if the event is a window close event
		/*!
		\param e A reference to the event
		\return A boolean representing if the event was successful or not
		*/
		bool onClose(WindowCloseEvent& e);
		//! Called if the event is a window resize event
		/*!
		\param e A reference to the event
		\return A boolean representing if the event was successful or not
		*/
		bool onResize(WindowResizeEvent& e); 

		//! Called if a keyboard key is pressed
		/*!
		\param e A reference to the event
		\return A boolean representing if the event was successful or not
		*/
		bool onKeyPressed(KeyPressedEvent& e); 

		virtual void run(); //!< Main loop

		//! Used to get the window
		/*!
		\return A reference to the current window
		*/
		inline Window& getWindow() { return *m_pWindow; }

		//! Function to load another level
		/*!
		\param layers A map of filepaths and names for the layers in the level
		\param projectName The name of the project/game
		*/
		virtual void loadLevel(Levels::iterator& layers, const std::string& projectName = "New Project");
		//! Function to load a level using it's name
		/*!
		\param levelName The name of the level to load
		*/
		void loadLevel(const std::string& levelName);

		//!	Return the application window resolution, used by IMGui window
		/*!
		\return A Vector2(floats), representing the resolution of the application window
		*/
		static glm::vec2& getScreenResolution() { return m_screenResolution; }

		//! Function to stop the game running in the level editor
		inline void stopRunning() { m_bRunning = false; }
	};

	// To be defined in users code
	Application* startApplication(); //!< Function definition which provides an entry hook

}