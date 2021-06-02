/** \file OpenGLTextRenderer.cpp
*/
#include "engine_pch.h"

#include "platform/openGL/renderer/OpenGLTextRenderer.h"

#include <glad/glad.h>

namespace Engine
{
	void OpenGLTextRenderer::actionCommand(RenderCommand* command)
	{
		command->action(); // Perform the command
		if (command->toKill())
			delete command; // Delete the command
	}

	void OpenGLTextRenderer::beginScene(const SceneData& sceneData)
	{
		// For each pair in the sceneData map
		for (auto uboPair : sceneData)
		{
			//unsigned int offset = 0;
			//unsigned int size;
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
	}

	void OpenGLTextRenderer::submit(const std::shared_ptr<Material>& material)
	{
#ifdef NG_DEBUG // Only in debug mode
		TIMER_START("TextRendererSubmit");	// Start 'TextRendererSubmit' timer
#endif

		auto shader = material->getShader();
		shader->bind(); // Bind the shader

		// Get the geometry from the material
		auto geometry = *(std::shared_ptr<VertexArray>*)material->getGeometry();
		geometry->bind(); // Bind the geometry

		auto perDrawData = material->getData(); // Get the data to be uploaded
		// For each pair with uniform data
		for (auto dataPair : perDrawData)
		{
			// Call the function to upload the data
			shader->uploadData(dataPair.first, dataPair.second);
		}
		// Draw the material
		glDrawElements(GL_QUADS, geometry->getDrawCount(), GL_UNSIGNED_INT, nullptr);

#ifdef NG_DEBUG // Only in debug mode
		TIMER_END("TextRendererSubmit"); // Stop 'TextRendererSubmit' timer
#endif
	}
}