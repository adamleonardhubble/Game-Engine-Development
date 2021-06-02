/** \file gameLayer.cpp
*/
#include <Engine.h>
#include "gameLayer.h"

void GameLayer::onAttach()
{
	// Make a new basic renderer
	m_pRenderer.reset(Engine::Renderer::createBasic3D());

	float FCvertices[6 * 24] = {
		-0.5f, -0.5f, -0.5f, 0.8f, 0.2f, 0.2f, // red square
		 0.5f, -0.5f, -0.5f, 0.8f, 0.2f, 0.2f,
		 0.5f,  0.5f, -0.5f, 0.8f, 0.2f, 0.2f,
		-0.5f,  0.5f, -0.5f,  0.8f, 0.2f, 0.2f,
		-0.5f, -0.5f, 0.5f, 0.2f, 0.8f, 0.2f, // green square
		 0.5f, -0.5f, 0.5f, 0.2f, 0.8f, 0.2f,
		 0.5f,  0.5f, 0.5f, 0.2f, 0.8f, 0.2f,
		-0.5f,  0.5f, 0.5f,  0.2f, 0.8f, 0.2f,
		-0.5f, -0.5f, -0.5f, 0.8f, 0.2f, 0.8f, // magenta(ish) square
		 0.5f, -0.5f, -0.5f, 0.8f, 0.2f, 0.8f,
		 0.5f, -0.5f, 0.5f, 0.8f, 0.2f, 0.8f,
		-0.5f, -0.5f, 0.5f,  0.8f, 0.2f, 0.8f,
		-0.5f, 0.5f, -0.5f, 0.8f, 0.8f, 0.2f, // yellow square 
		 0.5f, 0.5f, -0.5f, 0.8f, 0.8f, 0.2f,
		 0.5f, 0.5f, 0.5f, 0.8f, 0.8f, 0.2f,
		-0.5f, 0.5f, 0.5f,  0.8f, 0.8f, 0.2f,
		-0.5f, -0.5f, -0.5f, 0.2f, 0.8f, 0.8f, // Cyan(ish) square 
		-0.5f,  0.5f, -0.5f,  0.2f, 0.8f, 0.8f,
		-0.5f,  0.5f, 0.5f, 0.2f, 0.8f, 0.8f,
		-0.5f,  -0.5f, 0.5f, 0.2f, 0.8f, 0.8f,
		0.5f, -0.5f, -0.5f, 0.2f, 0.2f, 0.8f, // Blue square 
		0.5f,  0.5f, -0.5f,  0.2f, 0.2f, 0.8f,
		0.5f,  0.5f, 0.5f, 0.2f, 0.2f, 0.8f,
		0.5f,  -0.5f, 0.5f, 0.2f, 0.2f, 0.8f
	};

	unsigned int indices[3 * 12] = {
		2, 1, 0,
		0, 3, 2,
		4, 5, 6,
		6, 7, 4,
		8, 9, 10,
		10, 11, 8,
		14, 13, 12,
		12, 15, 14,
		18, 17, 16,
		16, 19, 18,
		20, 21, 22,
		22, 23, 20
	};

	// Temporary pointers to create shaders, vertex array objects and materials
	std::shared_ptr<Engine::Shader> tempSetupShader;
	std::shared_ptr<Engine::VertexArray> tempSetupVAO;

	// Create flat colour shader from shader file
	tempSetupShader = m_pResources->addShader("assets/shaders/flatColour2.glsl");
	// Create a vertex array object
	tempSetupVAO = m_pResources->addVAO("flatColourCube");
	// Create a vertex buffer and an index buffer and give pointers to them to the vertex array object
	tempSetupVAO->setVertexBuffer(m_pResources->addVBO("flatColourVBO", FCvertices, sizeof(FCvertices), tempSetupShader->getBufferLayout()));
	tempSetupVAO->setIndexBuffer(m_pResources->addIndexBuffer("FCCubeIndices", indices, sizeof(indices) / sizeof(unsigned int)));
	// Create the flat colour material
	std::shared_ptr<Engine::Material> FCcubeMat = m_pResources->addMaterial("flatColourMat", tempSetupShader, tempSetupVAO);

	m_materials.push_back(std::make_shared<Engine::MaterialComponent>(Engine::MaterialComponent(FCcubeMat)));
	m_positions.push_back(std::make_shared<Engine::PositionComponent>(Engine::PositionComponent(
		glm::vec3(1.5f, 0.f, 3.f), glm::vec3(0.f), glm::vec3(1.f))));
	m_velocities.push_back(std::make_shared<Engine::VelocityComponent>(Engine::VelocityComponent(
		glm::vec3(0.f), glm::vec3(0.f, 20.f, 0.f))));
	m_oscilate.push_back(std::make_shared<Engine::OscilateComponent>(Engine::OscilateComponent(
		Engine::OscilateComponent::DOWN, 10.f)));

	m_gameObjects.push_back(std::make_shared<Engine::GameObject>());
	m_gameObjects.back()->addComponent(m_materials.back());
	m_gameObjects.back()->addComponent(m_positions.back());
	m_gameObjects.back()->addComponent(m_velocities.back());
	m_gameObjects.back()->addComponent(m_oscilate.back());

	//////////////////////////////////////////////////////////
	// Added textured phong shader and cube //////////////////
	//////////////////////////////////////////////////////////

	float TPvertices[8 * 24] = {
		-0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f, 0.33f, 0.5f,
		 0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f, 0.f, 0.5f,
		 0.5f,  0.5f, -0.5f, 0.f, 0.f, -1.f, 0.f, 0.f,
		-0.5f,  0.5f, -0.5f, 0.f, 0.f, -1.f, 0.33f, 0.f,
		-0.5f, -0.5f, 0.5f,  0.f, 0.f, 1.f, 0.33f, 0.5f,
		 0.5f, -0.5f, 0.5f,  0.f, 0.f, 1.f, 0.66f, 0.5f,
		 0.5f,  0.5f, 0.5f,  0.f, 0.f, 1.f, 0.66f, 0.f,
		-0.5f,  0.5f, 0.5f,  0.f, 0.f, 1.f, 0.33, 0.f,
		-0.5f, -0.5f, -0.5f, 0.f, -1.f, 0.f, 1.f, 0.f,
		 0.5f, -0.5f, -0.5f, 0.f, -1.f, 0.f, 0.66f, 0.f,
		 0.5f, -0.5f, 0.5f,  0.f, -1.f, 0.f, 0.66f, 0.5f,
		-0.5f, -0.5f, 0.5f,  0.f, -1.f, 0.f, 1.0f, 0.5f,
		-0.5f, 0.5f, -0.5f,  0.f, 1.f, 0.f, 0.33f, 1.0f,
		 0.5f, 0.5f, -0.5f,  0.f, 1.f, 0.f, 0.f, 1.0f,
		 0.5f, 0.5f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.5f,
		-0.5f, 0.5f, 0.5f,   0.f, 1.f, 0.f, 0.3f, 0.5f,
		-0.5f, -0.5f, -0.5f, -1.f, 0.f, 0.f, 0.33f, 1.0f,
		-0.5f,  0.5f, -0.5f, -1.f, 0.f, 0.f, 0.33f, 0.5f,
		-0.5f,  0.5f, 0.5f,  -1.f, 0.f, 0.f, 0.66f, 0.5f,
		-0.5f,  -0.5f, 0.5f, -1.f, 0.f, 0.f, 0.66f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.f, 0.f, 0.f, 1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.f, 0.f, 0.f, 1.0f, 0.5f,
		0.5f,  0.5f, 0.5f, 1.f, 0.f, 0.f,  0.66f, 0.5f,
		0.5f,  -0.5f, 0.5f,  1.f, 0.f, 0.f, 0.66f, 1.0f
	};
	// Same as flat colour stuff
	tempSetupShader = m_pResources->addShader("assets/shaders/texturedPhong2.glsl");
	tempSetupVAO = m_pResources->addVAO("texturedPhongCube");
	tempSetupVAO->setVertexBuffer(m_pResources->addVBO("texturedPhongVBO", TPvertices, sizeof(TPvertices), tempSetupShader->getBufferLayout()));
	tempSetupVAO->setIndexBuffer(m_pResources->addIndexBuffer("TPCubeIndices", indices, sizeof(indices) / sizeof(unsigned int)));
	// Create the textures phong material
	std::shared_ptr<Engine::Material> TPcubeMat = m_pResources->addMaterial("texturedPhongMat", tempSetupShader, tempSetupVAO);

	std::shared_ptr<Engine::Texture> numberTex = m_pResources->addTexture("assets/textures/numberCube.png");
	std::shared_ptr<Engine::Texture> letterTex = m_pResources->addTexture("assets/textures/letterCube.png");

	m_materials.push_back(std::make_shared<Engine::MaterialComponent>(Engine::MaterialComponent(TPcubeMat)));
	m_positions.push_back(std::make_shared<Engine::PositionComponent>(Engine::PositionComponent(
		glm::vec3(-1.5f, 0.f, 3.f), glm::vec3(0.f), glm::vec3(1.f))));
	m_velocities.push_back(std::make_shared<Engine::VelocityComponent>(Engine::VelocityComponent(
		glm::vec3(0.f), glm::vec3(0.f, -20.f, 0.f))));
	m_textureSwitch.push_back(std::make_shared<Engine::TextureSwitchComponent>(Engine::TextureSwitchComponent(
		numberTex, letterTex)));
	m_oscilate.push_back(std::make_shared<Engine::OscilateComponent>(Engine::OscilateComponent(
		Engine::OscilateComponent::UP, 10.f)));

	m_gameObjects.push_back(std::make_shared<Engine::GameObject>());
	m_gameObjects.back()->addComponent(m_materials.back());
	m_gameObjects.back()->addComponent(m_positions.back());
	m_gameObjects.back()->addComponent(m_velocities.back());
	m_gameObjects.back()->addComponent(m_textureSwitch.back());
	m_gameObjects.back()->addComponent(m_oscilate.back());

	std::shared_ptr<Engine::Material> controlCubeMat = m_pResources->addMaterial("controlCube", tempSetupShader, tempSetupVAO);

	m_materials.push_back(std::make_shared<Engine::MaterialComponent>(Engine::MaterialComponent(controlCubeMat)));
	m_positions.push_back(std::make_shared<Engine::PositionComponent>(Engine::PositionComponent(
		glm::vec3(-4.5f, -0.6f, 4.f), glm::vec3(0.f), glm::vec3(0.8f))));
	m_velocities.push_back(std::make_shared<Engine::VelocityComponent>(Engine::VelocityComponent(
		glm::vec3(0.f), glm::vec3(0.f))));
	m_rotation.push_back(std::make_shared<Engine::RotateComponent>(Engine::RotateComponent()));
	//m_textures.push_back(std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(numberTex)));

	m_gameObjects.push_back(std::make_shared<Engine::GameObject>());
	m_gameObjects.back()->addComponent(m_materials.back());
	m_gameObjects.back()->addComponent(m_positions.back());
	m_gameObjects.back()->addComponent(m_velocities.back());
	m_gameObjects.back()->addComponent(m_rotation.back());
	m_gameObjects.back()->addComponent(m_textures.back());

	m_MatUBO = m_pResources->getUBO("Matrices");
	m_LightUBO = m_pResources->getUBO("Light");

	m_pCamera.reset(new Engine::FPSCameraControllerEuler(false));
	m_pCamera->init(45.f, 4.f / 3.f, 0.1f, 100.f);
}

void GameLayer::onDetach()
{

}

void GameLayer::onUpdate(float timestep)
{
	m_pCamera->onUpdate(timestep);

	for (auto& CGO : m_gameObjects)
		CGO->onUpdate(timestep);

	/////////////////////////////////////
	// Set scene data ///////////////////
	/////////////////////////////////////
	
	// Data for light uniform block
	m_lightColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_lightPosition = glm::vec4(1.0f, 4.0f, -6.0f, 1.0f);
	m_viewPosition = glm::vec4(m_pCamera->getCamera()->getPosition(), 1.0f);
	
	// Two vectors of data for Matrices uniform block and Light uniform block
	std::vector<void*> tempData[2];
	// Add Matrices data to vector
	tempData[0].push_back((void*)&m_pCamera->getCamera()->getViewProjection()[0][0]);
	
	// Add Light data to vector
	tempData[1].push_back((void*)&m_lightColour[0]);
	tempData[1].push_back((void*)&m_lightPosition[0]);
	tempData[1].push_back((void*)&m_viewPosition[0]);

	m_sceneData.insert(std::make_pair(m_MatUBO, tempData[0]));
	m_sceneData.insert(std::make_pair(m_LightUBO, tempData[1]));

	m_pRenderer->actionCommand(Engine::RenderCommand::setClearColourCommand(0, 0, 0, 1));
	m_pRenderer->actionCommand(Engine::RenderCommand::setDepthTestLessCommand(true));
	m_pRenderer->actionCommand(Engine::RenderCommand::setBackfaceCullingCommand(true));
	m_pRenderer->actionCommand(Engine::RenderCommand::clearDepthColourBufferCommand());

	// Begin the scene
	m_pRenderer->beginScene(m_sceneData);

	for (auto& mat : m_materials)
		m_pRenderer->submit(mat->getMaterial());

	m_pRenderer->actionCommand(Engine::RenderCommand::setDepthTestLessCommand(false));
	m_pRenderer->actionCommand(Engine::RenderCommand::setBackfaceCullingCommand(false));
}

void GameLayer::onEvent(Engine::Event& event)
{
	m_pCamera->onEvent(event);

	for (auto& CGO : m_gameObjects)
		CGO->onEvent(event);
}
