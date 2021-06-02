/** \file bufferLayout.cpp
*/
#include "engine_pch.h"

#include "rendering/bufferLayout.h"

namespace Engine
{
	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements) : m_elements(elements)
	{
		// Calculate the stride and offset
		calcStrideAndOffsets();
	}

	void BufferLayout::addElement(ShaderDataType dataType)
	{
		// Add the element to the vector
		m_elements.push_back(BufferElement(dataType));
		calcStrideAndOffsets(); // Calculate the stride and offset
	}

	void BufferLayout::calcStrideAndOffsets()
	{
		// To set the offset of an element
		unsigned int offset = 0;
		m_iStride = 0; // Set the stride to 0
		// For each element in the vector
		for (auto& element : m_elements)
		{
			// Set the offset of the element 
			element.m_iOffset = offset;
			// Increase the offset to what it will be for the next element
			offset += element.m_iSize;
			// Increase the stride
			m_iStride += element.m_iSize;
		}
	}
}