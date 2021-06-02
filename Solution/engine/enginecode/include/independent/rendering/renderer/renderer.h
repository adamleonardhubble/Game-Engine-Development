/** \file renderer.h
*/
#pragma once

#include "renderCommand.h"
#include "rendering/material.h"
#include "rendering/uniformBuffer.h"

#include <map>

namespace Engine
{
	//! SceneData is a map of shared pointers to uniform buffers and vectors containing the data to be uploaded
	using SceneData = std::unordered_map<std::shared_ptr<UniformBuffer>, std::vector<void *>>;
	//! PerDrawData is a map of strings (uniform names) and the data to be uploaded
	using PerDrawData = std::map<std::string, void *>;

	/**
	\class Renderer
	\brief Base class for any renderer
	*/
	class Renderer
	{
	public:
		//! Function to perform a render command
		/*!
		\param command A pointer to the command class which will do something
		*/
		virtual void actionCommand(RenderCommand* command) = 0;
		//! Function to prepare a scene for submission
		/*!
		\param sceneData A map of pointers to uniform buffers and the data to be set
		*/
		virtual void beginScene(const SceneData& sceneData) = 0;
		//! End a scene
		virtual void endScene() = 0;
		//! Function to submit a material for drawing
		/*!
		\param material A pointer to the material being drawn
		*/
		virtual void submit(const std::shared_ptr<Material>& material) = 0;
		virtual void flush() = 0; //!< Draw everything

		//! Function to create a super simple renderer
		/*!
		\return A pointer to the renderer
		*/
		static Renderer* createSimple3D();
		//! Function to create a basic renderer
		/*!
		\return A pointer to the renderer
		*/
		static Renderer* createBasic3D();
		//! Function to create a text renderer
		/*!
		\return A pointer to the renderer
		*/
		static Renderer* createText();
	};
}