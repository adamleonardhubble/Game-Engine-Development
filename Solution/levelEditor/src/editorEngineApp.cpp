/** \file editorEngineApp.cpp
*/

#include "editorEngineApp.h"

#include "layers/displayLayer.h"


EditorEngineApp::EditorEngineApp()
{
	// Make a new window called Level Editor
	getWindow().close();
	Engine::WindowProperties properties("Level Editor", 800, 600, false, true);
	getWindow().init(properties);

	// Make a new start menu layer
	m_pStartLayer.reset(new StartLayer("Starting Menu"));
	
	m_pLayerStack->push(m_pStartLayer);

#ifdef NG_DEBUG
	m_pDebugLayer = std::make_shared<Engine::DebugLayer>(Engine::DebugLayer(true));
	m_pLayerStack->push(m_pDebugLayer);
#endif

	// It's started in Application but isn't needed
	m_pObjectUpdateThread->stop();
}

EditorEngineApp::~EditorEngineApp()
{
	if (m_pUILayer != nullptr)
		m_pUILayer->onDetach();
}

void EditorEngineApp::onEvent(Engine::Event& e)
{
	// Create event dispatcher
	Engine::EventDispatcher dispatcher(e);
	// If the Event type matches, call the corresponding function
	dispatcher.dispatch<Engine::WindowCloseEvent>(std::bind(&Application::onClose, this, std::placeholders::_1));
	dispatcher.dispatch<Engine::WindowResizeEvent>(std::bind(&Application::onResize, this, std::placeholders::_1));

	if (m_pDebugLayer != nullptr)
		TIMER_START("EventHandling"); // Start 'EventHandling' timer

	if (m_pUILayer != nullptr)
	{
		e.setOverGame(m_pUILayer->hoveringOverGameImage());

		m_pUILayer->onEvent(e);
		m_pUILayer->onGameEvent(e);
	}
	if (m_pDebugLayer != nullptr)
		TIMER_END("EventHandling"); // Stop 'EventHandling' timer
}

void EditorEngineApp::loadLevel(Engine::Levels::iterator& layers, const std::string& projectName)
{
	clearLayerStack();
	// Remove the resources to have only the stuff for the current situation loaded
	m_pResources->stop();

	// If in the start menu, loading a project
	if (m_runningState == RunningState::StartMenu)
	{
		// Make a display layer
		std::shared_ptr<DisplayLayer> m_pDisplayLayer = 
			std::make_shared<DisplayLayer>(DisplayLayer(m_setup.getLevels().begin()->second));
		// Make a new UI layer
		m_pUILayer.reset(new UILayer(m_pDisplayLayer, m_pLayerStack, projectName, m_setup.getLevels(), 0, m_pResources, m_pDebugLayer));
		m_pUILayer->onAttach();
	}
	else if (m_runningState == RunningState::StopRunning) // If stopping running the game
	{
		// Make a display layer
		std::shared_ptr<DisplayLayer> m_pDisplayLayer =
			std::make_shared<DisplayLayer>(DisplayLayer(std::next(m_setup.getLevels().begin(), m_pUILayer->currentLevelNum())->second));
		// Make a new UI layer
		m_pUILayer.reset(new UILayer(m_pDisplayLayer, m_pLayerStack, projectName, m_setup.getLevels(), m_pUILayer->currentLevelNum(), m_pResources, m_pDebugLayer));
		m_pUILayer->onAttach();
	}
	else if (m_runningState == RunningState::Running) // If the editor is running, starting to run the game
	{
		Engine::JSONLayer::setupFrameBuffer();
		Engine::JSONLoader::AsyncLoad(layers->second.first);
		// For each layer in the new level
		for (auto& layer : layers->second.first)
		{
			// Push the layer onto the stack
			m_pLayerStack->push(std::make_shared<Engine::JSONLayer>(Engine::JSONLayer(layer.first, m_pDynamicsSystem, layer.second)));
		}
		for (auto it = m_pLayerStack->begin(); it != m_pLayerStack->end(); it++)
		{
			(*it)->setLightingSceneData();
		}
		// Set which level key code is active
		m_iCurrentLevel = layers->second.second;
	}
	if (m_pDebugLayer != nullptr)
		m_pLayerStack->push(m_pDebugLayer);
}

void EditorEngineApp::run()
{
	while (m_bRunning)
	{
		if (m_pDebugLayer != nullptr)
			TIMER_START("ApplicationUpdate"); // Start 'ApplicationUpdate' timer

		m_fTimestep = TIMER_NEWFRAME; // Tell timer to start for a new frame
		Application::getScreenResolution() = glm::vec2(Application::getWindow().getWidth(), Application::getWindow().getHeight()); // Set the IMGui window resolution to the application windows width and height dimensions

#ifdef NG_DEBUG
		//LOG_INFO("FPS: {0} T: {1}", (int)(1 / m_fTimestep), m_fTimestep); // Show the FPS
#endif
		// If in the start menu
		if (m_runningState == RunningState::StartMenu)
		{
			m_pStartLayer->onUpdate(m_fTimestep); // Update the layer
			std::pair<bool, std::string> starting = m_pStartLayer->isStarting(); // Get if a project has been selected
			// If a project has been selected
			if (starting.first)
			{
				// Create setup object
				m_setup = EditorJSONSetup();
				m_setup.load(starting.second); // Load the level information
				m_loaded = starting.second;
				// Load the first level
				Engine::Application::getInstance().loadLevel(m_levels.begin(), m_setup.getProjectName());

				m_fTimestep = TIMER_NEWFRAME;

				m_runningState = RunningState::Editor; // Set the state
			}
		}
		else if (m_runningState == RunningState::Editor) // If in the editor
		{
			// Update the ui layer
			m_pUILayer->onUpdate(m_fTimestep);
			// Quit has been selected
			if (m_pUILayer->isLeaving())
			{
				// Remove the resources to have only the stuff for the current level loaded
				m_pResources->stop();
				clearLayerStack();
				m_pStartLayer.reset(new StartLayer("Starting Menu"));
				// Make the start menu again
				m_pLayerStack->push(m_pStartLayer);

				m_runningState = RunningState::StartMenu; // Set the state
			}
			else if (m_pUILayer->isRunning()) // If run has been selected
			{
				m_runningState = RunningState::Running; // Set the state

				m_pUILayer->saveEditor();

				// Start the object update thread, only used if using a separate threads
				m_pObjectUpdateThread = Engine::ObjectUpdateThread::getInstance();
				m_pObjectUpdateThread->start();

				m_levels = Engine::Levels();
				// Create setup object
				m_runSetup = Engine::JSONSetup(&m_levels);
				m_runSetup.load("assets/savedJSON/TempControl.json"); // Load the level information
				// Load the first level
				Engine::Application::getInstance().loadLevel(m_levels.begin());
				// If the level uses physics
				if (Engine::DynamicPhysicsSystem::systemExists())
				{
					m_bPhysicsRunning = true; // The physics is running
					// If updating physics on a separate thread, make the separate thread and make it run the updateWorld physics loop
					if (Engine::DynamicPhysicsSystem::separateThread())
						m_physicsThread = std::thread(&Engine::DynamicPhysicsSystem::updateWorld, m_pDynamicsSystem, &m_bPhysicsRunning);
				}

				TIMER_NEWFRAME;
			}
		}
		else if (m_runningState == RunningState::Running) // If the editor is running a game
		{
			// Update the physics (if not on a separate thread, update all physics, if on  a separate thread, check collisions and raycasts)
			m_pDynamicsSystem->update(m_fTimestep);

			m_pUILayer->onUpdate(m_fTimestep);

			if (!m_pUILayer->isRunning()) // If stop has been selected
			{
				m_pObjectUpdateThread->stop();

				// If there is a dynamic physics system
				if (Engine::DynamicPhysicsSystem::systemExists())
				{
					m_bPhysicsRunning = false; // For the separate physics thread loop to end

					// If physics was on a separate thread, synchronise the threads
					if (Engine::DynamicPhysicsSystem::separateThread())
						m_physicsThread.join();

					m_pDynamicsSystem->stop(); // Stop it
				}

				m_runningState = RunningState::StopRunning;
				// Reload the project to reset everything
				m_setup = EditorJSONSetup();
				m_setup.load(m_loaded); // Load the level information
				// Load the current level
				Engine::Application::getInstance().loadLevel(m_levels.begin(), m_setup.getProjectName());

				m_runningState = RunningState::Editor; // Set the state
			}
		}

		// Update the window
		Application::getWindow().onUpdate(m_fTimestep, false);

		if (m_pDebugLayer != nullptr)
			TIMER_END("ApplicationUpdate"); // Stop 'ApplicationUpdate' timer
	}

	// If the physics system exists (closing the window while running a game in the editor)
	if (Engine::DynamicPhysicsSystem::systemExists())
	{
		m_bPhysicsRunning = false; // For the separate physics thread loop to end
		// If updating physics on a separate thread, make the separate thread and make it run the updateWorld physics loop
		if (Engine::DynamicPhysicsSystem::separateThread())
			m_physicsThread.join();
	}

	// So that there are no shared pointers to display layer, to destroy it now, preventing errors
	ChangeObjects::setDisplayLayer(nullptr);
}

Engine::Application* Engine::startApplication()
{
	return new EditorEngineApp();
}

