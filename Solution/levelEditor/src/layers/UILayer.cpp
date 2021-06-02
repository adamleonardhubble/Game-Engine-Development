/** \file UILayer.cpp
*/
#include <Engine.h>

#include "layers/UILayer.h"
#include "layers/load/loadEditor.h"

#include "makeShader.h"

#include <filesystem>
#include "json.hpp"

UILayer::~UILayer()
{
	std::string filepath;

	// Check for any running files, if there are any, remove them
	std::vector<std::string>::iterator it;
	for (it = m_displayLayer->m_layerData.m_3DLayers.begin(); it != m_displayLayer->m_layerData.m_3DLayers.end(); ++it)
	{
		filepath = "assets/savedJSON/" + (*it) + ".json";
		if (std::filesystem::exists(filepath))
			std::filesystem::remove(filepath.c_str());
	}
	for (it = m_displayLayer->m_layerData.m_2DLayers.begin(); it != m_displayLayer->m_layerData.m_2DLayers.end(); ++it)
	{
		filepath = "assets/savedJSON/" + (*it) + ".json";
		if (std::filesystem::exists(filepath))
			std::filesystem::remove(filepath.c_str());
	}
	
	filepath = "assets/savedJSON/TempControl.json";
	if (std::filesystem::exists(filepath))
		std::filesystem::remove(filepath.c_str());

	// Check for any temporary shaders, if there are any, remove them
	filepath = "assets/savedJSON/00.glsl";
	if (std::filesystem::exists(filepath))
		std::filesystem::remove(filepath.c_str());
	filepath = "assets/savedJSON/01.glsl";
	if (std::filesystem::exists(filepath))
		std::filesystem::remove(filepath.c_str());
	filepath = "assets/savedJSON/10.glsl";
	if (std::filesystem::exists(filepath))
		std::filesystem::remove(filepath.c_str());
	filepath = "assets/savedJSON/11.glsl";
	if (std::filesystem::exists(filepath))
		std::filesystem::remove(filepath.c_str());
}

void UILayer::onAttach()
{
	loadAssets(); // Load assets

	m_displayLayer->onAttach(); // Set up the display layer, load the level

	m_pRenderer.reset(Engine::Renderer::createText());

	for (auto& object : m_displayLayer->m_layerData.m_objects)
	{
		if (object.m_hasCamera)
		{
			m_iResetCamera = object.m_cameraComponentVectorValue;
			m_iLastCameraSelected = m_iResetCamera;
			break;
		}
	}
	for (auto& object : m_displayLayer->m_layerData.m_2DObjects)
	{
		if (object.m_hasCamera)
		{
			m_i2DCameraVectorValue = object.m_cameraComponentVectorValue;
			break;
		}
	}

	m_displayWindowMultiplier = m_displayLayer->m_displayWindowMultiplier; // Get the window size multiplier

	m_displayLayer->m_layerData.m_name = std::next(m_levels.begin(), m_iCurrentLevel)->first;
	m_displayLayer->m_layerData.m_filepath = std::next(m_levels.begin(), m_iCurrentLevel)->second;

	m_pFrameBuffer.reset(Engine::FrameBuffer::create());

	strcpy(m_newLevelName, "");
	strcpy(m_newTagName, "");

	m_pFrameBuffer->defaultBind();

	for (auto& renderCommand : m_displayLayer->getInitCommands())
		m_displayLayer->getRenderer()->actionCommand(renderCommand.get());

	m_pFrameBuffer->defaultUnbind();
}

void UILayer::onUpdate(float timestep)
{
	BeginFrame();
	ImGui::DockSpaceOverViewport(); // For docking ImGui windows

	// Window for viewing the scene with a camera that can be controlled
	ImGui::Begin("Scene View");
	if (!m_bRunning)
		m_displayLayer->onUpdate(timestep); // Update the display layer, render the scene with the editor camera
	ImGui::End();

	// Create Menu Window
	ImGui::Begin("Menu");

	if (!m_bRunning) // If not running the game
	{
		// Create New Button
		if (ImGui::Button("New"))
			m_bLeaving = true;

		// Same Line as...
		ImGui::SameLine();

		// Create Save Button
		if (ImGui::Button("Save"))
		{
			saveEditor();
		}

		// Same Line as...
		ImGui::SameLine();

		// Load Button
		if (ImGui::Button("Load"))
			m_bLeaving = true;

		ImGui::SameLine();

		if (ImGui::Button("Run")) // To start running the game
		{
			m_iCurrentObjectSelected = 0;
			m_bComponentsShowing = false;

			m_iLastCameraSelected = m_iResetCamera;

			saveRunning(); // Create temporary JSON files to load from

			m_bRunning = true;

			ImGui::SetWindowFocus("Game View"); // Focus on the game view window instead of scene view
		}
	}
	else // If running the game
	{
		// Output Running Text
		std::string sRunningTextOutput = "Running"; // Create Base String
		static short iRunningTextOutputDots = 0; // Set Number of Dots to Output
		static short iDotChangeCountdown = 10; // Set Countdown to Change Number of Dots
		for (int i = 0; i < 4; i++) // Loop
		{
			// Add a Dot or a Space
			if (i >= iRunningTextOutputDots) // Output Space?
				sRunningTextOutput += " "; // Add Space
			else // Otherwise
				sRunningTextOutput += "."; // Add Dot
		}
		iDotChangeCountdown--; // Decrement Dot Change Countdown
		if (iDotChangeCountdown <= 0) // If Countdown Ticked Over
		{
			// Set Number of Dots Value
			if (iRunningTextOutputDots >= 3) // If Max Value Reached
				iRunningTextOutputDots = 0; // Set to 0
			else // Otherwise
				iRunningTextOutputDots++; // Increment

			// Reset Counter
			iDotChangeCountdown = 10;
		}
		ImGui::Text(sRunningTextOutput.c_str()); // Output Text

		// Same Line as...
		ImGui::SameLine();

		// Add Stop Button
		if (ImGui::Button("Stop"))
		{
			// Stop Showing Object Components
			m_iCurrentObjectSelected = 0; // Select No Objects
			m_bComponentsShowing = false; // Don't Show Components

			// Forget Selected Camera
			m_iLastCameraSelected = m_iResetCamera;

			std::string filepath;
			// Remove temporary JSON files
			std::vector<std::string>::iterator it;
			for (it = m_displayLayer->m_layerData.m_3DLayers.begin(); it != m_displayLayer->m_layerData.m_3DLayers.end(); ++it)
			{
				filepath = "assets/savedJSON/" + (*it) + ".json";
				std::filesystem::remove(filepath.c_str());
			}
			for (it = m_displayLayer->m_layerData.m_2DLayers.begin(); it != m_displayLayer->m_layerData.m_2DLayers.end(); ++it)
			{
				filepath = "assets/savedJSON/" + (*it) + ".json";
				std::filesystem::remove(filepath.c_str());
			}

			filepath = "assets/savedJSON/TempControl.json";
			std::filesystem::remove(filepath.c_str());

			// Remove any temporary shaders
			filepath = "assets/savedJSON/00.glsl";
			if (std::filesystem::exists(filepath))
				std::filesystem::remove(filepath.c_str());
			filepath = "assets/savedJSON/01.glsl";
			if (std::filesystem::exists(filepath))
				std::filesystem::remove(filepath.c_str());
			filepath = "assets/savedJSON/10.glsl";
			if (std::filesystem::exists(filepath))
				std::filesystem::remove(filepath.c_str());
			filepath = "assets/savedJSON/11.glsl";
			if (std::filesystem::exists(filepath))
				std::filesystem::remove(filepath.c_str());

			// Set Running Control Boolean to False
			m_bRunning = false;
		}
	}

	ImGui::SameLine();
	// Slider to control the size multiplier of the game display windows
	ImGui::SliderFloat("Display Size", &m_displayWindowMultiplier, 0.2f, 2.f);
	m_displayLayer->m_displayWindowMultiplier = m_displayWindowMultiplier;

	// End Menu Window
	ImGui::End();

	// Create Add Game Objects Window
	ImGui::Begin("Add");
	ImGui::Text("Default Objects");
	// Make a button for each gameobject in the shared model file
	for (const auto& entry : std::filesystem::directory_iterator("assets/models/"))
	{
		std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());

		if (fileType.compare(".mtl") != 0)
		{
			// If the button is pressed
			if (ImGui::Button(ChangeObjects::parseFilePath(entry.path().string()).c_str()))
			{
				std::shared_ptr<Engine::GameObject> newObject;
				// Make the object and add the needed components
				ChangeObjects::makeGameObject(ChangeObjects::parseFilePath(entry.path().string()), newObject, false);
				ChangeObjects::addMesh(entry.path().string(), newObject);
				ChangeObjects::addMaterial(ChangeObjects::parseFilePath(entry.path().string()), newObject, false);
				ChangeObjects::addPosition(newObject, false);
				ChangeObjects::addTexture(newObject, false);
			}
		}
	}
	ImGui::NewLine();
	ImGui::Text("Custom Objects");
	std::string modelsFilePath = "assets/savedJSON/" + m_sProjectName + "/models/";
	// Make a button for each gameobject in the projects model file
	for (const auto& entry : std::filesystem::directory_iterator(modelsFilePath))
	{
		std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());

		if (fileType.compare(".txt") != 0 && fileType.compare(".mtl") != 0)
		{
			if (ImGui::Button(ChangeObjects::parseFilePath(entry.path().string()).c_str()))
			{
				std::shared_ptr<Engine::GameObject> newObject;
				// Make the object and add the needed components
				ChangeObjects::makeGameObject(ChangeObjects::parseFilePath(entry.path().string()), newObject, false);
				ChangeObjects::addMesh(entry.path().string(), newObject);
				ChangeObjects::addMaterial(ChangeObjects::parseFilePath(entry.path().string()), newObject, false);
				ChangeObjects::addPosition(newObject, false);
				ChangeObjects::addTexture(newObject, false);
			}
		}
	}
	ImGui::NewLine();
	ImGui::Text("Joints");
	// Make a button for each type of joint
	if (ImGui::Button("Ball and Socket"))
	{
		ChangeObjects::makeJoint("Ball and Socket");
	}
	if (ImGui::Button("Hinge"))
	{
		ChangeObjects::makeJoint("Hinge");
	}
	if (ImGui::Button("Fixed"))
	{
		ChangeObjects::makeJoint("Fixed");
	}
	if (ImGui::Button("Slider"))
	{
		ChangeObjects::makeJoint("Slider");
	}

	ImGui::NewLine();
	ImGui::Text("UI");
	if (ImGui::Button("Text"))
	{
		std::shared_ptr<Engine::GameObject> newObject;
		// Make the object and add the needed components
		ChangeObjects::makeGameObject("Text", newObject, true);
	}

	// End 'Add' window
	ImGui::End();

	// If there are game objects in the level
	if (!m_displayLayer->m_layerData.m_objects.empty())
	{
		// Create Editor Window
		ImGui::Begin("Editor");
		if (ImGui::CollapsingHeader("Objects")) // Create Joints Collapsable Header
		{
			// For each object
			for (auto& it : m_displayLayer->m_layerData.m_objects)
			{
				// Create Pointer to Object
				std::shared_ptr<Engine::GameObject> object = m_displayLayer->getGameObjects().at(it.m_objectVectorValue);

				// Define Header Name
				std::string sHeaderName = object->name() + "###Object" + std::to_string(it.m_objectVectorValue);

				bool selected = false;
				if (it.m_objectVectorValue == m_iCurrentObjectSelected && m_bComponentsShowing)
					selected = true;
				// Make a list of selectables for each object in the level
				if (ImGui::Selectable(sHeaderName.c_str(), selected))
				{
					// Select Object
					m_iCurrentObjectSelected = it.m_objectVectorValue;
					if (it.m_hasCamera)
						m_iLastCameraSelected = it.m_cameraComponentVectorValue;

					// Mark Components as Showing
					m_bComponentsShowing = true;
					m_b3DComponentsShowing = true;
				}

				// Begin Components Window
				ImGui::Begin("Components");

				// If Components are Showing AND the Current Object is Selected
				if (m_bComponentsShowing && m_iCurrentObjectSelected == it.m_objectVectorValue)
				{
					// Make ImGui fields to display and change things
					ChangeObjects::changeLayer(it.m_objectVectorValue, true, false);
					ChangeObjects::changeTag(it.m_objectVectorValue, false);
					ImGui::NewLine();
					ChangeObjects::changeName(it.m_objectVectorValue, false);

					ImGui::NewLine(); // Add Line Break

					// Indent
					ImGui::Indent(16.f);

					// Create Transform GUI
					std::string sFInputName = "Transform:###Tran" + std::to_string(it.m_objectVectorValue); // Set Collapsable Header Name
					if (ImGui::CollapsingHeader(sFInputName.c_str())) // Create Transform Collapsable Header
					{
						// Indent GUI
						ImGui::Indent(16.f);

						// Create Position GUI
						ChangeObjects::changePosition(it.m_objectVectorValue, false);

						// Create Rotation GUI
						ChangeObjects::changeRotation(it.m_objectVectorValue, false);

						// Create Scale GUI
						ChangeObjects::changeScale(it.m_objectVectorValue, false);

						// Unindent GUI
						ImGui::Unindent(16.f);
					}

					if (it.m_hasMesh)
						ChangeObjects::changeMesh(it.m_objectVectorValue);

					// Output Material GUI
					if (it.m_hasMat) // If Object Has A Material Component
						ChangeObjects::changeMaterial(it.m_objectVectorValue); // Output Material GUI

					// Output Light GUI
					if (it.m_hasLight) // If Object Is A/The Light
						ChangeObjects::changeLight(it.m_objectVectorValue); // Output Light GUI

					// Output Camera GUI
					if (it.m_hasCamera) // If Object Is A/The Camera
						ChangeObjects::changeCamera(it.m_objectVectorValue); // Output Camera GUI

					// Output Collider GUI
					if (it.m_hasCollider) // If Object Has Collider Component
						ChangeObjects::changeCollider(it.m_objectVectorValue); // Output Collider GUI

					// Output RidgidBody GUI
					if (it.m_hasRigidbody) // If Object Has RidgidBody Component
						ChangeObjects::changeRigidbody(it.m_objectVectorValue); // Output RidgidBody Component GUI

					// Output Lua Script GUI
					if (it.bHasLuaScript) // If Object Has Lua Script Component
						ChangeObjects::LuaScriptComponentGUI(it.m_objectVectorValue, false); // Output Lua Script Component GUI

					// Indent
					ImGui::Unindent(16.f);

					// Add a Line Break
					ImGui::NewLine();

					// Create Add Component Dropdown List
					ImGui::Text("Add Component:"); // Create Text Label
					ImGui::SameLine(); // Make Same Line As...
					const char* AddComponentListItems[] = { "Collider", "Rigidbody", "Lua Script" }; // Set List Variables
					static const char* CurrentAddComponentListItem = NULL; // Preinitialize Pointer to Null
					ImGui::PushItemWidth(20.f); // Set Widget Width to 20f, So Only Down Arrow Showing
					if (ImGui::BeginCombo("##combo", CurrentAddComponentListItem)) // Create Drop Down
					{
						// Loop Through List
						for (int n = 0; n < IM_ARRAYSIZE(AddComponentListItems); n++)
						{
							// Get if the Item is Selected
							bool is_selected = (CurrentAddComponentListItem == AddComponentListItems[n]);

							// Display List
							if (ImGui::Selectable(AddComponentListItems[n], is_selected)) // Create a Selectable Item
							{
								// If the Item Has Been Selected, Perform Relevant Action
								switch (n)
								{
									// Adding a Collider Component
								case 0:
									ChangeObjects::addCollider(it);
									break;

									// Adding a RidgidBody Component
								case 1:
									ChangeObjects::addRigidbody(it);
									break;

									// Adding A Lua Script Component
								case 2:
									ChangeObjects::AddLuaScriptComponent(it);
									break;
								}
							}
						}

						// Stop Creating List
						ImGui::EndCombo();
					}
					// Add a Line Break
					ImGui::NewLine();

					// If Not a Light or Camera Object
					if (!it.m_hasLight && !it.m_hasCamera)
					{
						// 'Indent' to the Right
						ImGui::Text("                                   ");
						ImGui::SameLine();

						// Define Delete Button Name
						sFInputName = "Delete Object###Delete" + std::to_string(it.m_objectVectorValue);

						// Create Delete Object Button
						if (ImGui::Button(sFInputName.c_str())) // If Pressed
						{
							// Delete Object
							DeleteObject(it);

							// Break Out of Iterator
							break;
						}
					}
				}

				// End Components Window
				ImGui::End();
			}
			ImGui::NewLine();
		}

		// If there are joints in the level
		if (!m_displayLayer->m_layerData.m_joints.empty())
		{
			if (ImGui::CollapsingHeader("Joints")) // Create Joints Collapsable Header
			{
				// For each joint
				for (auto& it : m_displayLayer->m_layerData.m_joints)
				{
					bool selected = false;
					if (it.m_jointVectorValue + m_displayLayer->m_layerData.m_objects.size() == m_iCurrentObjectSelected && m_bComponentsShowing)
						selected = true;
					std::string sHeaderName = "Joint " + std::to_string(it.m_jointVectorValue);
					// Make a selectable for each joint
					if (ImGui::Selectable(sHeaderName.c_str(), selected))
					{
						// Select joint
						m_iCurrentObjectSelected = it.m_jointVectorValue + m_displayLayer->m_layerData.m_objects.size();

						// Mark Components as Showing
						m_bComponentsShowing = true;
						m_b3DComponentsShowing = true;
					}

					// Begin Components Window
					ImGui::Begin("Components");

					// If Components are Showing AND the Current joint is Selected
					if (m_bComponentsShowing && m_iCurrentObjectSelected == it.m_jointVectorValue + m_displayLayer->m_layerData.m_objects.size() && m_b3DComponentsShowing)
					{
						ChangeObjects::changeLayer(it.m_jointVectorValue, false, false);
						ImGui::NewLine();
						ChangeObjects::changeJoint(it.m_jointVectorValue);

						// 'Indent' to the Right
						ImGui::Text("                                   ");
						ImGui::SameLine();
						// Define Delete Button Name
						std::string sFInputName = "Delete Joint###DeleteJoint" + std::to_string(it.m_jointVectorValue);

						// Create Delete Joint Button
						if (ImGui::Button(sFInputName.c_str())) // If Pressed
						{
							// Delete Joint
							DeleteJoint(it);

							// Break Out of Iterator
							break;
						}
					}

					// End Components Window
					ImGui::End();
				}
				ImGui::NewLine();
			}
		}
	}

	if (!m_displayLayer->m_layerData.m_2DObjects.empty())
	{
		if (ImGui::CollapsingHeader("UI")) // Create Joints Collapsable Header
		{
			// For each object
			for (auto& it : m_displayLayer->m_layerData.m_2DObjects)
			{
				if (!it.m_hasCamera)
				{
					// Create Pointer to Object
					std::shared_ptr<Engine::GameObject> object = m_displayLayer->get2DGameObjects().at(it.m_objectVectorValue);

					// Define Header Name
					std::string sHeaderName = object->name() + "###Object" + std::to_string(it.m_objectVectorValue + m_displayLayer->getGameObjects().size() + m_displayLayer->m_layerData.m_joints.size());

					bool selected = false;
					if (it.m_objectVectorValue + m_displayLayer->getGameObjects().size() + m_displayLayer->m_layerData.m_joints.size() == m_iCurrentObjectSelected && m_bComponentsShowing)
						selected = true;
					// Make a list of selectables for each object in the level
					if (ImGui::Selectable(sHeaderName.c_str(), selected))
					{
						// Select Object
						m_iCurrentObjectSelected = it.m_objectVectorValue + m_displayLayer->getGameObjects().size() + m_displayLayer->m_layerData.m_joints.size();

						// Mark Components as Showing
						m_bComponentsShowing = true;
						m_b3DComponentsShowing = false;
					}

					// Begin Components Window
					ImGui::Begin("Components");

					// If Components are Showing AND the Current Object is Selected
					if (m_bComponentsShowing && m_iCurrentObjectSelected == it.m_objectVectorValue + m_displayLayer->getGameObjects().size() + m_displayLayer->m_layerData.m_joints.size() && !m_b3DComponentsShowing)
					{
						// Make ImGui fields to display and change things
						ChangeObjects::changeLayer(it.m_objectVectorValue, true, true);
						ChangeObjects::changeTag(it.m_objectVectorValue, true);
						ImGui::NewLine();
						ChangeObjects::changeName(it.m_objectVectorValue, true);

						ImGui::NewLine(); // Add Line Break

						// Indent
						ImGui::Indent(16.f);

						// Create Transform GUI
						std::string sFInputName = "Transform:###Tran" + std::to_string(it.m_objectVectorValue + m_displayLayer->getGameObjects().size() + m_displayLayer->m_layerData.m_joints.size()); // Set Collapsable Header Name
						//if (ImGui::CollapsingHeader(sFInputName.c_str())) // Create Transform Collapsable Header
						//{
							// Indent GUI
							//ImGui::Indent(16.f);

							// Create Position GUI
							//ChangeObjects::changePosition(it.m_objectVectorValue, true);

							// Create Rotation GUI
							//ChangeObjects::changeRotation(it.m_objectVectorValue, true);

							// Create Scale GUI
							//ChangeObjects::changeScale(it.m_objectVectorValue, true);

							// Unindent GUI
							//ImGui::Unindent(16.f);
						//}

						if (it.m_hasText)
							ChangeObjects::changeText(it.m_objectVectorValue);

						// Output Lua Script GUI
						if (it.bHasLuaScript) // If Object Has Lua Script Component
							ChangeObjects::LuaScriptComponentGUI(it.m_objectVectorValue, true); // Output Lua Script Component GUI

						// Indent
						ImGui::Unindent(16.f);

						// Add a Line Break
						ImGui::NewLine();

						// Create Add Component Dropdown List
						ImGui::Text("Add Component:"); // Create Text Label
						ImGui::SameLine(); // Make Same Line As...
						const char* AddComponentListItems[] = { "Lua Script" }; // Set List Variables
						static const char* CurrentAddComponentListItem = NULL; // Preinitialize Pointer to Null
						ImGui::PushItemWidth(20.f); // Set Widget Width to 20f, So Only Down Arrow Showing
						if (ImGui::BeginCombo("##combo", CurrentAddComponentListItem)) // Create Drop Down
						{
							// Loop Through List
							for (int n = 0; n < IM_ARRAYSIZE(AddComponentListItems); n++)
							{
								// Get if the Item is Selected
								bool is_selected = (CurrentAddComponentListItem == AddComponentListItems[n]);

								// Display List
								if (ImGui::Selectable(AddComponentListItems[n], is_selected)) // Create a Selectable Item
								{
									// If the Item Has Been Selected, Perform Relevant Action
									switch (n)
									{
										// Adding A Lua Script Component
									case 0:
										ChangeObjects::AddLuaScriptComponent(it);
										break;
									}
								}
							}

							// Stop Creating List
							ImGui::EndCombo();
						}
						// Add a Line Break
						ImGui::NewLine();

						// If Not a Light or Camera Object
						if (!it.m_hasLight)
						{
							// 'Indent' to the Right
							ImGui::Text("                                   ");
							ImGui::SameLine();

							// Define Delete Button Name
							sFInputName = "Delete Object###Delete" + std::to_string(it.m_objectVectorValue + m_displayLayer->getGameObjects().size() + m_displayLayer->m_layerData.m_joints.size());

							// Create Delete Object Button
							if (ImGui::Button(sFInputName.c_str())) // If Pressed
							{
								// Delete Object
								DeleteObject(it);

								// Break Out of Iterator
								break;
							}
						}
					}

					// End Components Window
					ImGui::End();
				}
			}
		}
		// End Editor Window
		ImGui::End();
	}

	// Begine Camera View Window
	ImGui::Begin("Game View");

	if (!m_bRunning) // If the game is not running
	{
		// Set camera variables used for uniform buffers
		m_displayLayer->getCamViewProj() = m_displayLayer->getCameras().at(m_iLastCameraSelected)->getCamera()->getViewProjection();
		m_displayLayer->getCamPos() = m_displayLayer->getCameras().at(m_iLastCameraSelected)->getCamera()->getPosition();

		m_displayLayer->getOrthoCamViewProj() = m_displayLayer->getCameras().at(m_i2DCameraVectorValue)->getCamera()->getViewProjection();

		std::vector<std::string> layers = m_displayLayer->getCameras().at(m_iLastCameraSelected)->getLayersSeen();

		// Make a vector containing all of the objects in the layers seen be the camera
		std::vector<Object> renderObjects;
		int i = 0;
		for (auto& mat : m_displayLayer->getMaterials())
		{
			Object object;
			bool render = false;
			
			for (auto& objectIt : m_displayLayer->m_layerData.m_objects)
			{
				// If the object has a material and is the object currently being rendered
				if (objectIt.m_hasMat && objectIt.m_materialComponentVectorValue == i)
				{
					object = objectIt;
					render = true;
					break;
				}
			}
			if (render)
			{
				// If the object is in a layer which is seen by the camera, add it to the vector
				if (std::count(layers.begin(), layers.end(), object.m_layer))
				{
					renderObjects.push_back(object);
				}
			}
			i++;
		}

#pragma region Shadow Depth Map
		m_pFrameBuffer->shadowBind(); // Bind the shadow map frame buffer

		for (auto& renderCommand : m_displayLayer->getPredrawCommands())
			m_displayLayer->getRenderer()->actionCommand(renderCommand.get());

		glm::mat4 lightSpaceMatrix = m_displayLayer->m_lightSpaceMatrix;

		for (auto& object : renderObjects)
		{
			std::shared_ptr<Engine::MaterialComponent> mat = m_displayLayer->getMaterials().at(object.m_materialComponentVectorValue);

			std::pair<std::string, void*> lightSpace("u_lightSpaceMatrix", (void*)&lightSpaceMatrix[0][0]); // Store the light space matrix data
			Engine::ComponentMessage matrixMessage(Engine::ComponentMessageType::UniformSet, (void*)&lightSpace); // Submit the matrix uniform as a component message
			mat->receiveMessage(matrixMessage);	// Apply the recieved uniform data to the material

			std::shared_ptr<Engine::Shader> objectShader = mat->getMaterial()->getShader();	// Store the shader currently used by the material
			mat->getMaterial()->setShader(m_displayLayer->m_pFBShaderShadowLightSpace);	// Set the shader used by the material to the light space shader

			m_displayLayer->getRenderer()->submit(mat->getMaterial()); // Draw geometry to texture (off-screen rendering)
			mat->getMaterial()->setShader(objectShader); // Set the shader used by the material to the default shader
		}

		for (auto& renderCommand : m_displayLayer->getPostdrawCommands())
			m_displayLayer->getRenderer()->actionCommand(renderCommand.get());

		m_pFrameBuffer->shadowUnbind();	// Unbind the shadow map frame buffer
#pragma endregion

#pragma region Draw Objects
		m_pFrameBuffer->defaultBind();

		for (auto& renderCommand : m_displayLayer->getPredrawCommands())
			m_displayLayer->getRenderer()->actionCommand(renderCommand.get());

		m_displayLayer->getRenderer()->beginScene(*m_displayLayer->getSceneData()); // Submit uniform variable data

		unsigned int depthMapTextureSlot = m_pFrameBuffer->getDepthMapTextureSlot(); // Store the depth texture slot of the depth map texture

		for (auto& object : renderObjects)
		{
			std::shared_ptr<Engine::MaterialComponent> mat = m_displayLayer->getMaterials().at(object.m_materialComponentVectorValue);

			std::pair<std::string, void*> depthMap("u_shadowDepth", (void*)&depthMapTextureSlot); // Store the depth map texture slot data
			Engine::ComponentMessage textureMessage(Engine::ComponentMessageType::UniformSet, (void*)&depthMap); // Submit the texture uniform as a component message
			mat->receiveMessage(textureMessage); // Apply the recieved uniform data to the material

			std::pair<std::string, void*> lightSpace("u_lightSpaceMatrix", (void*)&lightSpaceMatrix[0][0]); // Store the light space matrix data
			Engine::ComponentMessage matrixMessage(Engine::ComponentMessageType::UniformSet, (void*)&lightSpace); // Submit the matrix uniform as a component message
			mat->receiveMessage(matrixMessage); // Apply the recieved uniform data to the material

			m_displayLayer->getRenderer()->submit(mat->getMaterial()); // Draw geometry to texture (off-screen rendering)
		}

		for (auto& renderCommand : m_displayLayer->getPostdrawCommands())
			m_displayLayer->getRenderer()->actionCommand(renderCommand.get());
#pragma endregion

#pragma region Draw UI
		for (auto& renderCommand : m_displayLayer->getPredrawUICommands())
			m_pRenderer->actionCommand(renderCommand.get());
		
		for (auto& object : m_displayLayer->m_layerData.m_2DObjects)
		{
			if (object.m_hasMat)
			{
				std::shared_ptr<Engine::MaterialComponent> mat = m_displayLayer->getMaterials().at(object.m_materialComponentVectorValue);

				m_pRenderer->submit(mat->getMaterial()); // Draw geometry to texture (off-screen rendering)
			}
		}
		
		for (auto& renderCommand : m_displayLayer->getPostdrawUICommands())
			m_pRenderer->actionCommand(renderCommand.get());
#pragma endregion
	}
	else // If the game is running
	{
		// Update all layers in the layer stack
		for (auto it = m_pLayerstack->begin(); it != m_pLayerstack->end(); it++)
			(*it)->onUpdate(timestep);
		for (auto it = m_pLayerstack->begin(); it != m_pLayerstack->end(); it++)
			(*it)->drawObjects();

		m_pFrameBuffer->defaultBind();

		for (auto it = m_pLayerstack->begin(); it != m_pLayerstack->end(); it++)
			(*it)->drawLayer();
	}

	m_pFrameBuffer->defaultUnbind();

	ImVec2 windowSize;
	windowSize.x = 800 * m_displayWindowMultiplier;
	windowSize.y = windowSize.x / 1.33333333f;

	int tex = m_pFrameBuffer->getColourTextureID(0);

	// Display the rendered game view in the Game View window
	ImGui::Image((void*)tex, windowSize, ImVec2(0, 1), ImVec2(1, 0));

	if (m_bRunning) // If the game is running
	{
		// For only getting player input for the game when the correct image is being hovered over
		if (ImGui::IsItemHovered())
		{
			m_bHoveringOnImage = true;
		}
		else
		{
			m_bHoveringOnImage = false;
		}
	}

	ImGui::End();

	ImGui::Begin("Level Menu"); // Make a Level Menu window

	if (ImGui::CollapsingHeader("Levels:"))
	{
		ImGui::Indent(16.f);

		std::vector<std::pair<std::string, std::string>>::iterator level;
		int i = 0;
		// For each level in the project
		for (level = m_levels.begin(); level != m_levels.end();)
		{
			// Make a collapsing header for it
			if (ImGui::CollapsingHeader(level->first.c_str()))
			{
				ImGui::Indent(16.f);

				std::string buttonName;
				// If the level is not the current one and there is more than one level
				if (m_displayLayer->m_layerData.m_name.compare(level->first) != 0 && m_levels.size() > 1)
				{
					buttonName = "Go To Level###GoToLevel" + std::to_string(i);
					// Make a button for loading the level
					if (ImGui::Button(buttonName.c_str()))
					{
						// Reset things for loading a new level
						m_displayLayer->onDetach();

						m_pResources->stop();

						m_displayLayer = nullptr;
						ChangeObjects::setDisplayLayer(nullptr);
						m_displayLayer.reset(new DisplayLayer(level->second, level->first));

						m_displayLayer->onAttach();
						m_displayWindowMultiplier = m_displayLayer->m_displayWindowMultiplier;

						m_iCurrentObjectSelected = 0;
						m_bComponentsShowing = false;
						m_iLastCameraSelected = m_iResetCamera;

						m_bHoveringOnImage = false;

						m_layerSelected = 0;
						m_layerIsSelected = false;
						m_3DSelected = true;
						strcpy(m_newLayerName, "");
						strcpy(m_newLevelName, "");

						ChangeObjects::setDisplayLayer(m_displayLayer);

						m_iCurrentLevel = i;

						ImGui::End();
						EndFrame();

						return;
					}

					ImGui::SameLine();

					buttonName = "Delete Level###DeleteLevel" + std::to_string(i);
					// Make a button to delete the level
					if (ImGui::Button(buttonName.c_str()))
					{
						std::filesystem::remove(level->second);

						// Get the data from the JSON file
						std::fstream handle("assets/savedJSON/" + m_sProjectName + "/" + m_sProjectName + " Control.json", std::ios::in);
						if (!handle.is_open()) {
							LOG_CRITICAL("Could not open file: {0}", "assets/savedJSON/" + m_sProjectName + "/" + m_sProjectName + " Control.json");
						}
						// Make the JSON loader to get all of the data
						nlohmann::json levelJSON;
						handle >> levelJSON;

						nlohmann::json JSONlevels;
						// Change the projects 'Control' JSON file to not have the level in anymore
						for (auto& JSONLevel : levelJSON["Load"]["levels"])
						{
							if (JSONLevel["name"].get<std::string>().compare(level->first) != 0)
							{
								JSONlevels.push_back(JSONLevel);
							}
						}
						levelJSON["Load"]["levels"] = JSONlevels;

						std::ofstream controlFile("assets/savedJSON/" + m_sProjectName + "/" + m_sProjectName + " Control.json");
						controlFile << levelJSON;

						m_levels.erase(level); // Erase the level

						level = m_levels.begin(); // Reset the iterator
						i = 0;
					}

					ImGui::NewLine();
				}

				ImGui::Text("New Name:");

				buttonName = "###NewLevelName" + std::to_string(i);
				// Create a text field for renaming the level
				ImGui::InputText(buttonName.c_str(), m_newLevelName, IM_ARRAYSIZE(m_newLevelName));

				buttonName = "Rename Level###RenameLevel" + std::to_string(i);
				// Create a button to rename the level
				if (ImGui::Button(buttonName.c_str()))
				{
					// Change everything needed
					std::string oldName = level->first;

					level->first = m_newLevelName;

					std::string newLevelFilepath = "assets/savedJSON/" + m_sProjectName + "/" + level->first + ".json";

					std::filesystem::rename(level->second, newLevelFilepath);

					level->second = newLevelFilepath;

					if (m_displayLayer->m_layerData.m_name.compare(oldName) == 0)
					{
						m_displayLayer->m_layerData.m_name = level->first;
						m_displayLayer->m_layerData.m_filepath = level->second;
					}

					// Get the data from the JSON file
					std::fstream handle("assets/savedJSON/" + m_sProjectName + "/" + m_sProjectName + " Control.json", std::ios::in);
					if (!handle.is_open()) {
						LOG_CRITICAL("Could not open file: {0}", "assets/savedJSON/" + m_sProjectName + "/" + m_sProjectName + " Control.json");
					}
					// Make the JSON loader to get all of the data
					nlohmann::json levelJSON;
					handle >> levelJSON;
					// Change the level name in the 'Control' JSON file
					for (auto& JSONLevel : levelJSON["Load"]["levels"])
					{
						std::string name = JSONLevel["name"].get<std::string>();

						if (name.compare(oldName) == 0)
						{
							JSONLevel["name"] = level->first;
							JSONLevel["layerFilepath"] = level->second;

							break;
						}
					}

					std::ofstream controlFile("assets/savedJSON/" + m_sProjectName + "/" + m_sProjectName + " Control.json");
					controlFile << levelJSON;

					strcpy(m_newLevelName, "");
				}

				ImGui::Unindent(16.f);
			}
			i++;

			if (level != m_levels.end())
				level++;
		}

		ImGui::NewLine();
		// Make a button to make a new level
		if (ImGui::Button("New Level"))
		{
			std::string newName = "Level " + std::to_string(m_levels.size() + 1);
			std::string newFilepath = "assets/savedJSON/" + m_sProjectName + "/" + newName + ".json";
			std::filesystem::copy("assets/savedJSON/Default/3DEditorLayer.json", newFilepath);

			m_levels.push_back(std::make_pair(newName, newFilepath));


			// Get the data from the JSON file
			std::fstream handle("assets/savedJSON/" + m_sProjectName + "/" + m_sProjectName + " Control.json", std::ios::in);
			if (!handle.is_open()) {
				LOG_CRITICAL("Could not open file: {0}", "assets/savedJSON/" + m_sProjectName + "/" + m_sProjectName + " Control.json");
			}
			// Make the JSON loader to get all of the data
			nlohmann::json levelJSON;
			handle >> levelJSON;
			// Add the level in the 'Control' JSON file
			nlohmann::json newLevel;
			newLevel["name"] = newName;
			newLevel["layerFilepath"] = newFilepath;

			levelJSON["Load"]["levels"].push_back(newLevel);

			std::ofstream controlFile("assets/savedJSON/" + m_sProjectName + "/" + m_sProjectName + " Control.json");
			controlFile << levelJSON;
		}
		
		ImGui::Unindent(16.f);
	}

	ImGui::NewLine();
	ImGui::Text("Layers in current level:");
	ImGui::Indent(16.f);
	// Make a collapsing header for all the 3D layers
	if (ImGui::CollapsingHeader("3D Layers"))
	{
		ImGui::Indent(16.f);

		int i = 0;
		std::vector<std::string>::iterator it;
		// For each 3D layer
		for (it = m_displayLayer->m_layerData.m_3DLayers.begin(); it != m_displayLayer->m_layerData.m_3DLayers.end();)
		{
			bool selected = false;
			if (m_3DSelected && m_layerIsSelected && m_layerSelected == i)
				selected = true;

			std::string name = *it + "###Layer" + std::to_string(i);
			// Make a seletable
			if (ImGui::Selectable(name.c_str(), selected))
			{
				m_layerSelected = i;

				strcpy(m_newLayerName, (*it).c_str());
				m_layerIsSelected = true;
				m_3DSelected = true;
			}
			// If the layer is selected
			if (selected && m_layerIsSelected)
			{
				ImGui::Indent(16.f);
				ImGui::Text("New Name:");

				name = "###NewName" + std::to_string(i);
				// Make a text field for changing the name
				ImGui::InputText(name.c_str(), m_newLayerName, IM_ARRAYSIZE(m_newLayerName));

				name = "Rename###Rename" + std::to_string(i);
				// Make a button to change the name of a layer
				if (ImGui::Button(name.c_str()))
				{
					// Change the layer name for all the objects in the layer
					for (auto& object : m_displayLayer->m_layerData.m_objects)
					{
						if (object.m_layer == *it)
						{
							object.m_layer = m_newLayerName;
						}
					}
					// Change the layer name for all the joints in the layer
					for (auto& joint : m_displayLayer->m_layerData.m_joints)
					{
						if (joint.m_layer == *it)
						{
							joint.m_layer = m_newLayerName;
						}
					}
					// Change the layer name in the list of layers the camera in the game can see
					for (auto& camera : m_displayLayer->getCameras())
					{
						std::vector<std::string> layerList = camera->getLayersSeen();
						for (auto& layer : layerList)
						{
							if (layer.compare(*it) == 0)
							{
								layer = m_newLayerName;
							}
						}
						camera->setLayersSeen(layerList);
					}

					*it = m_newLayerName;
				}

				name = "Delete Layer###Delete" + std::to_string(i);
				// If there is more than one 3D layer
				if (m_displayLayer->m_layerData.m_3DLayers.size() > 1)
				{
					ImGui::SameLine();
					// make a button to delete the layer
					if (ImGui::Button(name.c_str()))
					{
						// Change any object or joint in the layer being deleted to be in the first layer in the vector
						for (auto& object : m_displayLayer->m_layerData.m_objects)
						{
							if (object.m_layer == *it)
							{
								object.m_layer = (*m_displayLayer->m_layerData.m_3DLayers.begin());
							}
						}
						for (auto& joint : m_displayLayer->m_layerData.m_joints)
						{
							if (joint.m_layer == *it)
							{
								joint.m_layer = (*m_displayLayer->m_layerData.m_3DLayers.begin());
							}
						}
						// Erase the layer
						m_displayLayer->m_layerData.m_3DLayers.erase(it);

						it = m_displayLayer->m_layerData.m_3DLayers.begin();
					}
				}
				ImGui::Text("No two layers in the same level can have the same name");

				ImGui::NewLine();
				ImGui::Unindent(16.f);
			}
			i++;

			if (it != m_displayLayer->m_layerData.m_3DLayers.end())
				it++;
		}

		ImGui::Unindent(16.f);
		ImGui::NewLine();
		// Make a button to make a new 3D layer
		if (ImGui::Button("New 3D Layer"))
		{
			m_displayLayer->m_layerData.m_3DLayers.push_back("Default " + std::to_string(m_displayLayer->m_layerData.m_3DLayers.size()));
		}
		ImGui::NewLine();
	}
	// Same as above but for 2D layers instead of 3D
	if (ImGui::CollapsingHeader("2D Layers"))
	{
		ImGui::Indent(16.f);

		int i = 0;
		std::vector<std::string>::iterator it;
		for (it = m_displayLayer->m_layerData.m_2DLayers.begin(); it != m_displayLayer->m_layerData.m_2DLayers.end();)
		{
			bool selected = false;
			if (!m_3DSelected && m_layerIsSelected && m_layerSelected == i)
				selected = true;

			std::string name = *it + "###Layer2D" + std::to_string(i);
			if (ImGui::Selectable(name.c_str(), selected))
			{
				m_layerSelected = i;

				strcpy(m_newLayerName, (*it).c_str());
				m_layerIsSelected = true;
				m_3DSelected = false;
			}
			if (selected && m_layerIsSelected)
			{
				ImGui::Indent(16.f);
				ImGui::Text("New Name:");

				name = "###NewName2D" + std::to_string(i);

				ImGui::InputText(name.c_str(), m_newLayerName, IM_ARRAYSIZE(m_newLayerName));

				name = "Rename###Rename2D" + std::to_string(i);
				if (ImGui::Button(name.c_str()))
				{
					for (auto& object : m_displayLayer->m_layerData.m_2DObjects)
					{
						if (object.m_layer == *it)
						{
							object.m_layer = m_newLayerName;
						}
					}

					*it = m_newLayerName;

					m_displayLayer->getCameras().at(m_i2DCameraVectorValue)->setLayersSeen(m_displayLayer->m_layerData.m_2DLayers);
				}

				name = "Delete Layer###Delete2D" + std::to_string(i);

				if (m_displayLayer->m_layerData.m_2DLayers.size() > 1)
				{
					ImGui::SameLine();

					if (ImGui::Button(name.c_str()))
					{
						for (auto& object : m_displayLayer->m_layerData.m_2DObjects)
						{
							if (object.m_layer == *it)
							{
								object.m_layer = (*m_displayLayer->m_layerData.m_2DLayers.begin());
							}
						}

						m_displayLayer->m_layerData.m_2DLayers.erase(it);

						it = m_displayLayer->m_layerData.m_2DLayers.begin();

						m_displayLayer->getCameras().at(m_i2DCameraVectorValue)->setLayersSeen(m_displayLayer->m_layerData.m_2DLayers);
					}
				}
				ImGui::Text("No two layers in the same level can have the same name");

				ImGui::NewLine();
				ImGui::Unindent(16.f);
			}
			i++;

			if (it != m_displayLayer->m_layerData.m_2DLayers.end())
				it++;
		}

		ImGui::Unindent(16.f);
		ImGui::NewLine();
		if (ImGui::Button("New 2D Layer"))
		{
			m_displayLayer->m_layerData.m_2DLayers.push_back("Default " + std::to_string(m_displayLayer->m_layerData.m_2DLayers.size()));

			m_displayLayer->getCameras().at(m_i2DCameraVectorValue)->setLayersSeen(m_displayLayer->m_layerData.m_2DLayers);
		}
	}
	ImGui::Unindent(16.f);

	ImGui::NewLine();
	// Make a collapsing header for the tags
	if (ImGui::CollapsingHeader("Tags"))
	{
		ImGui::Indent(16.f);

		int i = 0;
		std::vector<std::string>::iterator it;
		// For each tag
		for (it = m_displayLayer->m_layerData.m_tags.begin(); it != m_displayLayer->m_layerData.m_tags.end();)
		{
			bool selected = m_iTagSelected == i;

			std::string name = *it + "###Tag" + std::to_string(i);
			// Make a selectable for it
			if (ImGui::Selectable(name.c_str(), selected))
			{
				m_iTagSelected = i;

				m_bTagIsSelected = true;
			}
			// If the tag is selected and it isn't the default tag
			if (selected && m_bTagIsSelected && (*it).compare("Default") != 0)
			{
				ImGui::Indent(16.f);

				name = "Delete Tag###Delete" + std::to_string(i);
				// If there is more than one tag
				if (m_displayLayer->m_layerData.m_tags.size() > 1)
				{
					// Make a button to delete it
					if (ImGui::Button(name.c_str()))
					{
						for (auto& object : m_displayLayer->m_layerData.m_objects)
						{
							// Change the tag of all objects with the one being deleted to be the first one in the vector
							if (object.m_tag == *it)
							{
								object.m_tag = (*m_displayLayer->m_layerData.m_tags.begin());
							}
						}
						// Erase the tag
						m_displayLayer->m_layerData.m_tags.erase(it);

						it = m_displayLayer->m_layerData.m_tags.begin();
					}
				}

				ImGui::Unindent(16.f);
			}
			i++;

			if (it != m_displayLayer->m_layerData.m_tags.end())
				it++;
		}
		ImGui::NewLine();
		if (!m_bMakingNewTag) // If not adding a new tag
		{
			// Make a button to start adding a new tag
			if (ImGui::Button("New Tag"))
			{
				m_bMakingNewTag = true;
			}
		}
		// If adding a new tag
		if (m_bMakingNewTag)
		{
			// Make a text field to give the tag a name
			ImGui::InputText("New Tag Name", m_newTagName, IM_ARRAYSIZE(m_newTagName));
			// Make a button to press to make the tag with the name given
			if (ImGui::Button("Create"))
			{
				m_bMakingNewTag = false;
				m_displayLayer->m_layerData.m_tags.push_back(m_newTagName);
			}
		}

		ImGui::Unindent(16.f);
	}

	ImGui::End();

	EndFrame();
}

void UILayer::onGameEvent(Engine::Event& event)
{
	// Create event dispatcher
	Engine::EventDispatcher dispatcher(event);
	// If the Event type matches, call the corresponding function
	dispatcher.dispatch<Engine::WindowFocusEvent>(std::bind(&UILayer::onWindowFocus, this, std::placeholders::_1));

	if (m_bRunning)
	{
		// Reverse iterate through the layer stack, once an event has been handled, leave the loop
		for (auto it = m_pLayerstack->rbegin() + 1; it != m_pLayerstack->rend(); ++it)
		{
			if (m_bHoveringOnImage)
			{
				(*it)->onEvent(event);
				if (event.handled()) break;
			}
		}
	}
	m_displayLayer->onEvent(event);

	if (m_pDebugLayer != nullptr)
		m_pDebugLayer->onEvent(event);
}

void UILayer::saveEditor()
{
	// Load the saved JSON file for the level
	std::fstream handle(m_displayLayer->m_layerData.m_filepath, std::ios::in);
	if (!handle.is_open()) {
		LOG_CRITICAL("Could not open file: {0}", m_displayLayer->m_layerData.m_filepath);
	}
	// Set up a JSON object to be changed
	nlohmann::json saveJSON;
	handle >> saveJSON;

	// Save the editor camera position/rotation
	auto& camera = saveJSON["camera"];
	glm::vec3 pos = m_displayLayer->getCamera()->getPosition();
	glm::vec2 rot = m_displayLayer->getCamera()->getYawPitch();
	camera["position"]["x"] = pos.x;
	camera["position"]["y"] = pos.y;
	camera["position"]["z"] = pos.z;
	camera["rotation"]["yaw"] = rot.x;
	camera["rotation"]["pitch"] = rot.y;
	// Save the display size multiplier
	saveJSON["displaySize"] = m_displayWindowMultiplier;

	// Make an array for the 3D layers
	nlohmann::json layerStart;
	layerStart["name"] = "Default3D";
	nlohmann::json layerArray = nlohmann::json(m_displayLayer->m_layerData.m_3DLayers.size(), layerStart);

	int i = 0;
	for (auto& it : m_displayLayer->m_layerData.m_3DLayers) // Add the names of all the 3D layers in the level to the array
	{
		layerArray[i]["name"] = it;

		i++;
	}
	// Save the names of the 3D layers
	saveJSON["3DLayers"] = layerArray;

	// Make an array for the 2D layers
	layerStart["name"] = "Default2D";
	layerArray = nlohmann::json(m_displayLayer->m_layerData.m_2DLayers.size(), layerStart);

	i = 0;
	for (auto& it : m_displayLayer->m_layerData.m_2DLayers) // Add the names of all the 2D layers in the level to the array
	{
		layerArray[i]["name"] = it;

		i++;
	}
	// Save the names of the 2D layers
	saveJSON["2DLayers"] = layerArray;

	// Make an array for all the tags in the level
	nlohmann::json tagStart;
	tagStart["tag"] = "Default";
	nlohmann::json tagArray = nlohmann::json(m_displayLayer->m_layerData.m_tags.size(), tagStart);

	i = 0;
	for (auto& it : m_displayLayer->m_layerData.m_tags) // Add all the tags to the array
	{
		tagArray[i]["tag"] = it;

		i++;
	}
	// Save all the tags in the level
	saveJSON["tags"] = tagArray;

	
	// Save the memory info vector sizes
	auto& memInfo = saveJSON["MemoryInfo"];
	memInfo["gameObjects"] = m_displayLayer->m_layerData.m_objects.size();
	memInfo["2DGameobjects"] = m_displayLayer->m_layerData.m_2DObjects.size();
	memInfo["materials"] = m_displayLayer->getMaterials().size();
	memInfo["mesh"] = m_displayLayer->getMeshes().size();
	memInfo["position"] = m_displayLayer->getPositions().size();
	memInfo["lights"] = m_displayLayer->getLights().size();
	memInfo["cameras"] = m_displayLayer->getCameras().size();
	memInfo["texture"] = m_displayLayer->getTextures().size();
	memInfo["joints"] = m_displayLayer->m_layerData.m_joints.size();
	memInfo["text"] = m_displayLayer->getTexts().size();

	auto& objects = saveJSON["GameObjects"]; // Get the current game object list saved

	nlohmann::json objectStart;
	objectStart["name"] = "Object";
	// Resize the list to contain all the objects now in the level
	nlohmann::json objectArray = nlohmann::json(m_displayLayer->m_layerData.m_objects.size() + m_displayLayer->m_layerData.m_2DObjects.size(), objectStart);

	i = saveEditorObject(0, &m_displayLayer->m_layerData.m_objects, m_displayLayer->getGameObjects(), objectArray);
	saveEditorObject(i, &m_displayLayer->m_layerData.m_2DObjects, m_displayLayer->get2DGameObjects(), objectArray);
	objects = objectArray;

	auto& joints = saveJSON["Joints"]; // Get the current joint list saved

	nlohmann::json jointStart;
	jointStart["type"] = "";
	// Resize the list for the current number of joints in the level
	nlohmann::json jointArray = nlohmann::json(m_displayLayer->m_layerData.m_joints.size(), jointStart);

	// Save information for each joint in the list
	i = 0;
	for (auto& it : m_displayLayer->m_layerData.m_joints)
	{
		jointArray[i]["type"] = it.m_type;
		jointArray[i]["anchorpoint"]["x"] = it.m_position.x;
		jointArray[i]["anchorpoint"]["y"] = it.m_position.y;
		jointArray[i]["anchorpoint"]["z"] = it.m_position.z;
		jointArray[i]["bodyA"] = it.m_bodyAName;
		jointArray[i]["bodyB"] = it.m_bodyBName;
		jointArray[i]["canCollide"] = it.m_bBodiesCanCollide;
		jointArray[i]["isLimited"] = it.m_bIsLimited;
		jointArray[i]["maxLimit"] = it.m_fMaxLimit;
		jointArray[i]["minLimit"] = it.m_fMinLimit;
		jointArray[i]["axis"]["x"] = it.m_axis.x;
		jointArray[i]["axis"]["y"] = it.m_axis.y;
		jointArray[i]["axis"]["z"] = it.m_axis.z;

		jointArray[i]["layerName"] = it.m_layer;

		i++;
	}
	joints = jointArray;

	// Save the changed JSON file
	std::ofstream file(m_displayLayer->m_layerData.m_filepath);
	file << saveJSON;
}

int UILayer::saveEditorObject(int i, std::vector<Object>* pObjects, std::vector<std::shared_ptr<Engine::GameObject>>& gameObjects, nlohmann::json& objectArray)
{
	// Save all the information for all the objects in the level
	for (auto& it : *pObjects)
	{
		// Save the objects name, index value and tag
		if (it.m_in3DLayer)
			objectArray[i]["index"] = it.m_objectVectorValue;
		else
			objectArray[i]["index"] = m_displayLayer->m_layerData.m_objects.size() + it.m_objectVectorValue;

		objectArray[i]["name"] = gameObjects.at(it.m_objectVectorValue)->name();
		objectArray[i]["tag"] = it.m_tag;

		// If the object has a mesh component
		if (it.m_hasMesh)
		{
			objectArray[i]["mesh"]["model"] = it.m_modelFilepath; // Save the model filepath
		}
		// If the object has a material component
		if (it.m_hasMat)
		{
			if (!it.m_hasText)
			{
				// Save the shader filepath, and material values
				objectArray[i]["material"]["shader"] = it.m_shaderFilepath;
				glm::vec3 diffuse = m_displayLayer->getMaterials().at(it.m_materialComponentVectorValue)->getDiffuse();
				glm::vec3 specular = m_displayLayer->getMaterials().at(it.m_materialComponentVectorValue)->getSpecular();
				objectArray[i]["material"]["material"]["diffuse"]["r"] = diffuse.x;
				objectArray[i]["material"]["material"]["diffuse"]["g"] = diffuse.y;
				objectArray[i]["material"]["material"]["diffuse"]["b"] = diffuse.z;
				objectArray[i]["material"]["material"]["specular"]["r"] = specular.x;
				objectArray[i]["material"]["material"]["specular"]["g"] = specular.y;
				objectArray[i]["material"]["material"]["specular"]["b"] = specular.z;
				objectArray[i]["material"]["material"]["shininess"] = m_displayLayer->getMaterials().at(it.m_materialComponentVectorValue)->getShininess();
			}
			else
			{
				objectArray[i]["material"]["shader"] = "text"; 
			}
		}

		glm::vec3 pos = m_displayLayer->getPositions().at(it.m_positionComponentVectorValue)->getPosition();
		glm::vec3 rot = m_displayLayer->getPositions().at(it.m_positionComponentVectorValue)->getRotation();
		rot = glm::vec3(glm::degrees(rot.x), glm::degrees(rot.y), glm::degrees(rot.z));
		glm::vec3 scale = m_displayLayer->getPositions().at(it.m_positionComponentVectorValue)->getScale();

		// If the object does not have a text component
		if (!it.m_hasText)
		{
			objectArray[i]["position"]["type"] = "values"; // The position component has values

			// Get all position data from the component and save it
			objectArray[i]["position"]["trans"]["x"] = pos.x;
			objectArray[i]["position"]["trans"]["y"] = pos.y;
			objectArray[i]["position"]["trans"]["z"] = pos.z;
			objectArray[i]["position"]["rot"]["x"] = rot.x;
			objectArray[i]["position"]["rot"]["y"] = rot.y;
			objectArray[i]["position"]["rot"]["z"] = rot.z;
			objectArray[i]["position"]["scale"]["x"] = scale.x;
			objectArray[i]["position"]["scale"]["y"] = scale.y;
			objectArray[i]["position"]["scale"]["z"] = scale.z;
		}
		else // If the object has a text component
		{
			objectArray[i]["position"]["type"] = "text"; // The postionion component uses text component values

			std::shared_ptr<Engine::TextLabel> text = m_displayLayer->getTexts().at(it.m_textComponentVectorValue)->getLabel();

			objectArray[i]["text"]["text"] = text->getText();
			objectArray[i]["text"]["font"] = text->getFont();
			objectArray[i]["text"]["charSize"] = text->getCharSize();
			objectArray[i]["text"]["position"]["x"] = pos.x;
			objectArray[i]["text"]["position"]["y"] = pos.y;
			objectArray[i]["text"]["rotation"] = rot.y;
			objectArray[i]["text"]["scale"] = scale.x;

			glm::vec3 colour = text->getColour();
			objectArray[i]["text"]["colour"]["r"] = colour.r;
			objectArray[i]["text"]["colour"]["g"] = colour.g;
			objectArray[i]["text"]["colour"]["b"] = colour.b;
		}

		// If the object has a light component
		if (it.m_hasLight)
		{
			// Get all light information
			glm::vec3 amb = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getAmbient();
			glm::vec3 pointDiff = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getPointDiffuse();
			glm::vec3 pointSpec = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getPointSpecular();
			glm::vec3 dirDiff = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getDirectionalDiffuse();
			glm::vec3 dirSpec = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getDirectionalSpecular();
			float att = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getAttenuation();

			// Save all light information
			objectArray[i]["light"]["ambient"]["r"] = amb.x;
			objectArray[i]["light"]["ambient"]["g"] = amb.y;
			objectArray[i]["light"]["ambient"]["b"] = amb.z;

			objectArray[i]["light"]["pointdiffuse"]["r"] = pointDiff.x;
			objectArray[i]["light"]["pointdiffuse"]["g"] = pointDiff.y;
			objectArray[i]["light"]["pointdiffuse"]["b"] = pointDiff.z;
			objectArray[i]["light"]["pointspecular"]["r"] = pointSpec.x;
			objectArray[i]["light"]["pointspecular"]["g"] = pointSpec.y;
			objectArray[i]["light"]["pointspecular"]["b"] = pointSpec.z;

			objectArray[i]["light"]["directionaldiffuse"]["r"] = dirDiff.x;
			objectArray[i]["light"]["directionaldiffuse"]["g"] = dirDiff.y;
			objectArray[i]["light"]["directionaldiffuse"]["b"] = dirDiff.z;
			objectArray[i]["light"]["directionalspecular"]["r"] = dirSpec.x;
			objectArray[i]["light"]["directionalspecular"]["g"] = dirSpec.y;
			objectArray[i]["light"]["directionalspecular"]["b"] = dirSpec.z;

			objectArray[i]["light"]["attenuation"] = att;
		}
		// If the object has a camera component
		if (it.m_hasCamera)
		{
			// Save all camera information
			objectArray[i]["camera"]["type"] = it.m_cameraType;
			std::shared_ptr<Engine::CameraComponent> camera = m_displayLayer->getCameras().at(it.m_cameraComponentVectorValue);

			if (it.m_in3DLayer)
			{
				objectArray[i]["camera"]["fov"] = camera->getFOV();
				objectArray[i]["camera"]["aspect"] = camera->getAspectRatio();
				objectArray[i]["camera"]["nearClip"] = camera->getNearClip();
				objectArray[i]["camera"]["farClip"] = camera->getFarClip();
				objectArray[i]["camera"]["updateMousePress"] = camera->getUpdateMousePress();
			}
			else
			{
				objectArray[i]["camera"]["left"] = 0.f;
				objectArray[i]["camera"]["top"] = 0.f;
				objectArray[i]["camera"]["width"] = 800.f;
				objectArray[i]["camera"]["height"] = 600.f;
			} 

			// Make and save a list of the layers in the level that the camera can see
			nlohmann::json layerList;
			for (auto& it : camera->getLayersSeen())
			{
				layerList.push_back(it);
			}
			objectArray[i]["camera"]["layers"] = layerList;
		}
		// If the object has a collider component
		if (it.m_hasCollider)
		{
			// Save the shape
			objectArray[i]["collider"]["shape"] = it.m_collider.m_shape;
			objectArray[i]["collider"]["scaleForSize"] = it.m_collider.m_useScaleForSize;
			// Depending on the shape of the collider, save the needed size information
			if (it.m_collider.m_shape.compare("box") == 0)
			{
				objectArray[i]["collider"]["halfExtents"]["x"] = it.m_collider.m_halfExtents.x;
				objectArray[i]["collider"]["halfExtents"]["y"] = it.m_collider.m_halfExtents.y;
				objectArray[i]["collider"]["halfExtents"]["z"] = it.m_collider.m_halfExtents.z;
			}
			else if (it.m_collider.m_shape.compare("sphere") == 0)
			{
				objectArray[i]["collider"]["radius"] = it.m_collider.m_radius;
			}
			else if (it.m_collider.m_shape.compare("capsule") == 0)
			{
				objectArray[i]["collider"]["radius"] = it.m_collider.m_radius;
				objectArray[i]["collider"]["height"] = it.m_collider.m_height;
			}
			else if (it.m_collider.m_shape.compare("convex mesh") == 0)
			{
				objectArray[i]["collider"]["model"] = it.m_collider.m_modelFilepath;
			}

			objectArray[i]["collider"]["mass"] = it.m_collider.m_mass;
			// Save which collision category the collider is in
			if (it.m_collider.m_category == 1)
				objectArray[i]["collider"]["collisionCategory"] = Engine::CollisionCategory::CATEGORY1;
			if (it.m_collider.m_category == 2)
				objectArray[i]["collider"]["collisionCategory"] = Engine::CollisionCategory::CATEGORY2;
			if (it.m_collider.m_category == 3)
				objectArray[i]["collider"]["collisionCategory"] = Engine::CollisionCategory::CATEGORY3;
			if (it.m_collider.m_category == 4)
				objectArray[i]["collider"]["collisionCategory"] = Engine::CollisionCategory::CATEGORY4;

			nlohmann::json canCollideWith; // Make and save a list of which collision categories the collider can collide with
			int used = 0;
			for (int j = 0; j < 4; j++)
			{
				if (it.m_collider.m_collideCat[j])
				{
					if (j == 0)
						canCollideWith[used] = Engine::CollisionCategory::CATEGORY1;
					if (j == 1)
						canCollideWith[used] = Engine::CollisionCategory::CATEGORY2;
					if (j == 2)
						canCollideWith[used] = Engine::CollisionCategory::CATEGORY3;
					if (j == 3)
						canCollideWith[used] = Engine::CollisionCategory::CATEGORY4;

					used++;
				}
			}
			objectArray[i]["collider"]["canCollideWith"] = canCollideWith;
		}
		// If the object has a rigidbody component
		if (it.m_hasRigidbody)
		{
			// Save rigidbody information
			objectArray[i]["rigidbody"]["type"] = it.m_rigidBody.m_type;
			objectArray[i]["rigidbody"]["gravity"] = it.m_rigidBody.m_gravity;
			objectArray[i]["rigidbody"]["material"]["bounciness"] = it.m_rigidBody.m_bounce;
			objectArray[i]["rigidbody"]["material"]["friction"] = it.m_rigidBody.m_friction;
			objectArray[i]["rigidbody"]["material"]["rollingResistance"] = it.m_rigidBody.m_rollResist;
		}
		// If the object has a texture component
		if (it.m_hasTexture)
		{
			if (!it.m_hasText)
			{
				// Save texture filepaths and information
				objectArray[i]["texture"]["diffuse"] = it.m_diffuseTextureFilepath;
				objectArray[i]["texture"]["specular"] = it.m_specularTextureFilepath;
				objectArray[i]["texture"]["normal"] = it.m_normalMapFilepath;
				objectArray[i]["texture"]["depth"] = it.m_depthMapFilepath;
				objectArray[i]["texture"]["heightScale"] = m_displayLayer->getTextures().at(it.m_textureComponentVectorValue)->getHeightScale();
			}
			else
			{
				objectArray[i]["texture"]["diffuse"] = "text";
			}
		}
		// If the object has a lua script component
		if (it.bHasLuaScript)
		{
			// Save the script filepath
			objectArray[i]["luascript"]["Filepath"] = it.m_LuaScript.m_sScriptFilePath;
		}
		// Save which layer the object is in
		objectArray[i]["layerName"] = it.m_layer;

		i++;
	}
	return i;
}

void UILayer::saveRunning()
{
	// Copy the default save control JSON into a new folder with a new name for use
	std::filesystem::copy("assets/savedJSON/Default/DefaultRunningControl.json", "assets/savedJSON/TempControl.json");

	std::vector<std::string>::iterator layer;

	// Iterate through all 3D layers in order to save each layer in a separate JSON file
	for (layer = m_displayLayer->m_layerData.m_3DLayers.begin(); layer != m_displayLayer->m_layerData.m_3DLayers.end(); ++layer)
	{
		saveLayerRunning(layer, false);
	}
	// Iterate through all 2D layers in order to save each layer in a separate JSON file
	for (layer = m_displayLayer->m_layerData.m_2DLayers.begin(); layer != m_displayLayer->m_layerData.m_2DLayers.end(); ++layer)
	{
		saveLayerRunning(layer, true);
	}
	

	std::fstream controlHandle("assets/savedJSON/TempControl.json", std::ios::in);
	if (!controlHandle.is_open()) {
		LOG_CRITICAL("Could not open file: {0}", "assets/savedJSON/TempControl.json");
	}
	// Load the temp control JSON file made above
	nlohmann::json saveControl;
	controlHandle >> saveControl;

	saveControl["Load"]["projectName"] = m_sProjectName; // Save the projects name

	nlohmann::json layersStart;
	layersStart["filepath"] = "";
	int numLayers = m_displayLayer->m_layerData.m_3DLayers.size() + m_displayLayer->m_layerData.m_2DLayers.size();
	nlohmann::json layersArray = nlohmann::json(numLayers, layersStart);

	auto& level = (*saveControl["Load"]["levels"].begin())["layers"];
	// Save a list of all layer names and filepaths
	int i = 0;
	for (auto& layer : m_displayLayer->m_layerData.m_3DLayers)
	{
		layersArray[i]["filepath"] = "assets/savedJSON/" + layer + ".json";
		layersArray[i]["name"] = layer;

		i++;
	}
	for (auto& layer : m_displayLayer->m_layerData.m_2DLayers)
	{
		layersArray[i]["filepath"] = "assets/savedJSON/" + layer + ".json";
		layersArray[i]["name"] = layer;
	
		i++;
	}
	level = layersArray;
	(*saveControl["Load"]["levels"].begin())["name"] = m_displayLayer->m_layerData.m_name;
	// Save the changed JSON file
	std::ofstream controlFile("assets/savedJSON/TempControl.json");
	controlFile << saveControl;
}

void UILayer::saveLayerRunning(std::vector<std::string>::iterator& layer, bool UI)
{
	// Copy the default save layer JSON into a new folder with a new name for use
	if (!UI)
		std::filesystem::copy("assets/savedJSON/Default/Default3DSaveLayer.json", "assets/savedJSON/" + (*layer) + ".json");
	else
		std::filesystem::copy("assets/savedJSON/Default/Default2DSaveLayer.json", "assets/savedJSON/" + (*layer) + ".json");

	std::fstream handle("assets/savedJSON/" + (*layer) + ".json", std::ios::in);
	if (!handle.is_open()) {
		LOG_CRITICAL("Could not open file: {0}", "assets/savedJSON/" + (*layer) + ".json");
	}

	std::vector<Object> objectsInLayer;
	std::vector<Joint> jointsInLayer;

	if (!UI)
	{
		// Make a list of all objects in the layer
		for (auto& object : m_displayLayer->m_layerData.m_objects)
		{
			if (object.m_layer == *layer)
			{
				objectsInLayer.push_back(object);
			}
		}

		// Make a list of all joints in the layer
		for (auto& joint : m_displayLayer->m_layerData.m_joints)
		{
			if (joint.m_layer == *layer)
			{
				jointsInLayer.push_back(joint);
			}
		}
	}
	else
	{
		// Make a list of all objects in the layer
		for (auto& object : m_displayLayer->m_layerData.m_2DObjects)
		{
			if (object.m_layer == *layer)
			{
				objectsInLayer.push_back(object);
			}
		}
	}

	nlohmann::json saveJSON;
	handle >> saveJSON; // Get the JSON file to be changed

	std::vector<std::string> modelFilepaths;
	std::vector<std::string> textureFilepaths;
	std::vector<std::string> shaderFilepaths;
	std::vector<std::pair<int, std::string>> fonts;
	int i = 0;

	int numMaterials = 0;
	int numLights = 0;
	int numCameras = 0;
	int numColliders = 0;
	int numRigidbodies = 0;
	int numLuaScripts = 0;
	int numTextures = 0;
	int numMeshes = 0;
	int numTexts = 0;

	int meshLength = 0;
	int textureLength = 0;
	int textLength = 0;
	int numDifferentShaders = 0;

	// For each object in the layer
	for (auto& it : objectsInLayer)
	{
		// If it has a mesh component
		if (it.m_hasMesh)
		{
			// Increase the number of meshes in the layer and if the filepath is not in the vector, add it
			std::vector<std::string>::iterator it2 = std::find(modelFilepaths.begin(), modelFilepaths.end(), it.m_modelFilepath);
			if (it2 == modelFilepaths.end())
			{
				modelFilepaths.push_back(it.m_modelFilepath);
				meshLength++;
			}
			numMeshes++;
		}
		// If the object has a component, increase the number of that component in the level
		if (it.m_hasMat)
		{
			std::string shader = MakeShader::makeShader(it.m_hasNormalMap, it.m_hasDepthMap);
			it.m_tempShaderFilepath = shader;

			std::vector<std::string>::iterator it2 = std::find(shaderFilepaths.begin(), shaderFilepaths.end(), shader);
			if (it2 == shaderFilepaths.end())
			{
				shaderFilepaths.push_back(shader);
				numDifferentShaders++;
			}

			numMaterials++;
		}
		if (it.m_hasLight)
			numLights++;
		if (it.m_hasCamera)
			numCameras++;
		if (it.m_hasCollider)
			numColliders++;
		if (it.m_hasRigidbody)
			numRigidbodies++;
		if (it.bHasLuaScript)
			numLuaScripts++;

		// If it has a text component
		if (it.m_hasText)
		{
			std::pair<int, std::string> font(it.m_textCharSize, "assets/fonts/" + it.m_textFont + ".ttf");

			// Add font filepaths and sizes to the vector
			std::vector<std::pair<int, std::string>>::iterator it2 = std::find(fonts.begin(), fonts.end(), font);
			if (it2 == fonts.end())
			{
				fonts.push_back(font);
				textLength++;
			}

			numTexts++;
		}

		// If the object has a texture component
		if (it.m_hasTexture)
		{
			if (!UI)
			{
				// Add any new texture filepaths to the vector
				std::vector<std::string>::iterator it2 = std::find(textureFilepaths.begin(), textureFilepaths.end(), it.m_diffuseTextureFilepath);
				if (it2 == textureFilepaths.end())
				{
					textureFilepaths.push_back(it.m_diffuseTextureFilepath);
					textureLength++;
				}

				it2 = std::find(textureFilepaths.begin(), textureFilepaths.end(), it.m_specularTextureFilepath);
				if (it2 == textureFilepaths.end())
				{
					textureFilepaths.push_back(it.m_specularTextureFilepath);
					textureLength++;
				}

				it2 = std::find(textureFilepaths.begin(), textureFilepaths.end(), it.m_normalMapFilepath);
				if (it2 == textureFilepaths.end())
				{
					textureFilepaths.push_back(it.m_normalMapFilepath);
					textureLength++;
				}

				it2 = std::find(textureFilepaths.begin(), textureFilepaths.end(), it.m_depthMapFilepath);
				if (it2 == textureFilepaths.end())
				{
					textureFilepaths.push_back(it.m_depthMapFilepath);
					textureLength++;
				}
			}
			numTextures++;
		}
	}
	nlohmann::json modelStart;
	modelStart["filepath"] = "filepath";
	nlohmann::json modelArray = nlohmann::json(meshLength, modelStart);
	// Make a list containing all the model filepaths in the layer
	i = 0;
	for (auto& it : modelArray)
	{
		it["filepath"] = modelFilepaths[i];
		i++;
	}
	saveJSON["Asyncload"]["models"] = modelArray;

	if (UI) // If it's a UI layer
	{
		nlohmann::json fontStart;
		fontStart["filepath"] = "filepath";
		nlohmann::json fontArray = nlohmann::json(textLength, fontStart);
		// Make a list containing all the font filepaths and sizes in the layer
		i = 0;
		for (auto& it : fontArray)
		{
			it["charSize"] = fonts[i].first;
			it["filepath"] = fonts[i].second;
			i++;
		}
		saveJSON["Asyncload"]["fonts"] = fontArray;
	}
	else
	{
		nlohmann::json textureStart;
		textureStart["filepath"] = "filepath";
		nlohmann::json textureArray = nlohmann::json(textureLength, textureStart);
		// Make a list containing all the texture filepaths in the layer
		i = 0;
		for (auto& it : textureArray)
		{
			it["filepath"] = textureFilepaths[i];
			i++;
		}
		saveJSON["Asyncload"]["textures"] = textureArray;


		nlohmann::json shaderStart;
		shaderStart["filepath"] = "filepath";
		nlohmann::json shaderArray = nlohmann::json(numDifferentShaders, shaderStart);
		// Make a list containing all the shader filepaths in the layer
		i = 0;
		for (auto& it : shaderArray)
		{
			it["filepath"] = shaderFilepaths[i];
			i++;
		}
		saveJSON["Asyncload"]["shaders"] = shaderArray;
	}

	// Set the memory information
	auto& memInfo = saveJSON["MemoryInfo"];
	memInfo["gameObjects"] = objectsInLayer.size();
	memInfo["materials"] = numMaterials;
	memInfo["mesh"] = numMeshes;
	memInfo["position"] = objectsInLayer.size();
	memInfo["lights"] = numLights;
	memInfo["cameras"] = numCameras;
	memInfo["colliders"] = numColliders;
	memInfo["rigidbody"] = numRigidbodies;
	memInfo["luascript"] = numLuaScripts;
	memInfo["texture"] = numTextures;
	memInfo["joints"] = jointsInLayer.size();
	memInfo["text"] = numTexts;

	auto& objects = saveJSON["GameObjects"];

	nlohmann::json objectStart;
	objectStart["name"] = "Object";
	nlohmann::json objectArray = nlohmann::json(objectsInLayer.size(), objectStart); // Get the array for all the objects in the layer

	// For each object
	i = 0;
	for (auto& it : objectsInLayer)
	{
		// Save the objects name, index value and tag
		objectArray[i]["index"] = it.m_objectVectorValue;

		if (!UI)
			objectArray[i]["name"] = m_displayLayer->getGameObjects().at(it.m_objectVectorValue)->name();
		else
			objectArray[i]["name"] = m_displayLayer->get2DGameObjects().at(it.m_objectVectorValue)->name();

		objectArray[i]["tag"] = it.m_tag;

		// If the object has a mesh component
		if (it.m_hasMesh)
		{
			objectArray[i]["mesh"]["model"] = it.m_modelFilepath; // Save the model filepath
		}
		// If the object has a material component
		if (it.m_hasMat)
		{
			if (!it.m_hasText)
			{
				// Save the shader filepath, and material values
				objectArray[i]["material"]["shader"] = it.m_tempShaderFilepath;
				glm::vec3 diffuse = m_displayLayer->getMaterials().at(it.m_materialComponentVectorValue)->getDiffuse();
				glm::vec3 specular = m_displayLayer->getMaterials().at(it.m_materialComponentVectorValue)->getSpecular();
				objectArray[i]["material"]["material"]["diffuse"]["r"] = diffuse.x;
				objectArray[i]["material"]["material"]["diffuse"]["g"] = diffuse.y;
				objectArray[i]["material"]["material"]["diffuse"]["b"] = diffuse.z;
				objectArray[i]["material"]["material"]["specular"]["r"] = specular.x;
				objectArray[i]["material"]["material"]["specular"]["g"] = specular.y;
				objectArray[i]["material"]["material"]["specular"]["b"] = specular.z;
				objectArray[i]["material"]["material"]["shininess"] = m_displayLayer->getMaterials().at(it.m_materialComponentVectorValue)->getShininess();
			}
			else
			{
				objectArray[i]["material"]["shader"] = "text";
			}
		}

		glm::vec3 pos = m_displayLayer->getPositions().at(it.m_positionComponentVectorValue)->getPosition();
		glm::vec3 rot = m_displayLayer->getPositions().at(it.m_positionComponentVectorValue)->getRotation();
		rot = glm::vec3(glm::degrees(rot.x), glm::degrees(rot.y), glm::degrees(rot.z));
		glm::vec3 scale = m_displayLayer->getPositions().at(it.m_positionComponentVectorValue)->getScale();

		// If the object does not have a text component
		if (!it.m_hasText)
		{
			objectArray[i]["position"]["type"] = "values"; // The position component has values

			// Get all position data from the component and save it
			objectArray[i]["position"]["trans"]["x"] = pos.x;
			objectArray[i]["position"]["trans"]["y"] = pos.y;
			objectArray[i]["position"]["trans"]["z"] = pos.z;
			objectArray[i]["position"]["rot"]["x"] = rot.x;
			objectArray[i]["position"]["rot"]["y"] = rot.y;
			objectArray[i]["position"]["rot"]["z"] = rot.z;
			objectArray[i]["position"]["scale"]["x"] = scale.x;
			objectArray[i]["position"]["scale"]["y"] = scale.y;
			objectArray[i]["position"]["scale"]["z"] = scale.z;
		}
		else // If the object has a text component
		{
			objectArray[i]["position"]["type"] = "text"; // The postionion component uses text component values

			std::shared_ptr<Engine::TextLabel> text = m_displayLayer->getTexts().at(it.m_textComponentVectorValue)->getLabel();

			objectArray[i]["text"]["text"] = text->getText();
			objectArray[i]["text"]["font"] = text->getFont();
			objectArray[i]["text"]["charSize"] = text->getCharSize();
			objectArray[i]["text"]["position"]["x"] = pos.x;
			objectArray[i]["text"]["position"]["y"] = pos.y;
			objectArray[i]["text"]["rotation"] = rot.y;
			objectArray[i]["text"]["scale"] = scale.x;

			glm::vec3 colour = text->getColour();
			objectArray[i]["text"]["colour"]["r"] = colour.r;
			objectArray[i]["text"]["colour"]["g"] = colour.g;
			objectArray[i]["text"]["colour"]["b"] = colour.b;
		}

		// If the object has a light component
		if (it.m_hasLight)
		{
			// Get all light information
			glm::vec3 amb = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getAmbient();
			glm::vec3 pointDiff = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getPointDiffuse();
			glm::vec3 pointSpec = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getPointSpecular();
			glm::vec3 dirDiff = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getDirectionalDiffuse();
			glm::vec3 dirSpec = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getDirectionalSpecular();
			float att = m_displayLayer->getLights().at(it.m_lightComponentVectorValue)->getAttenuation();

			// Save all light information
			objectArray[i]["light"]["ambient"]["r"] = amb.x;
			objectArray[i]["light"]["ambient"]["g"] = amb.y;
			objectArray[i]["light"]["ambient"]["b"] = amb.z;

			objectArray[i]["light"]["pointdiffuse"]["r"] = pointDiff.x;
			objectArray[i]["light"]["pointdiffuse"]["g"] = pointDiff.y;
			objectArray[i]["light"]["pointdiffuse"]["b"] = pointDiff.z;
			objectArray[i]["light"]["pointspecular"]["r"] = pointSpec.x;
			objectArray[i]["light"]["pointspecular"]["g"] = pointSpec.y;
			objectArray[i]["light"]["pointspecular"]["b"] = pointSpec.z;

			objectArray[i]["light"]["directionaldiffuse"]["r"] = dirDiff.x;
			objectArray[i]["light"]["directionaldiffuse"]["g"] = dirDiff.y;
			objectArray[i]["light"]["directionaldiffuse"]["b"] = dirDiff.z;
			objectArray[i]["light"]["directionalspecular"]["r"] = dirSpec.x;
			objectArray[i]["light"]["directionalspecular"]["g"] = dirSpec.y;
			objectArray[i]["light"]["directionalspecular"]["b"] = dirSpec.z;

			objectArray[i]["light"]["attenuation"] = att;
		}
		// If the object has a camera component
		if (it.m_hasCamera)
		{
			// Save all camera information
			objectArray[i]["camera"]["type"] = it.m_cameraType;
			std::shared_ptr<Engine::CameraComponent> camera = m_displayLayer->getCameras().at(it.m_cameraComponentVectorValue);

			if (it.m_in3DLayer)
			{
				objectArray[i]["camera"]["fov"] = camera->getFOV();
				objectArray[i]["camera"]["aspect"] = camera->getAspectRatio();
				objectArray[i]["camera"]["nearClip"] = camera->getNearClip();
				objectArray[i]["camera"]["farClip"] = camera->getFarClip();
				objectArray[i]["camera"]["updateMousePress"] = camera->getUpdateMousePress();
			}
			else
			{
				objectArray[i]["camera"]["left"] = 0.f;
				objectArray[i]["camera"]["top"] = 0.f;
				objectArray[i]["camera"]["width"] = 800.f;
				objectArray[i]["camera"]["height"] = 600.f;
			}

			// Save the list of which layers the camera can see
			nlohmann::json layerList;
			for (auto& it : camera->getLayersSeen())
			{
				layerList.push_back(it);
			}
			objectArray[i]["camera"]["layers"] = layerList;
		}
		// If the object has a collider component
		if (it.m_hasCollider)
		{
			// Save the shape and size of the collider
			objectArray[i]["collider"]["shape"] = it.m_collider.m_shape;
			if (it.m_collider.m_shape.compare("box") == 0)
			{
				objectArray[i]["collider"]["halfExtents"]["x"] = it.m_collider.m_halfExtents.x;
				objectArray[i]["collider"]["halfExtents"]["y"] = it.m_collider.m_halfExtents.y;
				objectArray[i]["collider"]["halfExtents"]["z"] = it.m_collider.m_halfExtents.z;
			}
			else if (it.m_collider.m_shape.compare("sphere") == 0)
			{
				objectArray[i]["collider"]["radius"] = it.m_collider.m_radius;
			}
			else if (it.m_collider.m_shape.compare("capsule") == 0)
			{
				objectArray[i]["collider"]["radius"] = it.m_collider.m_radius;
				objectArray[i]["collider"]["height"] = it.m_collider.m_height;
			}
			else if (it.m_collider.m_shape.compare("convex mesh") == 0)
			{
				objectArray[i]["collider"]["model"] = it.m_collider.m_modelFilepath;
			}

			objectArray[i]["collider"]["mass"] = it.m_collider.m_mass;

			// Save the colliders collision category
			if (it.m_collider.m_category == 1)
				objectArray[i]["collider"]["collisionCategory"] = Engine::CollisionCategory::CATEGORY1;
			if (it.m_collider.m_category == 2)
				objectArray[i]["collider"]["collisionCategory"] = Engine::CollisionCategory::CATEGORY2;
			if (it.m_collider.m_category == 3)
				objectArray[i]["collider"]["collisionCategory"] = Engine::CollisionCategory::CATEGORY3;
			if (it.m_collider.m_category == 4)
				objectArray[i]["collider"]["collisionCategory"] = Engine::CollisionCategory::CATEGORY4;

			// Save a list of which categories the collider can collide with
			nlohmann::json canCollideWith;
			int used = 0;
			for (int j = 0; j < 4; j++)
			{
				if (it.m_collider.m_collideCat[j])
				{
					if (j == 0)
						canCollideWith[used] = Engine::CollisionCategory::CATEGORY1;
					if (j == 1)
						canCollideWith[used] = Engine::CollisionCategory::CATEGORY2;
					if (j == 2)
						canCollideWith[used] = Engine::CollisionCategory::CATEGORY3;
					if (j == 3)
						canCollideWith[used] = Engine::CollisionCategory::CATEGORY4;

					used++;
				}
			}
			objectArray[i]["collider"]["canCollideWith"] = canCollideWith;
		}
		// If the object has a rigidbody component
		if (it.m_hasRigidbody)
		{
			// Save rigidbody information
			objectArray[i]["rigidbody"]["type"] = it.m_rigidBody.m_type;
			objectArray[i]["rigidbody"]["gravity"] = it.m_rigidBody.m_gravity;
			objectArray[i]["rigidbody"]["material"]["bounciness"] = it.m_rigidBody.m_bounce;
			objectArray[i]["rigidbody"]["material"]["friction"] = it.m_rigidBody.m_friction;
			objectArray[i]["rigidbody"]["material"]["rollingResistance"] = it.m_rigidBody.m_rollResist;
		}
		// If the object has a lua script component
		if (it.bHasLuaScript)
		{
			// Save the script filepath
			objectArray[i]["luascript"]["Filepath"] = it.m_LuaScript.m_sScriptFilePath;
		}
		// If the object has a texture component
		if (it.m_hasTexture)
		{
			if (!it.m_hasText)
			{
				// Save the texture filepaths
				objectArray[i]["texture"]["diffuse"] = it.m_diffuseTextureFilepath;
				objectArray[i]["texture"]["specular"] = it.m_specularTextureFilepath;
				objectArray[i]["texture"]["normal"] = it.m_normalMapFilepath;
				objectArray[i]["texture"]["depth"] = it.m_depthMapFilepath;
				objectArray[i]["texture"]["heightScale"] = m_displayLayer->getTextures().at(it.m_textureComponentVectorValue)->getHeightScale();
			}
			else
			{
				objectArray[i]["texture"]["diffuse"] = "text";
			}
		}

		i++;
	}
	objects = objectArray;

	if (!UI)
	{
		auto& joints = saveJSON["Joints"];

		nlohmann::json jointStart;
		jointStart["type"] = "";
		nlohmann::json jointArray = nlohmann::json(jointsInLayer.size(), jointStart); // Get the array of joints in the layer

		i = 0;
		for (auto& it : jointsInLayer)
		{
			// Save all joint information
			jointArray[i]["type"] = it.m_type;
			jointArray[i]["anchorpoint"]["x"] = it.m_position.x;
			jointArray[i]["anchorpoint"]["y"] = it.m_position.y;
			jointArray[i]["anchorpoint"]["z"] = it.m_position.z;
			jointArray[i]["bodyA"] = it.m_bodyAName;
			jointArray[i]["bodyB"] = it.m_bodyBName;
			jointArray[i]["canCollide"] = it.m_bBodiesCanCollide;
			jointArray[i]["isLimited"] = it.m_bIsLimited;
			jointArray[i]["maxLimit"] = it.m_fMaxLimit;
			jointArray[i]["minLimit"] = it.m_fMinLimit;
			jointArray[i]["axis"]["x"] = it.m_axis.x;
			jointArray[i]["axis"]["y"] = it.m_axis.y;
			jointArray[i]["axis"]["z"] = it.m_axis.z;

			i++;
		}
		joints = jointArray;
	}

	if (!UI)
	{
		// If the layer is the last in the vector
		if (layer == m_displayLayer->m_layerData.m_3DLayers.begin())
		{
			// Save come render commands needed
			nlohmann::json command;
			command["type"] = "ClearDepthColourBuffer";

			saveJSON["RendererCommands"]["predrawLayerTexture"].push_back(command);

			saveJSON["RendererCommands"]["predrawFramebuffer"].push_back(command);
		}
	}

	saveJSON["3DLayer"] = !UI;

	// Save the changed JSON file
	std::ofstream file("assets/savedJSON/" + (*layer) + ".json");
	file << saveJSON;
}

void UILayer::DeleteObject(Object& it)
{
	bool UI = !it.m_in3DLayer;

	// Remove all components that the object was using
	m_displayLayer->getPositions().erase(m_displayLayer->getPositions().begin() + it.m_positionComponentVectorValue);
	if (it.m_hasMesh)
		m_displayLayer->getMeshes().erase(m_displayLayer->getMeshes().begin() + it.m_meshComponentVectorValue);
	if (it.m_hasMat)
		m_displayLayer->getMaterials().erase(m_displayLayer->getMaterials().begin() + it.m_materialComponentVectorValue);
	if (it.m_hasTexture)
		m_displayLayer->getTextures().erase(m_displayLayer->getTextures().begin() + it.m_textureComponentVectorValue);
	if (it.m_hasText)
		m_displayLayer->getTexts().erase(m_displayLayer->getTexts().begin() + it.m_textComponentVectorValue);

	if (!UI)
		m_displayLayer->getGameObjects().erase(m_displayLayer->getGameObjects().begin() + it.m_objectVectorValue);
	else
		m_displayLayer->get2DGameObjects().erase(m_displayLayer->get2DGameObjects().begin() + it.m_objectVectorValue);

	std::vector<Object>* pObjects = &m_displayLayer->m_layerData.m_objects;

	// Decrease component vector values for all objects that had higher values than the one being deleted
	for (auto& it2 : *pObjects)
	{
		if (it2.m_positionComponentVectorValue > it.m_positionComponentVectorValue)
			it2.m_positionComponentVectorValue--;
		if (it.m_hasMesh)
		{
			if (it2.m_meshComponentVectorValue > it.m_meshComponentVectorValue)
				it2.m_meshComponentVectorValue--;
		}
		if (it.m_hasMat)
		{
			if (it2.m_materialComponentVectorValue > it.m_materialComponentVectorValue)
				it2.m_materialComponentVectorValue--;
		}
		if (it.m_hasTexture)
		{
			if (it2.m_textureComponentVectorValue > it.m_textureComponentVectorValue)
				it2.m_textureComponentVectorValue--;
		}
		if (it.m_hasText)
		{
			if (it2.m_textComponentVectorValue > it.m_textComponentVectorValue)
				it2.m_textComponentVectorValue--;
		}
		if (!UI)
		{
			if (it2.m_objectVectorValue > it.m_objectVectorValue)
				it2.m_objectVectorValue--;
		}
	}

	pObjects = &m_displayLayer->m_layerData.m_2DObjects;

	for (auto& it2 : *pObjects)
	{
		if (it2.m_positionComponentVectorValue > it.m_positionComponentVectorValue)
			it2.m_positionComponentVectorValue--;
		if (it.m_hasMesh)
		{
			if (it2.m_meshComponentVectorValue > it.m_meshComponentVectorValue)
				it2.m_meshComponentVectorValue--;
		}
		if (it.m_hasMat)
		{
			if (it2.m_materialComponentVectorValue > it.m_materialComponentVectorValue)
				it2.m_materialComponentVectorValue--;
		}
		if (it.m_hasTexture)
		{
			if (it2.m_textureComponentVectorValue > it.m_textureComponentVectorValue)
				it2.m_textureComponentVectorValue--;
		}
		if (it.m_hasText)
		{
			if (it2.m_textComponentVectorValue > it.m_textComponentVectorValue)
				it2.m_textComponentVectorValue--;
		}
		if (UI)
		{
			if (it2.m_objectVectorValue > it.m_objectVectorValue)
				it2.m_objectVectorValue--;
		}
	}

	// If the object had a collider and a rigidbody, decrease the number of these
	if (it.m_hasCollider)
		m_displayLayer->m_layerData.m_numColliders--;
	if (it.m_hasRigidbody)
		m_displayLayer->m_layerData.m_numRigidbodies--;

	m_iCurrentObjectSelected = 0;
	m_bComponentsShowing = false;

	if (!UI)
		pObjects = &m_displayLayer->m_layerData.m_objects;

	// Erase the object from the vector
	pObjects->erase(pObjects->begin() + it.m_objectVectorValue);
	ImGui::End();
}

void UILayer::DeleteJoint(Joint& it)
{
	// Decrease the joint vector value of all joints with a higher value than the one being deleted
	for (auto& it2 : m_displayLayer->m_layerData.m_joints)
	{
		if (it2.m_jointVectorValue > it2.m_jointVectorValue)
			it2.m_jointVectorValue--;
	}
	// Erase the joint from the vector
	m_displayLayer->m_layerData.m_joints.erase(m_displayLayer->m_layerData.m_joints.begin() + it.m_jointVectorValue);
	ImGui::End();
}

void UILayer::loadAssets()
{
	// Load models in a separate thread
	std::thread modelThread(&UILayer::loadModels, this);

	loadTextures();

	modelThread.join();
}

void UILayer::loadTextures()
{
	// Search through the common texture file and the one for this project and load all textures in them
	std::string texFilePath = "assets/textures/";
	for (const auto& entry : std::filesystem::directory_iterator(texFilePath))
	{
		m_displayLayer->getResources()->addTexture(entry.path().string());
	}
	texFilePath = "assets/savedJSON/" + m_sProjectName + "/textures/";
	for (const auto& entry : std::filesystem::directory_iterator(texFilePath))
	{
		std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
		if (fileType.compare(".txt") != 0)
			m_displayLayer->getResources()->addTexture(entry.path().string());
	}
}

void UILayer::loadModels()
{
	m_displayLayer->m_layerData.m_numDifferentModels = 0;
	// Load models on separate threads
	std::vector<std::thread> threads;

	std::string modelsFilePath = "assets/models/";
	for (const auto& entry : std::filesystem::directory_iterator(modelsFilePath))
	{
		std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());

		if (fileType.compare(".mtl") != 0)
		{
			threads.push_back(std::thread(&Engine::ResourceManager::addModel, entry.path().string()));
			m_displayLayer->m_layerData.m_numDifferentModels++;
		}
	}
	modelsFilePath = "assets/savedJSON/" + m_sProjectName + "/models/";
	for (const auto& entry : std::filesystem::directory_iterator(modelsFilePath))
	{
		std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());

		if (fileType.compare(".txt") != 0 && fileType.compare(".mtl") != 0)
		{
			threads.push_back(std::thread(&Engine::ResourceManager::addModel, entry.path().string()));
			m_displayLayer->m_layerData.m_numDifferentModels++;
		}
	}

	for (auto& thread : threads)
	{
		thread.join();
	}
}
