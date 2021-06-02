/** \file OpenGLPPRenderer.h
*/

#pragma once // Compile once

#include "rendering/renderer/PPrenderer.h" // Include 'PPrenderer.h' file

#include <glm/glm.hpp>

namespace Engine
{
	/**
	\class OpenGLPPRenderer
	\brief OpenGL specific post-processing rendering class which draws everything while separating the scene data from the draw data
	*/
	class OpenGLPPRenderer : public PPRenderer
	{
	private: // Private members#
		std::shared_ptr<Shader> m_pFBShader; //!< Shared pointer to the shader class

		std::shared_ptr<VertexArray> m_pFBVAO; //!< Shared pointer to the vertex array class

		std::shared_ptr<Material> m_pFBMaterial; //!< Shared pointer to the material class

		PerDrawData m_PPUniforms; //!< PerDrawData map object, store the post-processing uniform variables

		SceneData m_sceneData; //!< Scene data, frame buffer related uniform buffer data to be uploaded

		glm::mat4 m_viewProjectionMatrix; //!< Matrix(4x4), store the view projection matrix of the camera

	public: // Public members
		//! Constructor, set the default shader
		/*!
		\param shader A pointer to the shader being set
		*/
		OpenGLPPRenderer(std::shared_ptr<Shader> shader);

		//! Function to perform a render command
		/*!
		\param command A pointer to the command class which will do something
		*/
		void actionCommand(RenderCommand* command) override;

		//! Function to prepare a scene for submission
		/*!
		\param sceneData A map of pointers to uniform buffers and the data to be set
		*/
		void beginScene(const SceneData& sceneData) override {}

		//! End a scene
		void endScene() override {}

		//! Function to submit a material for drawing
		/*!
		\param material A pointer to the material being drawn
		*/
		void submit() override;

		void flush() override {} //!< Draw everything

		//! Set the current post-processing shader
		/*!
		\param Shader A pointer to the shader being set
		*/
		virtual void setPPShader(std::shared_ptr<Shader>& shader) override;

		//! Add a uniform variable to the uniform map
		/*!
		\param uniformName A string representing the name of the uniform variable being added
		\param data The data associated to the uniform variable
		*/
		virtual void addPPUniform(const std::string& uniformName, void * data) override;

		//! Delete a uniform variable from the uniform map
		/*!
		\param uniformName A string representing the name of the uniform variable being deleted
		*/
		virtual void deletePPUniform(const std::string& uniformName) override;
	};
}