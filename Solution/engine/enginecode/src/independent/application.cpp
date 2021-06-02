/** \file application.cpp
*/

#include "engine_pch.h"

#include "core/application.h"
#include "core/core.h"
#include "core/codes.h"

#include "load/JSONLayer.h"
#include "load/JSONLoader.h"

#ifdef NG_PLATFORM_WINDOWS
#include "platform/GLFW/GLFWWindowsSystem.h"
#endif // NG_PLATFORM_WINDOWS

namespace Engine
{
	Application* Application::s_instance = nullptr;	// Intialise static variable 's_instance', set to 'NULL'

	glm::vec2 Application::m_screenResolution; // Intialise static variable 'm_screenResolution'

	Application::Application()
	{
		if (s_instance == nullptr)
		{
			s_instance = this;
		}

		m_bRunning = true; // Application is running
		m_fTotalTimeElapsed = 0; // Set the time elapsed to nothing

		// Create and get instances of the system objects
		m_pLogger = Log::getInstance();
		m_pLogger->start();
		m_pTimer = Timer::getInstance();
		m_pTimer->start();


		m_pIMGui.reset(new IMGuiSystem);
		m_pIMGui->start();

#ifdef NG_PLATFORM_WINDOWS // If the engine is running on a windows computer
		m_pWindows.reset(new GLFWWindowsSystem); // Create an instance of the GLFW windows system
#endif // NG_PLATFORM_WINDOWS
		m_pWindows->start(); // Start the windows system
		LOG_INFO("Windows system initialised");

		// Create a window
		m_pWindow.reset(Window::create());

		// Set the windows event callback to call the onEvent function in Application
		m_pWindow->setEventCallback(std::bind(&Application::onEvent, this, std::placeholders::_1));

		// Make a new resource manager
		m_pResources.reset(new ResourceManager);

		// Make a new layer stack
		m_pLayerStack.reset(new Layerstack);
		m_pLayerStack->start();

		m_iHardwareConcurrency = std::thread::hardware_concurrency();

		m_pDynamicsSystem.reset(new DynamicPhysicsSystem(m_iHardwareConcurrency));

		m_screenResolution = glm::vec2(m_pWindow->getWidth(), m_pWindow->getHeight());

		m_pObjectUpdateThread = ObjectUpdateThread::getInstance();
		m_pObjectUpdateThread->start(SystemSignal::None, m_iHardwareConcurrency);

		TIMER_NEWFRAME; // Tell the timer to start for a new frame
	}

	Application::~Application()
	{
		// Run the stop functions of the systems

		if (m_pObjectUpdateThread->systemExists())
			m_pObjectUpdateThread->stop();

		// If there is a dynamic physics system
		if (DynamicPhysicsSystem::systemExists())
			m_pDynamicsSystem->stop();

		clearLayerStack();

		m_pLayerStack->stop();
		m_pResources->stop();
		m_pWindow->close(); // Close the window
		m_pWindows->stop();
		m_pIMGui->stop();
		m_pTimer->stop();
		m_pLogger->stop();
	}

	void Application::onEvent(Event& e)
	{
		// Create event dispatcher
		EventDispatcher dispatcher(e);
		// If the Event type matches, call the corresponding function
		
		dispatcher.dispatch<WindowCloseEvent>(std::bind(&Application::onClose, this, std::placeholders::_1));
		dispatcher.dispatch<WindowResizeEvent>(std::bind(&Application::onResize, this, std::placeholders::_1));
		dispatcher.dispatch<KeyPressedEvent>(std::bind(&Application::onKeyPressed, this, std::placeholders::_1));

#ifdef NG_DEBUG	// Only in debug mode
		TIMER_START("EventHandling"); // Start 'EventHandling' timer
#endif
		// Reverse iterate through the layer stack, once an event has been handled, leave the loop
		for (auto it = m_pLayerStack->rbegin(); it != m_pLayerStack->rend(); ++it)
		{
			(*it)->onEvent(e);
			if (e.handled()) break;
		}

#ifdef NG_DEBUG	// Only in debug mode
		TIMER_END("EventHandling"); // Stop 'EventHandling' timer
#endif
	}

	bool Application::onClose(WindowCloseEvent& e)
	{
		// Log what's happening
		LOG_TRACE("Window closing");
		m_bRunning = false; // No longer running
		return true;
	}

	bool Application::onResize(WindowResizeEvent& e)
	{
		// Log what's happening
		LOG_TRACE("Window resize event. Width {0}, Height {1}", e.getWidth(), e.getHeight());
		m_pWindow->onResize(e.getWidth(), e.getHeight()); // Set the windows new size in its properties
		return true;
	}

	bool Application::onKeyPressed(KeyPressedEvent& e)
	{
		// If the space key is pressed
		if (e.getKeyCode() == ENGINE_KEY_SPACE)
		{
			// If the window is not in fullscreen mode
			if (!m_pWindow->isFullScreenMode())
			{
				LOG_TRACE("Entering fullscreen"); // Log what's happening
				m_pWindow->setFullscreen(true); // Set the window to fullscreen
			}
			else // If the window is in fullscreen mode
			{
				LOG_TRACE("Exiting fullscreen"); // Log what's happening
				m_pWindow->setFullscreen(false); // Set the window to not fullscreen
			}
			return true; // Leave the function
		}
		// If escape key is pressed
		if (e.getKeyCode() == ENGINE_KEY_ESCAPE)
		{
			LOG_TRACE("Window closing"); // Log what's happening
			m_bRunning = false; // Stop the application from running
			return true; // Leave the function
		}
		// For each level
		for (Levels::iterator& level = m_levels.begin(); level != m_levels.end(); level++)
		{
			// If the key code matches the one to load the current level
			if (e.getKeyCode() == m_iCurrentLevel)
			{
				// Do nothing and return
				return true;
			}
			// If the key code matches the one to load another level
			if (e.getKeyCode() == level->second.second)
			{
				// Load the level
				loadLevel(level);
				return true; // Leave the function
			}
		}
		return false;
	}

	void Application::loadLevel(Levels::iterator& layers, const std::string& projectName)
	{
		// Remove the resources to have only the stuff for the current level loaded
		m_pResources->stop();
		// If there is a dynamic physics system
		if (DynamicPhysicsSystem::systemExists())
			m_pDynamicsSystem->stop(); // Stop it
		clearLayerStack();
		JSONLayer::setupFrameBuffer();
		JSONLoader::AsyncLoad(layers->second.first);
		// For each layer in the new level
		for (auto& layer : layers->second.first)
		{
			// Push the layer onto the stack
			m_pLayerStack->push(std::make_shared<JSONLayer>(JSONLayer(layer.first, m_pDynamicsSystem, layer.second)));
		}
		for (auto it = m_pLayerStack->begin(); it != m_pLayerStack->end(); it++)
		{
			(*it)->setLightingSceneData();
		}
		// Set which level key code is active
		m_iCurrentLevel = layers->second.second;

#ifdef NG_DEBUG
		m_pDebugLayer = std::make_shared<DebugLayer>(DebugLayer(false));
		m_pLayerStack->push(m_pDebugLayer);
#endif

		TIMER_NEWFRAME;
	}

	void Application::loadLevel(const std::string& levelName)
	{
		// Find the right level info
		Levels::iterator it = m_levels.find(levelName);

		// Remove the resources to have only the stuff for the current level loaded
		m_pResources->stop();
		// If there is a dynamic physics system
		if (DynamicPhysicsSystem::systemExists())
			m_pDynamicsSystem->stop(); // Stop it
		clearLayerStack();
		JSONLayer::setupFrameBuffer();
		JSONLoader::AsyncLoad(it->second.first);
		// For each layer in the new level
		for (auto& layer : it->second.first)
		{
			// Push the layer onto the stack
			m_pLayerStack->push(std::make_shared<JSONLayer>(JSONLayer(layer.first, m_pDynamicsSystem, layer.second)));
		}
		for (auto it = m_pLayerStack->begin(); it != m_pLayerStack->end(); it++)
		{
			(*it)->setLightingSceneData();
		}

#ifdef NG_DEBUG
		m_pDebugLayer = std::make_shared<DebugLayer>(DebugLayer(&m_bRunning, false));
		m_pLayerStack->push(m_pDebugLayer);
#endif

		TIMER_NEWFRAME;
	}

	void Application::clearLayerStack()
	{
		// While the layer stack has layers in it
		while (m_pLayerStack->begin() != m_pLayerStack->end())
		{
			// Remove a layer
			m_pLayerStack->pop();
		}

		// Clear all static pointers in the layers to prevent errors
		JSONLayer::clearStatic();
	}

	void Application::run()
	{
		// If there is a physics system, make a physics update thread
		if (DynamicPhysicsSystem::systemExists() && DynamicPhysicsSystem::separateThread())
			m_physicsThread = std::thread(&DynamicPhysicsSystem::updateWorld, m_pDynamicsSystem, &m_bRunning);

		while (m_bRunning)
		{
#ifdef NG_DEBUG	// Only in debug mode
			TIMER_START("ApplicationUpdate"); // Start 'ApplicationUpdate' timer
#endif

			m_fTimestep = TIMER_NEWFRAME; // Tell timer to start for a new frame

			m_screenResolution = glm::vec2(m_pWindow->getWidth(), m_pWindow->getHeight()); // Set the IMGui window resolution to the application windows width and height dimensions

#ifdef NG_DEBUG
			//LOG_TRACE("FPS: {0} T: {1}", (int)(1 / m_fTimestep), m_fTimestep); // Show the FPS
#endif
			// Update the physics (if not on a separate thread, update all physics, if on  a separate thread, check collisions and raycasts)
			m_pDynamicsSystem->update(m_fTimestep);

			// Update all layers in the layer stack
			for (auto it = m_pLayerStack->begin(); it != m_pLayerStack->end(); it++)
				(*it)->onUpdate(m_fTimestep);
			for (auto it = m_pLayerStack->begin(); it != m_pLayerStack->end(); it++)
				(*it)->drawObjects();
			for (auto it = m_pLayerStack->begin(); it != m_pLayerStack->end(); it++)
				(*it)->drawLayer();
			// Update the window
			m_pWindow->onUpdate(m_fTimestep);

#ifdef NG_DEBUG	// Only in debug mode
			TIMER_END("ApplicationUpdate"); // Stop 'ApplicationUpdate' timer
#endif
		}

		if (DynamicPhysicsSystem::systemExists() && DynamicPhysicsSystem::separateThread())
			m_physicsThread.join();
	}
}
