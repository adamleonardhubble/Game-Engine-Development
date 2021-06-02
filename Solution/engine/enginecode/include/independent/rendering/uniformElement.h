/** \file uniformElement.h
*/
#pragma once

#include "rendering/shaderData.h"

#include <string>
#include <functional>

namespace Engine
{
	//! UploadFunc is the function called to upload the uniform data
	using UploadFunc = std::function<bool(int, void*)>;

	/**
	\class UniformElement
	\brief API agnostic interface class for a shader uniform
	*/
	class UniformElement
	{
	public:
		//! Function to get the uniform location and store the function called to upload the data
		/*!
		\param shaderID The shader ID
		\param func The function that uploads the uniform data
		*/
		virtual void setLocationAndFunction(unsigned int shaderID, const UploadFunc& func) = 0;

		//! Function to get the uniform type
		/*!
		\return The type
		*/
		virtual ShaderDataType getType() = 0;
		//! Function to call the function to upload the data
		/*!
		\param data A void pointer to the data being uploaded
		*/
		virtual void uniformUpload(void* data) = 0;
	};
}