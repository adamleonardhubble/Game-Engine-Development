/** \file bufferElement.h
*/
#pragma once

#include "shaderData.h"

namespace Engine
{
	/**
	\class BufferElement
	\brief Element for the buffer, has a data type size and offset
	*/
	class BufferElement
	{
	public:
		ShaderDataType m_dataType; //!< The type of data it is
		unsigned int m_iSize; //!< The size of the element in bytes
		unsigned int m_iOffset; //!< The offset of the element
		bool m_bNormalized; //!< If the element is normalized

		//! Default constructor
		BufferElement() {}
		//! Constructor, sets the data type, the size, the offset, and if it is normalized
		/*!
		\param dataType The type of data it is
		\param normalized If the data is normalized, set to false by default
		*/
		BufferElement(ShaderDataType dataType, bool normalized = false) :
			m_dataType(dataType), m_iSize(ShaderDataTypeSize(dataType)), m_iOffset(0), m_bNormalized(normalized) {}
	};
}