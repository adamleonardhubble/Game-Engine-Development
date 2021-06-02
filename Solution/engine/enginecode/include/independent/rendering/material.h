/** \file material.h
*/
#pragma once

#include "shader.h"
#include "vertexArray.h"
#include "vertexBuffer.h"

namespace Engine
{
	/**
	\class Material
	\brief Base class for all materials
	*/
	class Material
	{
	public:
		//! Function to set the shader of the material
		/*!
		\param shader A poiner to the shader
		*/
		virtual void setShader(const std::shared_ptr<Shader>& shader) = 0;
		//! Function to set the geometry
		/*!
		\param geometry Void pointer will either be to a vertex array object or a vertex buffer depending on the rendering API
		*/
		virtual void setGeometry(const void* geometry) = 0;
		//! Function to set a block of uniform data
		/*!
		\param data A map of names of uniforms and void pointers to the data being uploaded
		*/
		virtual void setDataBlock(const std::map<std::string, void*>& data) = 0;
		//! Function to set a uniform data element
		/*!
		\param dataName The name of the data being set
		\param data A void pointer to the data
		*/
		virtual void setDataElement(const std::string& dataName, void* data) = 0;
		//virtual void setVertexData(float* vertices, unsigned int offset, unsigned int size) = 0;

		//! Function to get the shader
		/*!
		\return A pointer to the shader
		*/
		virtual std::shared_ptr<Shader> getShader() = 0;
		//! Function to get the geometry
		/*!
		\return Void pointer will either be to a vertex array object or a vertex buffer depending on the rendering API
		*/
		virtual void* getGeometry() = 0;
		//! Function to get the uniform data
		/*!
		\return The data being uploaded
		*/
		virtual std::map<std::string, void*> getData() = 0;

		//! Function to create a material
		/*!
		\param shader A pointer to the shader
		\param VAO A pointer to the vertex array object
		\return A pointer to the material
		*/
		static Material* create(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& VAO);
		//! Function to create a material
		/*!
		\param shader A pointer to the shader
		\param VBO A pointer to the vertex buffer object
		\return A pointer to the material
		*/
		static Material* create(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexBuffer>& VBO);
	};
}