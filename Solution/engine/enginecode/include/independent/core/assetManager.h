/** \file assetManager.h
*/
#pragma once

#include "systems/log.h"

#include <map>

namespace Engine
{
	/** 
	\class AssetManager
	\brief Templated class to store assets in a map
	*/
	template <class G>
	class AssetManager
	{
	private:
		std::map<std::string, std::shared_ptr<G>> m_container; //!< Map of strings and pointers to the asset classes
	public:
		//! Function to check if the map contains the string passed in
		/*!
		\param key The string being checked
		\return A bool representing if the string is in the map or not
		*/
		bool contains(const std::string& key);
		//! Function to add an asset to the map
		/*!
		\param key The key to represent the asset in the map
		\param element A pointer to the element being added
		*/
		void add(const std::string& key, std::shared_ptr<G>& element);
		//! Function to change an asset in the map
		/*!
		\param key The key to represent the asset in the map
		\param element A pointer to the element being added
		*/
		void replace(const std::string& key, std::shared_ptr<G>& element);
		//! Function to get an asset
		/*!
		\param key The key of the asset being added
		\return A pointer to the asset
		*/
		inline std::shared_ptr<G> get(const std::string& key);
		//! Function to clear the map
		void clear();
	};

	template<class G>
	bool AssetManager<G>::contains(const std::string& key)
	{
		std::map<std::string, std::shared_ptr<G>>::iterator it; // Make an iterator for the map
		it = m_container.find(key); // Find the key in the map

		// If the key is not in the map
		if (it == m_container.end())
			return false; // Return false
		else // If the key is in the map
			return true; // Return true
	}

	template<class G>
	void AssetManager<G>::add(const std::string& key, std::shared_ptr<G>& element)
	{
		std::map<std::string, std::shared_ptr<G>>::iterator it; // Make an iterator for the map
		it = m_container.find(key); // Find the key in the map

		// If the key is in the map
		if (it != m_container.end())
			LOG_ERROR("Asset with key: '{0}' already exists, use a different key", key); // Log an error, cannot have more than one of the same key in the map
		else // If the key is not in the map
		{
			m_container.insert(std::make_pair(key, element)); // Add the asset pointer and the key to the map
		}
	}

	template<class G>
	void AssetManager<G>::replace(const std::string& key, std::shared_ptr<G>& element)
	{
		std::map<std::string, std::shared_ptr<G>>::iterator it; // Make an iterator for the map
		it = m_container.find(key); // Find the key in the map

		// If the key is in the map
		if (it == m_container.end())
			LOG_ERROR("Asset with key: '{0}' doesn't exist, can't be changed", key); // Log an error, cannot change an asset that isn't there
		else // If the key is not in the map
		{
			m_container.erase(it); // Delete the previous asset
			m_container.insert(std::make_pair(key, element)); // Add the new asset pointer and the key to the map
		}
	}

	template<class G>
	std::shared_ptr<G> AssetManager<G>::get(const std::string& key)
	{
		std::map<std::string, std::shared_ptr<G>>::iterator it; // Make an iterator for the map
		it = m_container.find(key); // Find the key in the map
		
		// If the key is not in the map
		if (it == m_container.end())
		{
			// Log an error, cannot find key in the map
			LOG_ERROR("Asset with key: '{0}' doesn't exist, use a different key", key);
			return nullptr; // Return a null pointer
		}
		else // If the key is in the map
		{
			return it->second; // Return the pointer
		}
	}

	template<class G>
	void AssetManager<G>::clear()
	{
		// Clear the map
		m_container.clear();
	}
}