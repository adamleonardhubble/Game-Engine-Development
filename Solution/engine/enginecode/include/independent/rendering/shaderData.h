/** \file shaderData.h
\brief Has the data type enum as well as functions which get info about them
*/
#pragma once

namespace Engine
{
	/**
	\class ShaderDataType
	\brief Types of data which will be used with shaders
	*/
	enum class ShaderDataType
	{
		None = 0, Int, Int2, Int3, Int4, Float, Float2, Float3, Float4, Mat3, Mat4, Bool, Sampler2D, SamplerCube
	};


	//! Used to get the size in bytes of a data type
	/*!
	\param type The type of data being used
	\return The size of the data type
	*/
	static unsigned int ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type) // Which type is it
		{
		case ShaderDataType::Int:          return 4;
		case ShaderDataType::Int2:         return 4 * 2;
		case ShaderDataType::Int3:         return 4 * 3;
		case ShaderDataType::Int4:         return 4 * 4;
		case ShaderDataType::Float:        return 4;
		case ShaderDataType::Float2:       return 4 * 2;
		case ShaderDataType::Float3:       return 4 * 3;
		case ShaderDataType::Float4:       return 4 * 4;
		case ShaderDataType::Mat3:         return 4 * 3 * 3;
		case ShaderDataType::Mat4:         return 4 * 4 * 4;
		case ShaderDataType::Bool:         return 1;
		case ShaderDataType::Sampler2D:    return 1;
		case ShaderDataType::SamplerCube:  return 1;
		}
		return 0;
	}

	//! Used to get the number of components in the data type
	/*!
	\param type The type of data being used
	\return The number of components of the data type
	*/
	static unsigned int ShaderDataTypeComponentCount(ShaderDataType type)
	{
		switch (type) // Which type is it
		{
		case ShaderDataType::Int:          return 1;
		case ShaderDataType::Int2:         return 2;
		case ShaderDataType::Int3:         return 3;
		case ShaderDataType::Int4:         return 4;
		case ShaderDataType::Float:        return 1;
		case ShaderDataType::Float2:       return 2;
		case ShaderDataType::Float3:       return 3;
		case ShaderDataType::Float4:       return 4;
		case ShaderDataType::Mat3:         return 3 * 3;
		case ShaderDataType::Mat4:         return 4 * 4;
		case ShaderDataType::Bool:         return 1;
		case ShaderDataType::Sampler2D:    return 1;
		case ShaderDataType::SamplerCube:  return 1;
		}
		return 0;
	}
}