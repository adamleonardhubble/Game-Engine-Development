/** \file JSONLayer.cpp
*/

#include "engine_pch.h"

#include "load/JSONLayer.h"
#include "load/JSONLoader.h"

#include "core/codes.h"

namespace Engine
{
	std::shared_ptr<CameraComponent> JSONLayer::s_currentCamera = nullptr;
	std::shared_ptr<CameraComponent> JSONLayer::s_current2DCamera = nullptr;

	std::shared_ptr<FrameBuffer> JSONLayer::s_pFrameBuffer = nullptr;
	std::shared_ptr<PPRenderer> JSONLayer::s_pPPRenderer = nullptr;
               
	std::shared_ptr<Texture> JSONLayer::s_pFBNightVisionNoise = nullptr;	    
	std::shared_ptr<Texture> JSONLayer::s_pFBNightVisionMask = nullptr;	    

	std::shared_ptr<Shader> JSONLayer::s_pFBShaderDefault = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderInversed = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderGrayscale = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderSharpen = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderBlur = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderEdgeDetection = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderNightVision = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderMetalPalace = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderDepth = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderShakeConfuseChaos = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderSimulatedFog = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderCartoon = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderDepthOfField = nullptr;

	std::shared_ptr<Shader> JSONLayer::s_pFBShaderShadowDepthMap = nullptr;
	std::shared_ptr<Shader> JSONLayer::s_pFBShaderShadowLightSpace = nullptr;

	std::shared_ptr<Shader> JSONLayer::s_pFBShaderTest = nullptr;

	bool JSONLayer::s_bDraw = true;

	std::shared_ptr<LightComponent> JSONLayer::s_pLight = nullptr;

	void JSONLayer::setupFrameBuffer()
	{
		s_pFBNightVisionNoise = ResourceManager::addTexture("assets/textures/FBOShader/noise_texture_1.png"); // Add texture for frame buffers (night vision)
		s_pFBNightVisionMask = ResourceManager::addTexture("assets/textures/FBOShader/vignette_texture_2.png"); // Add texture for frame buffers (night vision)

		s_pFBShaderDefault = ResourceManager::addShader("assets/shaders/frameBufferDefault.glsl"); // Add shader for frame buffers (default)
		s_pFBShaderInversed = ResourceManager::addShader("assets/shaders/frameBufferInversed.glsl"); // Add shader for frame buffers (inversed colour)
		s_pFBShaderGrayscale = ResourceManager::addShader("assets/shaders/frameBufferGrayscale.glsl"); // Add shader for frame buffers (grayscale)
		s_pFBShaderSharpen = ResourceManager::addShader("assets/shaders/frameBufferSharpen.glsl"); // Add shader for frame buffers (sharpen)
		s_pFBShaderBlur = ResourceManager::addShader("assets/shaders/frameBufferBlur.glsl"); // Add shader for frame buffers (blur)
		s_pFBShaderEdgeDetection = ResourceManager::addShader("assets/shaders/frameBufferEdgeDetection.glsl"); // Add shader for frame buffers (edge detection)
		s_pFBShaderNightVision = ResourceManager::addShader("assets/shaders/frameBufferNightVision.glsl"); // Add shader for frame buffers (night vision)
		s_pFBShaderMetalPalace = ResourceManager::addShader("assets/shaders/frameBufferMetalPalace.glsl"); // Add shader for frame buffers (metal palace)
		s_pFBShaderDepth = ResourceManager::addShader("assets/shaders/frameBufferDepth.glsl"); // Add shader for frame buffers (depth)
		s_pFBShaderShakeConfuseChaos = ResourceManager::addShader("assets/shaders/frameBufferShakeConfuseChaos.glsl"); // Add shader for frame buffers (shake, confuse and chaos)
		s_pFBShaderSimulatedFog = ResourceManager::addShader("assets/shaders/frameBufferSimulatedFog.glsl"); // Add shader for frame buffers (simulated fog)
		s_pFBShaderCartoon = ResourceManager::addShader("assets/shaders/frameBufferCartoon.glsl"); // Add shader for frame buffers (cartoon)
		s_pFBShaderDepthOfField = ResourceManager::addShader("assets/shaders/frameBufferDepthOfField.glsl"); // Add shader for frame buffers (DoF)

		s_pFBShaderShadowDepthMap = ResourceManager::addShader("assets/shaders/frameBufferShadowDepthMap.glsl"); // Add shader for frame buffers (shadow depth map)
		s_pFBShaderShadowLightSpace = ResourceManager::addShader("assets/shaders/frameBufferShadowLightSpace.glsl"); // Add shader for frame buffers (shadow light space)

		s_pFBShaderTest = ResourceManager::addShader("assets/shaders/frameBufferTest.glsl"); // Add shader for frame buffers (test)

		s_pPPRenderer.reset(PPRenderer::createPPRenderer(s_pFBShaderDefault)); // Initialise post-processing renderer object
		s_pFrameBuffer.reset(FrameBuffer::create()); // Intialise frame buffer object
	}

	void JSONLayer::onAttach()
	{
		// Load the layer
		JSONLoader::load(m_filepath, *this);

		// Call all init render commands
		for (auto& renderCommand : m_initCommands)
			m_pRenderer->actionCommand(renderCommand.get());

		m_objectUpdateThread = ObjectUpdateThread::getInstance();

		if (m_lights.size() != 0)
			s_pLight = m_lights.back();
	}

	void JSONLayer::onDetach()
	{
		// Call exit render commands
		for (auto& renderCommand : m_exitCommands) m_pRenderer->actionCommand(renderCommand.get());

		for (auto& ptr : m_data)
		{
			delete ptr;
		}
	}

	void JSONLayer::onUpdate(float timestep)
	{
		m_fTimestep = timestep;	// Store the timestep accumulated for the current frame passing

		// If there is a player component
		if (m_player != nullptr)
		{
			// Get the cameras
			auto cameras = m_player->getCameras();
			cameras.first->onUpdate(timestep);
			// If there is a second camera
			if (cameras.second != nullptr)
				cameras.second->onUpdate(timestep);
		}
		else if (m_pCamera != nullptr)
			m_pCamera->onUpdate(timestep);

		if (m_pCamera != nullptr)
		{
			// Set camera variables used for uniform buffers
			m_camView = m_pCamera->getCamera()->getView();
			m_camProj = m_pCamera->getCamera()->getProjection();
			m_camViewProj = m_pCamera->getCamera()->getViewProjection();
			m_camPos = m_pCamera->getCamera()->getPosition();
		}
		else // If the camera being used is a camera component
		{
			if (m_bIs3DLayer)
			{
				// Set camera variables used for uniform buffers
				m_camView = s_currentCamera->getCamera()->getView();
				m_camProj = s_currentCamera->getCamera()->getProjection();
				m_camViewProj = s_currentCamera->getCamera()->getViewProjection();
				m_camPos = s_currentCamera->getCamera()->getPosition();
			}
			else
			{
				// Set camera variables used for uniform buffers
				m_camView = s_current2DCamera->getCamera()->getView();
				m_camProj = s_current2DCamera->getCamera()->getProjection();
				m_camViewProj = s_current2DCamera->getCamera()->getViewProjection();
				m_camPos = s_current2DCamera->getCamera()->getPosition();
			}
		}
		// To spread object updates between the separate (or main, depending on if a separate thread has been made) thread and the physics update one
		bool separate = true;
		// Update all objects
		for (auto& CGO : m_gameObjects)
		{
			// Check for a LuaScriptComponent
			auto script = CGO->getComponent<LuaScriptComponent<DynamicPhysicsSystem>>();
			// If there is one
			if (script != CGO->end())
			{
				// Update on the main thread
				CGO->onUpdate(timestep);
			}
			else // If no LuaScriptComponent
			{
				// If the object update is going on the separate (or main) thread, or if there is no physics, or if the physics is updated on the main thread
				if (separate || !DynamicPhysicsSystem::systemExists() || !DynamicPhysicsSystem::separateThread())
				{
					// If using a separate update thread
					if (m_objectUpdateThread->separateThreadForObjectUpdates())
					{
						// Add the object to the vector for the thread to update them
						m_objectUpdateThread->addObject(CGO, timestep);
					}
					else // If not using separate threads
					{
						// Update on the main thread
						CGO->onUpdate(timestep);
					}
					// If spreading updates between the physics thread and others, to give the next object update to the physics thread
					separate = false;
				}
				else // If the next object update is going to the physics thread
				{
					// Add the object to the vector for the physics thread thread to update them when not updating the physics
					m_physicsSystem->addObjectToUpdate(CGO, timestep);

					separate = true;
				}
			}
		}

		if (m_bIs3DLayer)
		{
			if (s_currentCamera != nullptr)
			{
				std::vector<std::string> layers = s_currentCamera->getLayersSeen();

				s_pFrameBuffer->shadowBind(); // Bind the shadow map frame buffer

					// Call predraw render commands
				for (auto& renderCommand : m_predrawFramebufferCommands)
					m_pRenderer->actionCommand(renderCommand.get());

				// Only render layer objects if the layer name is in the list on the camera
				if (std::count(layers.begin(), layers.end(), m_name))
				{
					glm::mat4 lightProjection; // Light projection matrix
					glm::mat4 lightView; // Light view matrix

					auto lightObject = *s_pLight->getObject()->getComponent<PositionComponent>(); // Store the light component used by the scene
					std::shared_ptr<PositionComponent> lightPositionComponent = std::dynamic_pointer_cast<PositionComponent>(lightObject); // Cast the light object (dynamic)
					glm::vec3 lightObjectRotation = lightPositionComponent->getRotation(); // Store the light objects rotation vector
					glm::vec3 lightObjectPosition = lightPositionComponent->getPosition(); // Store the light objects position vector

					lightProjection = glm::ortho<float>(-20.0f, 20.0f, -20.0f, 20.0f, m_fNearClippingPlane, m_fFarClippingPlane); // Create a light projection matrix (orthogrpahic projection)
					lightView = glm::lookAt(lightObjectPosition, lightObjectRotation, glm::vec3(0.0f, 1.0f, 0.0f)); // Create a viewpoint representing the light objects position and facing direction
					m_lightSpaceMatrix = lightProjection * lightView; // Calculate the light space matrix

					for (auto& mat : m_materials)
					{
						std::pair<std::string, void*> lightSpace("u_lightSpaceMatrix", (void*)&m_lightSpaceMatrix[0][0]); // Store the light space matrix data
						ComponentMessage matrixMessage(ComponentMessageType::UniformSet, (void*)&lightSpace); // Submit the matrix uniform as a component message
						mat->receiveMessage(matrixMessage);	// Apply the recieved uniform data to the material

						std::shared_ptr<Shader> objectShader = mat->getMaterial()->getShader();	// Store the shader currently used by the material
						mat->getMaterial()->setShader(s_pFBShaderShadowLightSpace);	// Set the shader used by the material to the light space shader

						m_pRenderer->submit(mat->getMaterial()); // Draw geometry to texture (off-screen rendering)
						mat->getMaterial()->setShader(objectShader); // Set the shader used by the material to the default shader
					}
				}

				// Call postdraw render commands
				for (auto& renderCommand : m_postdrawFramebufferCommands)
					m_pRenderer->actionCommand(renderCommand.get());

				s_pFrameBuffer->shadowUnbind();	// Unbind the shadow map frame buffer
			}
			else
			{
				/*
				s_pFrameBuffer->bind();	// Bind the frame buffer

				// Call predraw render commands
				for (auto& renderCommand : m_predrawFramebufferCommands)
					m_pRenderer->actionCommand(renderCommand.get());

				m_pRenderer->beginScene(m_sceneData); // Submit uniform variable data

				for (auto& mat : m_materials)
					m_pRenderer->submit(mat->getMaterial()); // Draw geometry to texture (off-screen rendering)

				// Call postdraw render commands
				for (auto& renderCommand : m_postdrawFramebufferCommands)
					m_pRenderer->actionCommand(renderCommand.get());

				s_pFrameBuffer->unbind(); // Unbind the frame buffer
				*/

				glm::mat4 lightProjection; // Light projection matrix
				glm::mat4 lightView; // Light view matrix

				auto lightObject = *s_pLight->getObject()->getComponent<PositionComponent>(); // Store the light component used by the scene
				std::shared_ptr<PositionComponent> lightPositionComponent = std::dynamic_pointer_cast<PositionComponent>(lightObject); // Cast the light object (dynamic)
				glm::vec3 lightObjectRotation = lightPositionComponent->getRotation(); // Store the light objects rotation vector
				glm::vec3 lightObjectPosition = lightPositionComponent->getPosition(); // Store the light objects position vector

				lightProjection = glm::ortho<float>(-20.0f, 20.0f, -20.0f, 20.0f, m_fNearClippingPlane, m_fFarClippingPlane); // Create a light projection matrix (orthogrpahic projection)
				lightView = glm::lookAt(lightObjectPosition, lightObjectRotation, glm::vec3(0.0f, 1.0f, 0.0f)); // Create a viewpoint representing the light objects position and facing direction
				m_lightSpaceMatrix = lightProjection * lightView; // Calculate the light space matrix

				s_pFrameBuffer->shadowBind(); // Bind the shadow map frame buffer

				// Call predraw render commands
				for (auto& renderCommand : m_predrawFramebufferCommands)
					m_pRenderer->actionCommand(renderCommand.get());

				for (auto& mat : m_materials)
				{
					std::pair<std::string, void*> lightSpace("u_lightSpaceMatrix", (void*)&m_lightSpaceMatrix[0][0]); // Store the light space matrix data
					ComponentMessage matrixMessage(ComponentMessageType::UniformSet, (void*)&lightSpace); // Submit the matrix uniform as a component message
					mat->receiveMessage(matrixMessage);	// Apply the recieved uniform data to the material

					std::shared_ptr<Shader> objectShader = mat->getMaterial()->getShader();	// Store the shader currently used by the material
					mat->getMaterial()->setShader(s_pFBShaderShadowLightSpace);	// Set the shader used by the material to the light space shader

					m_pRenderer->submit(mat->getMaterial()); // Draw geometry to texture (off-screen rendering)
					mat->getMaterial()->setShader(objectShader); // Set the shader used by the material to the default shader
				}

				// Call postdraw render commands
				for (auto& renderCommand : m_postdrawFramebufferCommands)
					m_pRenderer->actionCommand(renderCommand.get());

				s_pFrameBuffer->shadowUnbind();	// Unbind the shadow map frame buffer
			}
		}
		
		s_bDraw = true;
	}

	void JSONLayer::drawObjects()
	{
		std::shared_ptr<CameraComponent> camera;
		if (m_bIs3DLayer)
			camera = s_currentCamera;
		else
			camera = s_current2DCamera;

		if (camera != nullptr)
		{
			std::vector<std::string> layers = camera->getLayersSeen();

			s_pFrameBuffer->defaultBind(); // Bind the default frame buffer

				// Call predraw render commands
			for (auto& renderCommand : m_predrawFramebufferCommands)
				m_pRenderer->actionCommand(renderCommand.get());

			m_pRenderer->beginScene(m_sceneData); // Submit uniform variable data

			// Only render layer objects if the layer name is in the list on the camera
			if (std::count(layers.begin(), layers.end(), m_name))
			{
				unsigned int depthMapTextureSlot = s_pFrameBuffer->getDepthMapTextureSlot(); // Store the depth texture slot of the depth map texture

				for (auto& mat : m_materials)
				{
					if (m_bIs3DLayer)
					{
						std::pair<std::string, void*> depthMap("u_shadowDepth", (void*)&depthMapTextureSlot); // Store the depth map texture slot data
						ComponentMessage textureMessage(ComponentMessageType::UniformSet, (void*)&depthMap); // Submit the texture uniform as a component message
						mat->receiveMessage(textureMessage); // Apply the recieved uniform data to the material

						std::pair<std::string, void*> lightSpace("u_lightSpaceMatrix", (void*)&m_lightSpaceMatrix[0][0]); // Store the light space matrix data
						ComponentMessage matrixMessage(ComponentMessageType::UniformSet, (void*)&lightSpace); // Submit the matrix uniform as a component message
						mat->receiveMessage(matrixMessage); // Apply the recieved uniform data to the material
					}

					m_pRenderer->submit(mat->getMaterial()); // Draw geometry to texture (off-screen rendering)
				}
			}

			// Call postdraw render commands
			for (auto& renderCommand : m_postdrawFramebufferCommands)
				m_pRenderer->actionCommand(renderCommand.get());

			s_pFrameBuffer->defaultUnbind(); // Unbind the default frame buffer
		}
		else
		{
			/*
			s_pFrameBuffer->bind();	// Bind the frame buffer

			// Call predraw render commands
			for (auto& renderCommand : m_predrawFramebufferCommands)
				m_pRenderer->actionCommand(renderCommand.get());

			m_pRenderer->beginScene(m_sceneData); // Submit uniform variable data

			for (auto& mat : m_materials)
				m_pRenderer->submit(mat->getMaterial()); // Draw geometry to texture (off-screen rendering)

			// Call postdraw render commands
			for (auto& renderCommand : m_postdrawFramebufferCommands)
				m_pRenderer->actionCommand(renderCommand.get());

			s_pFrameBuffer->unbind(); // Unbind the frame buffer
			*/
		
			s_pFrameBuffer->defaultBind(); // Bind the default frame buffer

				// Call predraw render commands
			for (auto& renderCommand : m_predrawFramebufferCommands)
				m_pRenderer->actionCommand(renderCommand.get());

			m_pRenderer->beginScene(m_sceneData); // Submit uniform variable data

			unsigned int depthMapTextureSlot = s_pFrameBuffer->getDepthMapTextureSlot(); // Store the depth texture slot of the depth map texture

			for (auto& mat : m_materials)
			{
				if (m_bIs3DLayer)
				{
					std::pair<std::string, void*> depthMap("u_shadowDepth", (void*)&depthMapTextureSlot); // Store the depth map texture slot data
					ComponentMessage textureMessage(ComponentMessageType::UniformSet, (void*)&depthMap); // Submit the texture uniform as a component message
					mat->receiveMessage(textureMessage); // Apply the recieved uniform data to the material

					std::pair<std::string, void*> lightSpace("u_lightSpaceMatrix", (void*)&m_lightSpaceMatrix[0][0]); // Store the light space matrix data
					ComponentMessage matrixMessage(ComponentMessageType::UniformSet, (void*)&lightSpace); // Submit the matrix uniform as a component message
					mat->receiveMessage(matrixMessage); // Apply the recieved uniform data to the material
				}

				m_pRenderer->submit(mat->getMaterial()); // Draw geometry to texture (off-screen rendering)
			}

			// Call postdraw render commands
			for (auto& renderCommand : m_postdrawFramebufferCommands)
				m_pRenderer->actionCommand(renderCommand.get());

			s_pFrameBuffer->defaultUnbind(); // Unbind the default frame buffer
		}
	}

	void JSONLayer::drawLayer()
	{
		if (s_bDraw)
		{
			#pragma region Shadow Mapping
			unsigned int depthMapTextureSlot = s_pFrameBuffer->getDepthMapTextureSlot(); // Store the depth texture slot of the depth map texture

			s_pPPRenderer->addPPUniform("u_depthMapTexture", (void*)&depthMapTextureSlot); // Add the depth map texture uniform variable to the post-processing uniform map
			#pragma endregion

			// Call predraw render commands
			for (auto& renderCommand : m_predrawLayerTextureCommands)
				m_pRenderer->actionCommand(renderCommand.get());

			unsigned int colourTextureSlotPrimary = s_pFrameBuffer->getColourTextureSlot(0); // Store the colour texture slot of the primary colour texture
			unsigned int colourTextureSlotSecondary = s_pFrameBuffer->getColourTextureSlot(1); // Store the colour texture slot of the secondary colour texture
			unsigned int colourTextureSlotTertiary = s_pFrameBuffer->getColourTextureSlot(2); // Store the colour texture slot of the tertiary colour texture
			unsigned int colourTextureSlotQuaternary = s_pFrameBuffer->getColourTextureSlot(3); // Store the colour texture slot of the quaternary colour texture

			s_pPPRenderer->addPPUniform("u_colourTexturePrimary", (void*)&colourTextureSlotPrimary); // Add the primary colour texture uniform variable to the post-processing uniform map
			s_pPPRenderer->addPPUniform("u_colourTextureSlotSecondary", (void*)&colourTextureSlotSecondary); // Add the secondary colour texture uniform variable to the post-processing uniform map
			s_pPPRenderer->addPPUniform("u_colourTextureSlotTertiary", (void*)&colourTextureSlotTertiary); // Add the tertiary colour texture uniform variable to the post-processing uniform map
			s_pPPRenderer->addPPUniform("u_colourTextureSlotQuaternary", (void*)&colourTextureSlotQuaternary); // Add the quaternary colour texture uniform variable to the post-processing uniform map

			unsigned int depthTextureSlot = s_pFrameBuffer->getDepthTextureSlot(); // Store the depth texture slot of the depth texture

			s_pPPRenderer->addPPUniform("u_depthTexture", (void*)&depthTextureSlot); // Add the depth texture uniform variable to the post-processing uniform map

			#pragma region Sharpen
			s_pPPRenderer->addPPUniform("u_activeSharpen", (void*)&m_iActiveSharpen); // Add the sharpen uniform variable to the post-processing uniform map

			//LOG_CRITICAL("BLUR: {0}", m_iActiveSharpen); // Output the active sharpen counter
			#pragma endregion

			#pragma region Blur
			s_pPPRenderer->addPPUniform("u_activeBlur", (void*)&m_iActiveBlur); // Add the blur uniform variable to the post-processing uniform map

			//LOG_CRITICAL("BLUR: {0}", m_iActiveBlur); // Output the active blur counter
			#pragma endregion

			#pragma region Edge Detection
			glm::vec2 screenResolution = s_pFrameBuffer->getScreenResolution(); // Vignette resolution

			s_pPPRenderer->addPPUniform("u_screenResolution", (void*)&screenResolution[0]); // Add the screen resolution uniform variable to the post-processing uniform map
			s_pPPRenderer->addPPUniform("u_activeEdgeDetection", (void*)&m_iActiveEdgeDetection); // Add the edge detection uniform variable to the post-processing uniform map
			
			//LOG_CRITICAL("EDGE DETECTION: {0}", m_iActiveEdgeDetection); // Output the active edge detection counter
			#pragma endregion

			#pragma region Metal Palace
			s_pPPRenderer->addPPUniform("u_activeMetal", (void*)&m_iActiveMetal); // Add the metal palace uniform variable to the post-processing uniform map

			//LOG_CRITICAL("METAL PALACE: {0}", m_iActiveMetal); // Output the active metal palace counter
			#pragma endregion

			#pragma region Night Vision
			m_fNightVisionTimestep += m_fTimestep; // Add and equal the time passed since the last frame was made

			unsigned int noiseTexture = s_pFBNightVisionNoise->getSlot(); // Store the noise texture slot of the night vision shader
			unsigned int maskTexture = s_pFBNightVisionMask->getSlot(); // Store the mask texture slot of the night vision shader

			s_pPPRenderer->addPPUniform("u_noiseTexture", (void*)&noiseTexture); // Add the noise texture uniform variable to the post-processing uniform map
			s_pPPRenderer->addPPUniform("u_maskTexture", (void*)&maskTexture); // Add the mask texture uniform variable to the post-processing uniform map
			s_pPPRenderer->addPPUniform("u_timestepNV", (void*)&m_fNightVisionTimestep); // Add the timestep uniform variable to the post-processing uniform map																		   
			s_pPPRenderer->addPPUniform("u_effectCoverage", (void*)&m_fEffectCoverage); // Add the effect coverage uniform variable to the post-processing uniform map
			
			//LOG_CRITICAL("NIGHT VISION: {0}", m_iEffectCoverage); // Output the effect coverage amount counter
			#pragma endregion

			#pragma region Chaos/ Confuse/ Shake
			m_fShakeTimestep += m_fTimestep; // Add and equal the time passed since the last frame was made

			bool chaosEffectActive;	// Determine whether the chaos effect is active
			bool confuseEffectActive; // Determine whether the confuse effect is active
			bool shakeEffectActive;	// Determine whether the shake effect is active

			if ((int)m_fShakeTimestep % 2 == 0)	// If the accumulated time passed is an even number, do the following
			{
				chaosEffectActive = true; // Enable the chaos effect
				confuseEffectActive = false; // Disable the confuse effect
			}
			else // Else if the accumulated time passed is an odd number, do the following
			{
				chaosEffectActive = false; // Disable the chaos effect
				confuseEffectActive = true;	// Enable the confuse effect
			}

			s_pPPRenderer->addPPUniform("u_chaos", (void*)&chaosEffectActive); // Add the chaos uniform variable to the post-processing uniform map
			s_pPPRenderer->addPPUniform("u_confuse", (void*)&confuseEffectActive); // Add confuse chaos uniform variable to the post-processing uniform map
			s_pPPRenderer->addPPUniform("u_shake", (void*)&shakeEffectActive); // Add the shake uniform variable to the post-processing uniform map
			s_pPPRenderer->addPPUniform("u_timestepSCC", (void*)&m_fShakeTimestep); // Add the timestep uniform variable to the post-processing uniform map
			#pragma endregion

			#pragma region Cartoon
			s_pPPRenderer->addPPUniform("u_cartoonRate", (void*)&m_fCartoonRate); // Add the cartoon rate uniform variable to the post-processing uniform map

			//LOG_CRITICAL("CARTOON: {0}", m_fCartoonRate); // Output the cartoon rate counter
			#pragma endregion

			#pragma region Depth of Field
			s_pPPRenderer->addPPUniform("u_focalDepth", (void*)&m_fFocalDepth); // Add the focal depth uniform variable to the post-processing uniform map

			//LOG_CRITICAL("DEPTH OF FIELD: {0}", m_fFocalDepth); // Output the focal depth counter
			#pragma endregion

			s_pPPRenderer->submit(); // Submit the texture, draw the quad

			// Call postdraw render commands
			for (auto& renderCommand : m_postdrawLayerTextureCommands)
				m_pRenderer->actionCommand(renderCommand.get());

			s_bDraw = false;
		}
	}

	void JSONLayer::onEvent(Event & event)
	{
		// If there is a player component
		if (m_player != nullptr)
		{
			// Get the cameras
			auto cameras = m_player->getCameras();
			// update camera(s)
			cameras.first->onEvent(event);
			// If there is a second camera
			if (cameras.second != nullptr)
				cameras.second->onEvent(event);
		}
		else if (m_pCamera != nullptr)
			m_pCamera->onEvent(event);

		for (auto& CGO : m_gameObjects) CGO->onEvent(event);

		s_pFrameBuffer->onEvent(event); // Dispatch frame buffer related events

		// Create event dispatcher
		EventDispatcher dispatcher(event);
		// If the Event type matches, call the corresponding function
		dispatcher.dispatch<KeyPressedEvent>(std::bind(&JSONLayer::onKeyPressed, this, std::placeholders::_1));
	}

	bool JSONLayer::onKeyPressed(KeyPressedEvent& e)
	{
		// If there is a player object in the layer
		if (m_player != nullptr)
		{
			if (e.getKeyCode() == ENGINE_KEY_RIGHT_SHIFT) // If the right shift key is pressed, do the following
			{
				m_pCamera = m_player->getCurrentCamera(); // Set the  camera to the current player camera
				return true;
			}
		}

		if (e.getKeyCode() == ENGINE_KEY_F)	// If the letter f key is pressed, do the following
		{
			if (m_iShaderToggle != 14) // If the shader currently applied is not the last shader, do the following
			{
				m_iShaderToggle++; // Increment the toggle count
			}
			else // Else if the shader currently applied is the last shader, do the following
			{
				m_iShaderToggle = 0; // Reset the toggle count
			}

			if (m_iShaderToggle == 0) // If the current toggle count is '0', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderDefault); // Set the frame buffer texture to the default shader
			}
			else if (m_iShaderToggle == 1) // If the current toggle count is '1', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderInversed); // Set the frame buffer texture to the inversed shader
			}
			else if (m_iShaderToggle == 2) // If the current toggle count is '2', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderGrayscale); // Set the frame buffer texture to the grayscale shader
			}
			else if (m_iShaderToggle == 3) // If the current toggle count is '3', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderSharpen); // Set the frame buffer texture to the sharpen shader
			}
			else if (m_iShaderToggle == 4) // If the current toggle count is '4', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderBlur); // Set the frame buffer texture to the blur shader
			}
			else if (m_iShaderToggle == 5) // If the current toggle count is '5', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderEdgeDetection); // Set the frame buffer texture to the edge detection shader
			}
			else if (m_iShaderToggle == 6) // If the current toggle count is '6', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderNightVision); // Set the frame buffer texture to the night vision shader
			}
			else if (m_iShaderToggle == 7) // If the current toggle count is '7', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderMetalPalace); // Set the frame buffer texture to the metal palace shader
			}
			else if (m_iShaderToggle == 8) // If the current toggle count is '8', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderDepth); // Set the frame buffer texture to the depth shader
			}
			else if (m_iShaderToggle == 9) // If the current toggle count is '9', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderShakeConfuseChaos); // Set the frame buffer texture to the shake, confuse and chaos shader
			}  
			else if (m_iShaderToggle == 10) // If the current toggle count is '10', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderSimulatedFog); // Set the frame buffer texture to the simulated fog shader
			}
			else if (m_iShaderToggle == 11) // If the current toggle count is '11', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderCartoon); // Set the frame buffer texture to the cartoon shader
			}
			else if (m_iShaderToggle == 12) // If the current toggle count is '12', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderDepthOfField); // Set the frame buffer texture to the DOF shader
			}
			else if (m_iShaderToggle == 13) // If the current toggle count is '13', do the following
			{
				s_pPPRenderer->setPPShader(s_pFBShaderShadowDepthMap); // Set the frame buffer texture to the depth map shader
			}
			else if (m_iShaderToggle == 14) // If the current toggle count is '14', do the following
			{  
				s_pPPRenderer->setPPShader(s_pFBShaderTest); // Set the frame buffer texture to the test shader
			}

			return false;
		}

		if (e.getKeyCode() == ENGINE_KEY_N)	// If the letter n key is pressed, do the following
		{																				 
			if (m_iShaderToggle == 3) // If the current toggle count is '4' (blur), do the following
			{
				if (m_iActiveSharpen != 2) // If the active sharpen kernel is not the last kernel in the cycle, do the following
				{																											  
					m_iActiveSharpen++; // Increment the active sharpen kernel counter
				}
				else // If the active sharpen kernel is the last kernel in the cycle, do the following
				{
					m_iActiveSharpen = 1; // Reset the active blur kernel counter
				}
			}
			else if (m_iShaderToggle == 4) // If the current toggle count is '4' (blur), do the following
			{
				if (m_iActiveBlur != 3) // If the active blur kernel is not the last kernel in the cycle, do the following
				{
					m_iActiveBlur++; // Increment the active blur kernel counter
				}
				else // If the active blur kernel is the last kernel in the cycle, do the following
				{
					m_iActiveBlur = 1; // Reset the active blur kernel counter
				}
			}
			else if (m_iShaderToggle == 5) // If the current toggle count is '5' (edge detection), do the following
			{
				if (m_iActiveEdgeDetection != 3) // If the active edge detection kernel is not the last kernel in the cycle, do the following
				{
					m_iActiveEdgeDetection++; // Increment the active edge detection kernel counter
				}
				else // If the active edge detection kernel is the last kernel in the cycle, do the following
				{
					m_iActiveEdgeDetection = 1;	// Reset the active edge detection kernel counter
				}
			}
			else if (m_iShaderToggle == 6) // If the current toggle count is '6' (night vision), do the following
			{
				if (m_fEffectCoverage != 1.0f) // If the effect coverage amount is not the last amount in the cycle, do the following
				{
					m_fEffectCoverage += 0.5f; // Increment the effect coverage counter
				}
				else // If the effect coverage amount is the last amount in the cycle, do the following
				{
					m_fEffectCoverage = 0.0f; // Reset the effect coverage counter
				}
			}
			else if (m_iShaderToggle == 7) // If the current toggle count is '7' (metal palace), do the following
			{
				if (m_iActiveMetal != 3) // If the active metal palace colour is not the last colour in the cycle, do the following
				{
					m_iActiveMetal++; // Increment the active metal palace colour counter
				}
				else // If the active metal palace colour is the last colour in the cycle, do the following
				{
					m_iActiveMetal = 1;	// Reset the active metal palace colour counter
				}
			}
			else if (m_iShaderToggle == 11) // If the current toggle count is '11' (cartoon), do the following
			{
				if (m_fCartoonRate != 3.0f) // If the cartoon rate is not the last rate in the cycle, do the following
				{
					m_fCartoonRate += 0.25f; // Increment the cartoon rate counter
				}
				else // If the cartoon rate is the last rate in the cycle, do the following
				{
					m_fCartoonRate = 0.0f; // Reset the cartoon rate counter
				}
			}
			else if (m_iShaderToggle == 12) // If the current toggle count is '12' (focal depth), do the following
			{
				m_bFarDoF = !m_bFarDoF;	// Toggle the DoF active (near or far)

				if (m_bFarDoF == true) // If the DoF is set as 'far', do the following
				{
					m_fFocalDepth = 2.0f; // Set the focal depth of the camera frustum to '2' (near)
				}
				else // Else if the DoF is set as 'near', do the following
				{
					m_fFocalDepth = 100.0f;	// Set the focal depth of the camera frustum to '100' (far)
				}
			}

			return false;
		}

		if (e.getKeyCode() == ENGINE_KEY_M) // If the letter m key is pressed, do the following
		{
			s_pFrameBuffer->toggleMSAA(); // Toggle the activeness of MSAA
			return true;
		}
		if (e.getKeyCode() == ENGINE_KEY_UP) // If the up arrow key is pressed, do the following
		{
			if (s_pFrameBuffer->getMSAA() == true) // If MSAA is enabled, do the following
			{
				s_pFrameBuffer->increaseMSAA();	// Increase the number of samples used by MSAA
				s_pFrameBuffer->updateSampleMSAA();	// Update the MSAA sample amount
			}

			return true;
		}
		if (e.getKeyCode() == ENGINE_KEY_DOWN) // If the down arrow key is pressed
		{
			if (s_pFrameBuffer->getMSAA() == true) // If MSAA is enabled, do the following
			{
				s_pFrameBuffer->decreaseMSAA();	// Decrease the number of samples used by MSAA
				s_pFrameBuffer->updateSampleMSAA();	// Update the MSAA sample amount
			}

			return true;
		}
		if (e.getKeyCode() == ENGINE_KEY_B) // If the letter b key is pressed
		{
			s_pFrameBuffer->toggleWireframe(); // Toggle wireframe mode
			return true;
		}
		
		return false;

	}
}