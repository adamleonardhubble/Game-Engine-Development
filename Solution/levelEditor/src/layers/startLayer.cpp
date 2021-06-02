/** \file startLayer.cpp
*/
#include <Engine.h>

#include "layers/startLayer.h"

#include <filesystem>
#include "json.hpp"

void StartLayer::onAttach()
{
	// Set the clear colour
	Engine::RenderCommand* clear = Engine::RenderCommand::setClearColourCommand(0.5f, 0.5f, 0.5f, 1.f);
	clear->action();
	delete clear;

	std::string filepath = "assets/savedJSON/";
	// Iterate through savedJSON folder
	for (const auto& entry : std::filesystem::directory_iterator(filepath))
	{
		// If there is a file (with a '.' in the filepath)
		if (entry.path().string().find(".") != std::string::npos)
		{
			std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
			// If it is a json file
			if (fileType.compare(".json") == 0 || fileType.compare(".glsl") == 0)
			{
				// Remove it
				std::filesystem::remove(entry.path());
			}
		}
	}
}

void StartLayer::onUpdate(float timestep)
{
	// Clear the screen
	Engine::RenderCommand* clear = Engine::RenderCommand::clearColourBufferCommand();
	clear->action();
	delete clear;

	// Begin Frame
	BeginFrame();

	// Create Window
	ImGui::Begin("Team Funk Level Editor");

	// Define New Project Header
	if (ImGui::CollapsingHeader("Start a New Project"))
	{
		// Create Make New Project GUI
		ImGui::Text("Project Name:"); // Text Input Title
		ImGui::PushItemWidth(200.f); // Set Text Input Width
		ImGui::InputText("###NewProjectNameInputText", m_newName, IM_ARRAYSIZE(m_newName)); // Create Text Input Box
		ImGui::NewLine(); // Line Break

		// Crate 'Create Project' Button
		if (ImGui::Button("Create Project"))
		{
			std::string name = "assets/savedJSON/"; // Make filepath
			name += m_newName; // Add folder name to filepath
			// Create new folder
			if (!std::filesystem::create_directory(name.c_str()))
			{
				// If couldn't make folder, show error
				LOG_INFO("Coudn't make folder: {0}", name);
				m_bNewFolderError = true;
			}
			else // If could make folder
			{
				// Make filepaths for other folders needed
				std::string texFolder = name + "/textures/";
				std::string modelFolder = name + "/models/";
				std::string scriptFolder = name + "/LuaScript/";
				// Make new folders
				if (!std::filesystem::create_directory(texFolder.c_str()) || !std::filesystem::create_directory(modelFolder.c_str()) || !std::filesystem::create_directory(scriptFolder.c_str()))
				{
					// If couldn't make folders, show error
					LOG_INFO("Coudn't make folders: {0}, {1} or {2}", texFolder, modelFolder, scriptFolder);
					m_bNewFolderError = true;
				}
				else // If could make folders
				{
					// Copy txt file to each so that the folders are kept when commiting to github
					std::filesystem::copy("assets/to keep folder in git commit.txt", name + "/LuaScript/to keep folder in git commit.txt");
					std::filesystem::copy("assets/to keep folder in git commit.txt", name + "/models/to keep folder in git commit.txt");
					std::filesystem::copy("assets/to keep folder in git commit.txt", name + "/textures/to keep folder in git commit.txt");
					// Copy the 3D layer and control json files to the project
					std::filesystem::copy("assets/savedJSON/Default/3DEditorLayer.json", name + "/Level 1.json");
					m_loadFilepath = name + "/" + m_newName + " Control.json";
					std::filesystem::copy("assets/savedJSON/Default/DefaultControl.json", m_loadFilepath);

					std::fstream handle(m_loadFilepath, std::ios::in);
					if (!handle.is_open()) {
						LOG_CRITICAL("Could not open file: {0}", m_loadFilepath);
					}
					// Set needed project name and layer filepath in the control json
					nlohmann::json loadJSON;
					handle >> loadJSON;
					loadJSON["Load"]["projectName"] = m_newName;
					for (auto& levels : loadJSON["Load"]["levels"])
					{
						levels["layerFilepath"] = name + "/Level 1.json";
						break;
					}
					std::ofstream file(m_loadFilepath);
					file << loadJSON;


					m_bStarting = true; // Starting the editor
				}
			}
		}

		// Output Error Message
		if (m_bNewFolderError)
		{
			// Define Error Message
			ImGui::SameLine(); // Same Line as Text Input Box
			ImVec4 red(1.f, 0.f, 0.f, 1.f); // Define Red Colour
			ImGui::TextColored(red, "ERROR! Cannot Create A Folder With This Name"); // Output Error Message
		}
		
	}
	// Line Break
	ImGui::NewLine();

	// Define Load Saved Project Header
	if (ImGui::CollapsingHeader("Open a Saved Project"))
	{
		std::map<std::string, bool> names; // Map of project names
		
		// Iterate through the folder
		for (const auto& entry : std::filesystem::directory_iterator("assets/savedJSON"))
		{
			// Add each project name to the map
			std::string name = parseFilePath(entry.path().string());
			if (name.compare("Default") != 0)
				names.insert(std::make_pair(name, false));
		}

		// Create Load Saved Project GUI
		ImGui::Text("Saved Projects:"); // List Title
		ImGui::PushItemWidth(200.f); // Set List Box Width
		ImGui::ListBoxHeader("###SavedProjectsListBox"); // Start Creating List Box
		for (auto& it : names)
		{
			std::string name = it.first;
			// If a name is selected
			if (ImGui::Selectable(name.c_str(), it.second))
			{
				// Set the load filepath
				m_loadFilepath = "assets/savedJSON/" + name + "/" + name + " Control.json";
				// Starting the editor
				m_bStarting = true;
			}
		}
		ImGui::ListBoxFooter(); // Stop Creating List Box
		ImGui::NewLine(); // Line Break
		ImGui::NewLine(); // Line Break
		ImGui::NewLine(); // Line Break
	}

	// Line Break
	ImGui::NewLine();

	// Define Exit Program Button
	if (ImGui::Button("Exit Program"))
		Engine::Application::getInstance().stopRunning();

	// End Window
	ImGui::End();

	// End Frame
	EndFrame();
}

std::string StartLayer::parseFilePath(const std::string& str)
{
	int i = str.rfind('/', str.length()); // Find the position in the string of the last occurence of '/'
	int j = str.rfind('\\', str.length()); // Find the position in the string of the last occurence of '\'

	if (i != std::string::npos && i > j) // If '/' and '.' are found
	{
		// Return a string with the name of the folder
		return str.substr(i + 1, str.length());
	}
	else if (j != std::string::npos) // If '/' and '.' are found
	{
		// Return a string with the name of the folder
		return str.substr(j + 1, str.length());
	}
	else // If '/' and '.' were not found
	{
		// Log an error
		LOG_INFO("Problem finding name of file: {0}, using full filepath", str);
		return str; // Return the full filepath
	}
}