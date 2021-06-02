/** \file loadSetup.h
*/
#pragma once

#include <fstream>

#include "json.hpp"
#include "systems/layerstack.h"

#include "systems/log.h"


class EditorJSONSetup
{
private:
	//Engine::Levels* m_plevels;
	std::string m_sProjectName;

	std::vector<std::pair<std::string, std::string>> m_levels;
public:
	//! Default constructor
	EditorJSONSetup() {}
	//! Constructor
	/*!
	\param levels A pointer to level information
	*/
	//EditorJSONSetup(Engine::Levels* levels) : m_plevels(levels) {}

	std::vector<std::pair<std::string, std::string>>& getLevels() { return m_levels; }

	//! Function to set level loading information
	/*!
	\param filepath The file location of the setup json file
	*/
	void load(const std::string& filepath)
	{
		//m_plevels->clear();

		std::fstream handle(filepath, std::ios::in);
		if (!handle.is_open()) {
			LOG_CRITICAL("Could not open file: {0}", filepath);
		}
		nlohmann::json layerJSON;
		handle >> layerJSON;

		if (layerJSON.count("Load") > 0)
		{
			m_sProjectName = layerJSON["Load"]["projectName"].get<std::string>();
			// If there are level objects in the setup file
			if (layerJSON["Load"].count("levels") > 0)
			{
				// For each level
				for (auto& level : layerJSON["Load"]["levels"])
				{
					std::string filepath = level["layerFilepath"].get<std::string>();

					std::string name = level["name"].get<std::string>();
					m_levels.push_back(std::make_pair(name, filepath));
				}
			}
		}
	}

	std::string getProjectName() { return m_sProjectName; }
};