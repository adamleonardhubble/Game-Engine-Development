/** \file OpenGLMaterial.h
*/
#pragma once

#include "rendering/material.h"

namespace Engine
{
	/**
	\class OpenGLMaterial
	\brief Open GL specific material

	Class containing a shader, geometry and uniform data for the renderer to draw
	*/
	class OpenGLMaterial : public Material
	{
	private:
		std::shared_ptr<Shader> m_pShader; //!< Pointer to the shader
		std::shared_ptr<VertexArray> m_pGeometry; //!< Pointer to the vertex array (geometry to be drawn)
		std::map<std::string, void*> m_data; //!< Map of uniform data to upload and the name of the uniform
	public:
		//! Constructor
		/*!
		\param shader A pointer to the shader
		\param VAO A pointer to the vertec array object
		*/
		OpenGLMaterial(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& VAO);

		//! Function to set the shader of the material
		/*!
		\param shader A poiner to the shader
		*/
		void setShader(const std::shared_ptr<Shader>& shader) override;
		//! Function to set the geometry
		/*!
		\param geometry Void pointer for a vertex array object for opengl
		*/
		void setGeometry(const void* geometry) override;
		//! Function to set a block of uniform data
		/*!
		\param data A map of names of uniforms and void pointers to the data being uploaded
		*/
		void setDataBlock(const std::map<std::string, void*>& data) override;
		//! Function to set a uniform data element
		/*!
		\param dataName The name of the data being set
		\param data A void pointer to the data
		*/
		void setDataElement(const std::string& dataName, void* data) override;

		//void setVertexData(float* vertices, unsigned int offset, unsigned int size) override;

		//! Function to get the shader
		/*!
		\return A pointer to the shader
		*/
		inline std::shared_ptr<Shader> getShader() override { return m_pShader; }
		//! Function to get the geometry
		/*!
		\return Void pointer for a vertex array object for opengl
		*/
		inline void* getGeometry() override { return (void*)&m_pGeometry; }
		//! Function to get the uniform data
		/*!
		\return The data being uploaded
		*/
		inline std::map<std::string, void*> getData() override { return m_data; }
	};
}