/** \file OpenGLMaterial.cpp
*/
#include "engine_pch.h"

#include "platform/openGL/OpenGLMaterial.h"

namespace Engine
{
	OpenGLMaterial::OpenGLMaterial(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& VAO)
	{
		// Set the shader and the vertex array object to the ones passed in
		m_pShader = shader;
		m_pGeometry = VAO;
	}

	void OpenGLMaterial::setShader(const std::shared_ptr<Shader>& shader)
	{
		// Set the shader to the one passed in
		m_pShader = shader;
	}

	void OpenGLMaterial::setGeometry(const void* geometry)
	{
		// Set the vertex array object to the one passed in
		m_pGeometry = *(std::shared_ptr<VertexArray>*)geometry;
	}

	void OpenGLMaterial::setDataBlock(const std::map<std::string, void*>& data)
	{
		// Add the data passed in to the stored map
		m_data.insert(data.begin(), data.end());
	}

	void OpenGLMaterial::setDataElement(const std::string& dataName, void* data)
	{
		// Add the data passed in to the stored map
		m_data.insert(std::make_pair(dataName, data));
	}

	/*void OpenGLMaterial::setVertexData(float* vertices, unsigned int offset, unsigned int size)
	{

	}*/
}