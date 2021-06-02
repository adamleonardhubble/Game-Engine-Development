/** \file displayLayer.cpp
*/
#include <Engine.h>

#include "layers/displayLayer.h"
#include "layers/load/loadEditor.h"

#include <glad/glad.h>

void DisplayLayer::onAttach()
{
	// Load the layer
	EditorJSONLoader::load(m_layerData.m_filepath, *this);

	m_pFrameBuffer.reset(Engine::FrameBuffer::create()); // Intialise a frame buffer object instance

	// Vertex and index data for the skybox
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f
	};

	unsigned int skyboxIndices[] = {
		0, 1, 2,
		2, 3, 0,
		4, 1, 0,
		0, 5, 4,
		2, 6, 7,
		7, 3, 2,
		4, 5, 7,
		7, 6, 4,
		0, 3, 7,
		7, 5, 0,
		1, 4, 2,
		2, 4, 6
	};

	std::shared_ptr<Engine::Shader> tempShader = Engine::ResourceManager::addShader("assets/shaders/skybox.glsl"); // Add shader
	std::shared_ptr<Engine::VertexArray> tempVAO = Engine::ResourceManager::addVAO("SkyBox"); // Add vertex array object

	tempVAO->setVertexBuffer(Engine::ResourceManager::addVBO("SkyBox", skyboxVertices, sizeof(float) * 108, tempShader->getBufferLayout())); // Add vertex buffer
	tempVAO->setIndexBuffer(Engine::ResourceManager::addIndexBuffer("SkyBox", skyboxIndices, 36));	// Add index buffer

	m_pSkyBoxMat = Engine::ResourceManager::addMaterial("SkyBox", tempShader, tempVAO); // Add material

	// The filepaths for the skybox face textures
	std::vector<std::string> faces;
	faces.push_back("assets/skybox/right.png");
	faces.push_back("assets/skybox/left.png");
	faces.push_back("assets/skybox/top.png");
	faces.push_back("assets/skybox/bottom.png");
	faces.push_back("assets/skybox/front.png");
	faces.push_back("assets/skybox/back.png");
	// Create skybox cubemap texture
	m_pSkyBoxTex = Engine::ResourceManager::addCubemap(faces);

	m_pFBShaderShadowLightSpace = Engine::ResourceManager::addShader("assets/shaders/frameBufferShadowLightSpace.glsl");

	// Call all init render commands
	for (auto& renderCommand : m_initCommands)
		m_pRenderer->actionCommand(renderCommand.get());
}

void DisplayLayer::onDetach()
{
	// Call exit render commands
	for (auto& renderCommand : m_exitCommands) m_pRenderer->actionCommand(renderCommand.get());

	for (auto& ptr : m_data)
	{
		delete ptr;
	}
}

void DisplayLayer::onUpdate(float timestep)
{
	if (m_bMousePressed)
		m_pCamera->onUpdate(timestep);

	// Set camera variables used for uniform buffers
	m_camViewProj = m_pCamera->getCamera()->getViewProjection();
	m_camView = m_pCamera->getCamera()->getView();
	m_camProj = m_pCamera->getCamera()->getProjection();
	m_camPos = m_pCamera->getCamera()->getPosition();

#pragma region Shadow Depth
	auto lightObject = *m_lights.back()->getObject()->getComponent<Engine::PositionComponent>(); // Store the light component used by the scene
	std::shared_ptr<Engine::PositionComponent> lightPositionComponent = std::dynamic_pointer_cast<Engine::PositionComponent>(lightObject); // Cast the light object (dynamic)
	glm::vec3 lightObjectRotation = lightPositionComponent->getRotation(); // Store the light objects rotation vector
	glm::vec3 lightObjectPosition = lightPositionComponent->getPosition(); // Store the light objects position vector

	glm::mat4 lightProjection = glm::ortho<float>(-20.0f, 20.0f, -20.0f, 20.0f, 3.f, 60.f); // Create a light projection matrix (orthogrpahic projection)
	glm::mat4 lightView = glm::lookAt(lightObjectPosition, lightObjectRotation, glm::vec3(0.f, 1.f, 0.f)); // Create a viewpoint representing the light objects position and facing direction
	m_lightSpaceMatrix = lightProjection * lightView; // Calculate the light space matrix

	m_pFrameBuffer->shadowBind(); // Bind the shadow map frame buffer

	for (auto& renderCommand : m_predrawCommands)
		m_pRenderer->actionCommand(renderCommand.get());

	for (auto& mat : m_materials)
	{
		std::pair<std::string, void*> lightSpace("u_lightSpaceMatrix", (void*)&m_lightSpaceMatrix[0][0]); // Store the light space matrix data
		Engine::ComponentMessage matrixMessage(Engine::ComponentMessageType::UniformSet, (void*)&lightSpace); // Submit the matrix uniform as a component message
		mat->receiveMessage(matrixMessage);	// Apply the recieved uniform data to the material

		std::shared_ptr<Engine::Shader> objectShader = mat->getMaterial()->getShader();	// Store the shader currently used by the material
		mat->getMaterial()->setShader(m_pFBShaderShadowLightSpace);	// Set the shader used by the material to the light space shader

		m_pRenderer->submit(mat->getMaterial()); // Draw geometry to texture (off-screen rendering)
		mat->getMaterial()->setShader(objectShader); // Set the shader used by the material to the default shader
	}

	for (auto& renderCommand : m_postdrawCommands)
		m_pRenderer->actionCommand(renderCommand.get());

	m_pFrameBuffer->shadowUnbind();	// Unbind the shadow map frame buffer
#pragma endregion

	m_pFrameBuffer->defaultBind();	// Bind the frame buffer
	m_pRenderer->beginScene(m_sceneData);

	// Render the scene with the editor camera
	for (auto& renderCommand : m_predrawCommands)
		m_pRenderer->actionCommand(renderCommand.get());
	
	unsigned int depthMapTextureSlot = m_pFrameBuffer->getDepthMapTextureSlot(); // Store the depth texture slot of the depth map texture

	int i = 0;
	for (auto& mat : m_materials)
	{
		for (auto& object : m_layerData.m_objects)
		{
			if (object.m_materialComponentVectorValue == i)
			{
				std::pair<std::string, void*> depthMap("u_shadowDepth", (void*)&depthMapTextureSlot); // Store the depth map texture slot data
				Engine::ComponentMessage textureMessage(Engine::ComponentMessageType::UniformSet, (void*)&depthMap); // Submit the texture uniform as a component message
				mat->receiveMessage(textureMessage); // Apply the recieved uniform data to the material

				std::pair<std::string, void*> lightSpace("u_lightSpaceMatrix", (void*)&m_lightSpaceMatrix[0][0]); // Store the light space matrix data
				Engine::ComponentMessage matrixMessage(Engine::ComponentMessageType::UniformSet, (void*)&lightSpace); // Submit the matrix uniform as a component message
				mat->receiveMessage(matrixMessage); // Apply the recieved uniform data to the material

				m_pRenderer->submit(mat->getMaterial()); // Draw geometry to texture (off-screen rendering)

				break;
			}
		}
		i++;
	}

	// Render the skybox
	glDepthFunc(GL_LEQUAL);
	unsigned int texSlot = m_pSkyBoxTex->getSlot();
	m_pSkyBoxMat->setDataElement("u_skybox", (void*)&texSlot);
	m_pSkyBoxMat->setDataElement("u_projection", (void*)&m_camProj[0]);
	glm::mat4 view = glm::mat4(glm::mat3(m_pCamera->getCamera()->getView()));
	m_pSkyBoxMat->setDataElement("u_view", (void*)&view[0]);
	m_pRenderer->submit(m_pSkyBoxMat);
	glDepthFunc(GL_LESS);
	
	for (auto& renderCommand : m_postdrawCommands)
		m_pRenderer->actionCommand(renderCommand.get());

	m_pFrameBuffer->defaultUnbind(); // Unbind the frame buffer
	
	ImVec2 windowSize;
	windowSize.x = 800 * m_displayWindowMultiplier;
	windowSize.y = windowSize.x / 1.33333333f;
	// Draw the texture in an imgui window
	ImGui::Image((void*)m_pFrameBuffer->getColourTextureID(0), windowSize, ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::IsItemHovered())
	{
		m_bHoveringOnImage = true;
	}
	else
	{
		m_bHoveringOnImage = false;
	}
}

void DisplayLayer::onEvent(Engine::Event& event)
{
	// Create event dispatcher
	Engine::EventDispatcher dispatcher(event);
	// If the Event type matches, call the corresponding function
	dispatcher.dispatch<Engine::MouseButtonPressedEvent>(std::bind(&DisplayLayer::onMouseButtonPress, this, std::placeholders::_1));
	dispatcher.dispatch<Engine::MouseButtonReleasedEvent>(std::bind(&DisplayLayer::onMouseButtonRelease, this, std::placeholders::_1));
	dispatcher.dispatch<Engine::MouseScrolledEvent>(std::bind(&DisplayLayer::onMouseScrolled, this, std::placeholders::_1));
}

bool DisplayLayer::onMouseButtonPress(Engine::MouseButtonPressedEvent& event)
{
	// If the mouse is over the image
	if (m_bHoveringOnImage)
	{
		m_pCamera->onEvent(event);

		if (event.getMouseButton() == ENGINE_MOUSE_BUTTON_LEFT || event.getMouseButton() == ENGINE_MOUSE_BUTTON_RIGHT)
		{
			m_bMousePressed = true;
		}
	}
	return false;
}

bool DisplayLayer::onMouseButtonRelease(Engine::MouseButtonReleasedEvent& event)
{
	m_pCamera->onEvent(event);

	m_bMousePressed = false;
	return false;
}

bool DisplayLayer::onMouseScrolled(Engine::MouseScrolledEvent& event)
{
	if (m_bHoveringOnImage)
		m_pCamera->onEvent(event);

	return false;
}