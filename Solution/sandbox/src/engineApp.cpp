/** \file engineApp.cpp
*/

#include "engineApp.h"

engineApp::engineApp()
{
	// Create setup object
	m_setup = Engine::JSONSetup(&m_levels);
	m_setup.load("assets/Physics/TempControl.json"); // Load the level information
	// Load the first level
	loadLevel(m_levels.begin());
}

engineApp::~engineApp()
{

}

Engine::Application* Engine::startApplication()
{
	return new engineApp();
}

