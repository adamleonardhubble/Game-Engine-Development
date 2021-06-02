/** \file OpenGLBasicRenderer.h
*/
#pragma once

#include "rendering/renderer/renderer.h"

#include "systems/timer.h"

namespace Engine
{
	/**
	\class OpenGLBasicRenderer
	\brief Open GL specific rendering class which draws everything while separating the scene data from the draw data
	*/
	class OpenGLBasicRenderer : public Renderer
	{
	public:
		//! Function to perform a render command
		/*!
		\param command A pointer to the command class which will do something
		*/
		void actionCommand(RenderCommand* command) override;
		//! Function to prepare a scene for submission
		/*!
		\param sceneData A map of pointers to uniform buffers and the data to be set
		*/
		void beginScene(const SceneData& sceneData) override;
		//! End a scene
		void endScene() override {}
		//! Function to submit a material for drawing
		/*!
		\param material A pointer to the material being drawn
		*/
		void submit(const std::shared_ptr<Material>& material) override;
		void flush() override {} //!< Draw everything
	};
}