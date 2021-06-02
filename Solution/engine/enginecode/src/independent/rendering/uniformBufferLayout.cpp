/** \file uniformBufferLayout.cpp
*/
#include "engine_pch.h"

#include "rendering/uniformBufferLayout.h"

namespace Engine
{
	UniformBufferLayout::UniformBufferLayout(const std::initializer_list<BufferElement>& elements) : m_elements(elements)
	{
		// Calculate the stride and offset
		calcStrideAndOffsets();
	}

	void UniformBufferLayout::addElement(ShaderDataType dataType)
	{
		// Add the element to the vector
		m_elements.push_back(BufferElement(dataType));
		calcStrideAndOffsets(); // Calculate the stride and offset
	}

	void UniformBufferLayout::calcStrideAndOffsets()
	{
		// To set the offset of an element
		unsigned int offset = 0;
		m_iStride = 0; // Set the stride to 0
		// For each element in the vector
		for (auto& element : m_elements)
		{
			// Set the offset of the element 
			element.m_iOffset = offset;
			// Set the total size of the layout to the offset + the size of the current element
			m_iSize = offset + element.m_iSize;

			// Get the remainder when the size is divided by 64
			int remainder = element.m_iSize % 16;
			if (remainder == 0) // If size is multiple of 64
			{
				// Increase the offset to what it will be for the next element
				offset += element.m_iSize;
				// Increase the stride
				m_iStride += element.m_iSize;
			}
			else // If size is not multiple of 64
			{
				// Round size up to the nearest multiple of 64 and increase the offset
				offset += ((element.m_iSize + 16) - remainder);
				// Increase the stride
				m_iStride += ((element.m_iSize + 16) - remainder);
			}
		}
	}
}