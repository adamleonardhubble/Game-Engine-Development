/** \file editorEngineApp.h
*/
#pragma once

#include <Engine.h>

// entry point
#include "include/independent/core/entrypoint.h"

#include "layers/load/loadSetup.h"
#include "layers/startLayer.h"
#include "layers/UILayer.h"

/**
\class EditorEngineApp
\brief Inheriting from the engine Application class to run the editor
*/
class EditorEngineApp : public Engine::Application
{
private:
	//! The states the editor can be in
	enum RunningState { StartMenu, Editor, Running, StopRunning };
	//! The state the editor is in
	RunningState m_runningState = RunningState::StartMenu;

	EditorJSONSetup m_setup; //!< The level setup object
	Engine::JSONSetup m_runSetup; //!< The running setup object

	float m_fTimestep = 0.f; //!< The timestep

	std::shared_ptr<StartLayer> m_pStartLayer; //!< The start menu layer
	std::shared_ptr<UILayer> m_pUILayer; //!< The UI layer

	std::string m_loaded;

	bool m_bPhysicsRunning = false; //!< If the physics is running or not, for the separate thread loop
public:
	//! Constructor
	EditorEngineApp();
	//! Destructor
	~EditorEngineApp() override;

	//! When an event happens
	/*!
	\param e A reference to the event
	*/
	void onEvent(Engine::Event& e) override;
	//! Function to load another level
	/*!
	\param layers A map of filepaths and names for the layers in the level
	\param projectName The name of the project/game
	*/
	void loadLevel(Engine::Levels::iterator& layers, const std::string& projectName) override;
	void run() override; //!< Main loop
};
