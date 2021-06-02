/** \file layerstack.h
*/
#pragma once

#include "systems/system.h"
#include "core/layer.h"

namespace Engine
{
	/**
	\class Layerstack
	\brief Contains all of the layers in the game
	*/
	class Layerstack : public System
	{
	private:
		std::vector<std::shared_ptr<Layer>> m_layers; //!< Container of pointers to the layers
	public:
		void start(SystemSignal init = SystemSignal::None, ...) override {} //!< A function to start the layerstack
		void stop(SystemSignal close = SystemSignal::None, ...) override {} //!< A function to stop the layerstack

		//! Function to add a layer
		/*!
		\param layer A pointer to the layer being added
		*/
		void push(std::shared_ptr<Layer> layer) { m_layers.push_back(layer); m_layers.back()->onAttach(); }
		//! Function to remove a layer
		void pop() { m_layers.back()->onDetach(); m_layers.pop_back(); }

		//! Used to set an iterator to the start of the vector
		/*!
		\return An iterator at the start of the vector
		*/
		inline std::vector<std::shared_ptr<Layer>>::iterator begin() { return m_layers.begin(); }
		//! Used to set an iterator to the end of the vector
		/*!
		\return An iterator at the end of the vector
		*/
		inline std::vector<std::shared_ptr<Layer>>::iterator end() { return m_layers.end(); }

		//! Used to set a reverse iterator to the end of the vector
		/*!
		\return A reverse iterator at the end of the vector
		*/
		inline std::vector<std::shared_ptr<Layer>>::reverse_iterator rbegin() { return m_layers.rbegin(); }
		//! Used to set a reverse iterator to the start of the vector
		/*!
		\return A reverse iterator at the start of the vector
		*/
		inline std::vector<std::shared_ptr<Layer>>::reverse_iterator rend() { return m_layers.rend(); }
	};
}