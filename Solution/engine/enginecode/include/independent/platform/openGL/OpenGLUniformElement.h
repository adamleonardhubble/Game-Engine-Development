/** \file OpenGLUniformElement.h
*/
#pragma once

#include "rendering/uniformElement.h"

#include <glad/glad.h>

namespace Engine
{
	/**
	\class OpenGLUniformElement
	\brief OpenGL class for a shader uniform
	*/
	class OpenGLUniformElement : public UniformElement
	{
	private:
		std::string m_sName; //!< The name of the uniform
		ShaderDataType m_type; //!< The type of the uniform
		unsigned int m_location; //!< The location of the uniform

		UploadFunc m_uploadFunc; //!< The function to call to upload the data
	public:
		//! Constructor, Sets the name and the type
		/*!
		\param name The name of the uniform
		\param type The type of the uniform
		*/
		OpenGLUniformElement(std::string& name, ShaderDataType type) : m_sName(name), m_type(type) {}

		//! Function to get the location of the uniform and set the function to set the data
		/*!
		\param shaderID The shader ID
		\param func The function that uploads the uniform data
		*/
		void setLocationAndFunction(unsigned int shaderID, const UploadFunc& func) override
		{
			m_location = glGetUniformLocation(shaderID, m_sName.c_str()); // Get the location
			m_uploadFunc = func; // Set the upload function
		}

		//! Function to get the type
		/*!
		\return The type
		*/
		inline ShaderDataType getType() override { return m_type; }
		//! Function to call the function to upload the data
		/*!
		\param data A void pointer to the data being uploaded
		*/
		inline void uniformUpload(void* data) override { m_uploadFunc(m_location, data); }
	};
}