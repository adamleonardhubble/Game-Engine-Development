/** \file JSONSetup.h
*/
#pragma once

#include <fstream>

#include "json.hpp"
#include "JSONlayer.h"
#include "systems/layerstack.h"

#include "systems/log.h"

namespace Engine
{
	/**
	\class JSONSetup
	\brief Class which sets up level loading order
	*/
	class JSONSetup
	{
	private:
		Levels* m_plevels; //!< Pointer to level information
	public:
		//! Default constructor
		JSONSetup() {}
		//! Constructor
		/*!
		\param levels A pointer to level information
		*/
		JSONSetup(Levels* levels) : m_plevels(levels) {}
		//! Destructor
		~JSONSetup()
		{
			// Make the pointer null
			m_plevels = nullptr;
		}

		//! Function to set level loading information
		/*!
		\param filepath The file location of the setup json file
		*/
		void load(const std::string& filepath)
		{
			std::fstream handle(filepath, std::ios::in);
			if (!handle.is_open()) {
				LOG_CRITICAL("Could not open file: {0}", filepath);
			}
			nlohmann::json layerJSON;
			handle >> layerJSON;

			if (layerJSON.count("Load") > 0)
			{
				// If there are level objects in the setup file
				if (layerJSON["Load"].count("levels") > 0)
				{
					// For each level
					for (auto& level : layerJSON["Load"]["levels"])
					{
						int loadKey; // The key code to load the level
						if (level.count("load") > 0)
						{
							// Get which button loads a level
							if (level["load"]["method"]["button"].get<std::string>().compare("left") == 0)
								loadKey = ENGINE_KEY_LEFT;
							if (level["load"]["method"]["button"].get<std::string>().compare("right") == 0)
								loadKey = ENGINE_KEY_RIGHT;
						}

						std::map<std::string, std::string> layers; // For layer information (filepath and name)
						int i = 0;
						// For each layer in the level
						for (auto& layer : level["layers"])
						{
							// Get the filepath and the name
							std::string filepath = layer["filepath"].get<std::string>();
							std::string name = layer["name"].get<std::string>();

							// Add them to the map
							layers.insert(std::make_pair(std::to_string(i) + filepath, name));
							i++;
						}
						std::string levelName = level["name"].get<std::string>();

						// Add layer info and the load key to the level map
						m_plevels->insert(std::make_pair(levelName, std::make_pair(layers, loadKey)));
					}				
				}
			}
		}
	};
}