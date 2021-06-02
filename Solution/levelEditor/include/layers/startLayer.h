/** \file startLayer.h
*/
#pragma once

#include "layers/load/loadSetup.h"

/**
\class StartLayer
*/
class StartLayer : public Engine::ImGuiLayer
{
private:
	char m_newName[41] = { "" }; //!< Array for a new name of a project if making one

	bool m_bNewFolderError = false; //!< If a name couldn't be used, to show an error message
	bool m_bStarting = false; //!< If the editor is starting, for EditorEngineApp to know to do something

	std::string m_loadFilepath; //!< The filepath of the layer being loaded

	//! Function to get the name from a filepath
	/*!
	\param str The filepath
	\return The name
	*/
	std::string parseFilePath(const std::string& str);
public:
	//! Constructor
	/*!
	\param name The name of the layer
	*/
	StartLayer(const std::string& name = "UILayer") : ImGuiLayer(name) {}

	//! Function called when the layer is added
	void onAttach() override;

	//! Function called when the layer is removed
	void onDetach() override { m_bNewFolderError = false; m_bStarting = false; }

	//! Function to update the layer every frame
	/*!
	\param timestep The amount of time passed since the previous frame
	*/
	void onUpdate(float timestep) override;

	void drawObjects() override {}
	//! Function to draw the texture that the layer is rendered to
	void drawLayer() override {}

	//! Function for EditorEngineApp to see if the editor needs too be started
	/*!
	\return A pair containing a bool for it is starting and the filepath to load if it is
	*/
	inline std::pair<bool, std::string> isStarting() { return std::make_pair(m_bStarting, m_loadFilepath); }
};