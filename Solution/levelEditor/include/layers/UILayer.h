/** \file UILayer.h
*/

// Compile Header Once
#pragma once

// Include Display Layer Header
#include "layers/displayLayer.h"
#include "changeObjects.h"

#include <thread>

/**
\class UILayer
\brief Layer with all editor UI, controls the editor
*/
class UILayer : public Engine::ImGuiLayer
{
private:
	std::string m_sProjectName; //!< The name of the project

	std::shared_ptr<DisplayLayer> m_displayLayer; //!< Pointer to the display layer

	bool m_bLeaving = false; //!< If leaving the editor
	bool m_bRunning = false; //!< If running the game

	int m_iCurrentObjectSelected = 0; //!< The vector value of the current object selected
	bool m_bComponentsShowing = false; //!< If components of an object are showing
	bool m_b3DComponentsShowing = true;

	int m_iResetCamera;
	int m_iLastCameraSelected = 0; //!< The vector value of the camera component of the last camera object selected
	int m_i2DCameraVectorValue = 0;
	float m_displayWindowMultiplier = 1; //!< The multiplier for the display window size

	std::shared_ptr<Engine::FrameBuffer> m_pFrameBuffer; //!< The frame buffer for rendering the view of the game camera
	
	std::shared_ptr<Engine::Layerstack> m_pLayerstack; //!< A pointer to the layerstack

	bool m_bHoveringOnImage = false; //!< If the mouse is over the rendered image

	std::vector<std::pair<std::string, std::string>> m_levels; //!< Level names an their filepaths
	int m_layerSelected = 0; //!< The layer selected to change
	bool m_layerIsSelected = false; //!< If a layer is selected
	bool m_3DSelected = true; //!< If the layer selected is 3D
	char m_newLayerName[61]; //!< Array for changing layer names

	char m_newLevelName[61]; //!< Array for changing level names

	bool m_bTagIsSelected = false; //!< Is a tag selected
	int m_iTagSelected = 0; //!< Which tag is selected
	bool m_bMakingNewTag = false; //!< Is the user making a new tag
	char m_newTagName[61]; //!< Array for making a new tag name

	int m_iCurrentLevel = 0; //!< Which level is currently showing

	std::shared_ptr<Engine::DebugLayer> m_pDebugLayer; //!< A pointer to the debug layer

	int saveEditorObject(int i, std::vector<Object>* pObjects, std::vector<std::shared_ptr<Engine::GameObject>>& gameObjects, nlohmann::json& objectArray);

	void saveRunning(); //!< Function to save the current state of the editor to run the game
	void saveLayerRunning(std::vector<std::string>::iterator& layer, bool UI);

	void loadTextures(); //!< Function for loading textures from the texture folders
	void loadModels(); //!< Function for loading models from the model folders
public:
	//! Constructor
	/*!
	\param displayLayer A shared pointer to the display layer
	\param layerstack A pointer to the layerstack
	\param projectName The name of the project
	\param levels The names and JSON filepaths of the levels
	\param currentLevel Whichs level is loading
	\param resources A pointer to the resource manager
	\param debugLayer A pointer to the debugLayer
	\param name The name to be assigned to this layer
	*/
	UILayer(std::shared_ptr<DisplayLayer> displayLayer, std::shared_ptr<Engine::Layerstack> layerstack, const std::string& projectName, 
		std::vector<std::pair<std::string, std::string>>& levels, int currentLevel, std::shared_ptr<Engine::ResourceManager> resources, 
		std::shared_ptr<Engine::DebugLayer> debugLayer, const std::string& name = "UILayer") :
		m_displayLayer(displayLayer), m_pLayerstack(layerstack), m_sProjectName(projectName), m_levels(levels), m_iCurrentLevel(currentLevel), 
		m_pDebugLayer(debugLayer), ImGuiLayer(name)
	{
		ChangeObjects::setDisplayLayer(m_displayLayer);
		ChangeObjects::setProjectName(m_sProjectName);

		m_pResources = resources;
	}

	~UILayer(); //!< Destructor

	void saveEditor(); //!< Function to save the editor to be loaded again

	//! Function called when the layer is added
	void onAttach() override;

	//! Function called when the layer is removed
	void onDetach() override 
	{ 
		if (!m_bRunning)
			m_displayLayer->onDetach(); 
	}

	//! Function to update the layer every frame
	/*!
	\param timestep The time since the previous frame
	*/
	void onUpdate(float timestep) override;

	void drawObjects() override {}

	//! Function to draw the texture that the layer is rendered to
	void drawLayer() override {}

	//! Function to handle events
	/*!
	\param event The event
	*/
	void onGameEvent(Engine::Event& event);
	//! Called when a window focus event occurs
	/*!
	\param event The event
	\return If the event was successful
	*/
	bool onWindowFocus(Engine::WindowFocusEvent& event) { loadAssets(); return true; }

	//! Accessor function for which level is currently showing
	/*!
	\return The number of the level currently in
	*/
	inline int currentLevelNum() { return m_iCurrentLevel; }

	//! Accessor Function for m_bLeaving for EditorEngineApp
	/*!
	\return If the editor is being left or not
	*/
	inline bool isLeaving() { return m_bLeaving; }
	//! Accessor Function for m_bRunning for EditorEngineApp
	/*!
	\return If the game is being run
	*/
	inline bool isRunning() { return m_bRunning; }

	void loadAssets(); //!< Function to go through the projects folders and load the assets

	//! Function to delete an object
	/*!
	\param it A reference to the object
	*/
	void DeleteObject(Object& it);
	//! Function to delete a joint
	/*!
	\param it A reference to the joint
	*/
	void DeleteJoint(Joint& it);

	//! Function to get if the mouse is hovering over the game image or not
	inline bool hoveringOverGameImage() { return m_bHoveringOnImage; }
};