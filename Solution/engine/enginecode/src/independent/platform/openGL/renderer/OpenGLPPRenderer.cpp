/** \file OpenGLPPRenderer.cpp
*/

#include "engine_pch.h" // Include precompiled headers

#include "platform/openGL/renderer/OpenGLPPRenderer.h" // Include 'OpenGLPPRenderer.h' file

#include <glad/glad.h> // Include 'glad.h' file

#include <glm/gtc/matrix_transform.hpp>

namespace Engine // Engine namespace
{
	OpenGLPPRenderer::OpenGLPPRenderer(std::shared_ptr<Shader> shader)
	{
		m_pFBShader = shader; // Set the current frame buffer shader

		float textureVertices[] = { // Vertex attributes for a quad that fills the entire screen in normalized device coordinates
			// Positions      // TexCoords	// Normals
			0.0f,   600.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f, // Bottom-left
			0.0f,	0.0f,     0.0f, 0.0f,   0.0f, 1.0f, 0.0f, // Top-left
			800.0f, 0.0f,     1.0f, 0.0f,   0.0f, 1.0f, 0.0f, // Top-right
			800.0f, 600.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f  // Bottom-right
		};

		unsigned int textureIndices[] = { // Indice attributes for two triangles that fills the entire screen 
			 1, 2, 3, // Top-left, top-right, bottom-right
			 0, 1, 3  // Bottom-left, top-left, bottom-right
		};

		m_pFBVAO = ResourceManager::addVAO("FrameBuffer"); // Add vertex array object for frame buffers

		m_pFBVAO->setVertexBuffer(ResourceManager::addVBO("FrameBuffer", textureVertices, sizeof(float) * 28, m_pFBShader->getBufferLayout())); // Add vertex buffer for frame buffers
		m_pFBVAO->setIndexBuffer(ResourceManager::addIndexBuffer("FrameBuffer", textureIndices, 6)); // Add index buffer for frame buffers

		m_pFBMaterial = ResourceManager::addMaterial("FrameBuffer", m_pFBShader, m_pFBVAO); // Add material for frame buffers

		m_viewProjectionMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f); // Setup the orthographic view projection matrix, made relative to the windows original resolution

		std::shared_ptr<UniformBuffer> pUBO = ResourceManager::getUBO("FBMatrix"); // Create and appoint a shared pointer to the frame buffer matrix uniform buffer object (binding point)
		std::vector<void*> uboData; // Create a vector container, used to store the uniform buffer data for frame buffers

		uboData.push_back(&m_viewProjectionMatrix);	// Store the view project matrix uniform variable data

		m_sceneData[pUBO] = uboData; // Store the uniform buffer object data, as the scene data
	}

	void OpenGLPPRenderer::actionCommand(RenderCommand* command)
	{
		command->action(); // Perform the command
		if (command->toKill())
			delete command; // Delete the command
	}

	void OpenGLPPRenderer::submit()
	{
		m_pFBShader->bind(); // Bind the shader

		// For each pair of variables for the scene data (uniform buffer data)
		for (auto uboPair : m_sceneData)
		{
			int i = 0; // To iterate through the vector of data

			UniformBufferLayout layout = uboPair.first->getLayout(); // Get the layout from the uniform buffer
			// For each element in the layout
			for (auto bufferElement : layout)
			{
				// Call the unifomr buffer function to set the data with the offset, the size and the data
				uboPair.first->setData(bufferElement.m_iOffset, bufferElement.m_iSize, uboPair.second[i]);
				i++; // To set the next data
			}
		}

		auto geometry = *(std::shared_ptr<VertexArray>*)m_pFBMaterial->getGeometry(); // Store the geometry from the passed material
		geometry->bind(); // Bind the geometry

		// For each pair of variables for the uniform data
		for (auto dataPair : m_PPUniforms)
		{
			// Upload the shader uniform data, to the GPU 
			m_pFBShader->uploadData(dataPair.first, dataPair.second);
		}
		
		glDrawElements(GL_TRIANGLES, geometry->getDrawCount(), GL_UNSIGNED_INT, nullptr); // Submit a draw call, for the materiala geometry
	}

	void OpenGLPPRenderer::setPPShader(std::shared_ptr<Shader>& shader)
	{
		m_pFBShader = shader; // Set the current frame buffer shader

		m_pFBMaterial->setShader(m_pFBShader); // Update the frame buffer material, passing the updated shader
	}

	void OpenGLPPRenderer::addPPUniform(const std::string & uniformName, void * data)
	{
		m_PPUniforms.insert(std::make_pair(uniformName, data)); // Insert a uniform variable into the map
	}

	void OpenGLPPRenderer::deletePPUniform(const std::string& uniformName)
	{
		std::map<std::string, void*>::iterator uniformIterator; // Create a uniform map iterator

		uniformIterator = m_PPUniforms.find(uniformName); // Set the uniform map iterator to find the passed uniform variable

		if (uniformIterator != m_PPUniforms.end()) // If the uniform map iterator has not reached the end of the map, do the following
		{
			m_PPUniforms.erase(uniformIterator); // Erase the passed uniform variable

			LOG_TRACE("Post-processing uniform variable [{0}] has been erased!", uniformName); // Output type 'trace', a post-processing uniform variable has been erased 
		}
		else // Else if the uniform map iterator has not found the passed uniform variable, do the following
		{
			LOG_CRITICAL("Post-processing uniform variable [{0}] does not exist!", uniformName); // Output type 'trace', a post-processing uniform variable does not exist
		}
	}
}