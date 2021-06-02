/** \file PPrenderer.h
*/

#pragma once // Compile once

#include "renderCommand.h" // Include 'renderCommand.h' file

#include "renderer.h" // Include 'renderer.h' file

#include "rendering/material.h" // Include 'material.h' file

#include "rendering/uniformBuffer.h" // Include 'uniformBuffer.h' file

#include "systems/log.h" // Include 'log.h' file

#include "systems/resourceManager.h" // Include 'resourceManager' file

#include <map> // Include map support

namespace Engine
{
	//! PerDrawData is a map of strings (uniform names) and the data to be uploaded
	using PerDrawData = std::map<std::string, void *>;

	/**
	\class PPRenderer
	\brief Base class for any post-processing renderer
	*/
	class PPRenderer 
	{
	public: // Public members
		//! Function to create a post-processing renderer
		/*!
		\param shader A pointer to the shader being set
		\return A pointer to the post-processing renderer
		*/
		static PPRenderer* createPPRenderer(std::shared_ptr<Shader> shader);

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
		virtual void submit() = 0;

		virtual void flush() = 0; //!< Draw everything

		virtual void setPPShader(std::shared_ptr<Shader>& shader) = 0; //!< Set the shader being used by the post-processing renderer

		virtual void addPPUniform(const std::string& uniformName, void * data) = 0;	//!< Add a shader uniform variable to the post-processing uniform map

		virtual void deletePPUniform(const std::string& uniformName) = 0; //!< Delete a shader uniform variable from the post-processing uniform map

	};
}
