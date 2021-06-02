/** \file OpenGLSuperSimpleRenderer.cpp
*/
#include "engine_pch.h"

#include "platform/openGL/renderer/OpenGLSuperSimpleRenderer.h"

#include <glad/glad.h>

namespace Engine
{
	void OpenGLSuperSimpleRenderer::actionCommand(RenderCommand* command)
	{
		command->action(); // Perform the command
		if (command->toKill())
			delete command; // Delete the command
	}

	void OpenGLSuperSimpleRenderer::submit(const std::shared_ptr<Material>& material)
	{
		// Get the shader from the material
		auto shader = material->getShader();
		shader->bind(); // Bind the shader

		// Get the geometry from the material
		auto geometry = *(std::shared_ptr<VertexArray>*)material->getGeometry();
		geometry->bind(); // Bind the geometry

		auto uniformData = material->getData(); // Get the data to be uploaded
		// For each pair with uniform data
		for (auto dataPair : uniformData)
		{
			// Call the function to upload the data
			shader->uploadData(dataPair.first, dataPair.second);
		}
		// Draw the material
		glDrawElements(GL_TRIANGLES, geometry->getDrawCount(), GL_UNSIGNED_INT, nullptr);
	}
}