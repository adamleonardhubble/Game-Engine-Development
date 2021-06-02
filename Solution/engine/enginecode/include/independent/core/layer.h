/** \file layer.h
*/
#pragma once

#include "systems/resourceManager.h"
#include "rendering/renderer/renderer.h"
#include "cameras/cameraController.h"

#include "rendering/renderer/PPrenderer.h"

namespace Engine
{
	/**
	\class Layer
	\brief Base class for layers to go in the layer stack
	*/
	class Layer
	{
	protected:
		std::string m_name; //!< The name of the layer
		std::shared_ptr<ResourceManager> m_pResources; //!< Pointer to the resource manager
		std::shared_ptr<Renderer> m_pRenderer; //!< Pointer to a renderer
		std::shared_ptr<CameraController> m_pCamera; //!< Pointer to a camera

		SceneData m_sceneData; //!< Scene data for rendering
		static SceneData s_lightSceneData; //!< Scene data for lights, used in layers that use the scene lighting

		bool m_bIs3DLayer = false; //!< If the layer is 3D, whether or not to use the lighting
	public:
		//! Constructor
		/*!
		\param layer The name of the layer
		*/
		Layer(const std::string& layer) : m_name(layer) {}

		//! Function called when the layer is added
		virtual void onAttach() = 0;
		//! Function called when the layer is removed
		virtual void onDetach() = 0;
		//! Function to update the layer every frame
		/*!
		\param timestep The time since the previous frame
		*/
		virtual void onUpdate(float timestep) = 0;
		virtual void drawObjects() = 0;
		//! Function to draw the texture that the layer is rendered to
		virtual void drawLayer() = 0;
		//! Function to handle events
		/*!
		\param e The event
		*/
		virtual void onEvent(Event& e) = 0;

		//! Function to get the name of the layer
		/*!
		\return The name of the layer
		*/
		inline const std::string& getName() const { return m_name; }

		//! Function to get the pointer to the camera controller
		/*!
		\return A pointer to the camera controller
		*/
		inline std::shared_ptr<CameraController>& getCamera() { return m_pCamera; }
		//! Function to get the resourceManager
		/*!
		\return A pointer to the resource manager
		*/
		inline std::shared_ptr<ResourceManager> getResources() { return m_pResources; }
		//! Function to get the renderer
		/*!
		\return A pointer to the renderer
		*/
		inline std::shared_ptr<Renderer>& getRenderer() { return m_pRenderer; }
		//! Function to get the scene data
		/*!
		\return A pointer to the scene data
		*/
		inline SceneData* getSceneData() { return &m_sceneData; }
		//! Function to get the light scene data
		/*!
		\return A pointer to the light scene data
		*/
		inline SceneData* getLightSceneData() { return &s_lightSceneData; }
		//! Function to set if the layer is a 3D layer
		/*!
		\param is3DLayer If the layer is 3D
		*/
		void setIf3DLayer(bool is3DLayer) { m_bIs3DLayer = is3DLayer; }

		bool getIf3DLayer() { return m_bIs3DLayer; }

		//! Function called after all layers onAttach functions have been called to add the lighting to the layer scene data
		void setLightingSceneData() {
			if (m_bIs3DLayer)
			{
				m_sceneData.insert(s_lightSceneData.begin(), s_lightSceneData.end());
			}
		}
	};
}