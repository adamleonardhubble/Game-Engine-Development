/** \file changeObjects.cpp
*/
#include <Engine.h>

#include "changeObjects.h"

#include <filesystem>
#include <glad/glad.h>

std::shared_ptr<DisplayLayer> ChangeObjects::s_displayLayer = nullptr;
std::string ChangeObjects::s_sProjectName;
bool ChangeObjects::bShowLuaHelpWindow = false;

// Make a New Game Object
void ChangeObjects::makeGameObject(const std::string& name, std::shared_ptr<Engine::GameObject>& object, bool UI)
{
	std::vector<Object>* pObjects;

	if (!UI)
	{
		int numberOfObjects = s_displayLayer->getGameObjects().size();
		// Create a new GameObject in the GameObject Vector
		s_displayLayer->getGameObjects().push_back(std::make_shared<Engine::GameObject>(Engine::GameObject(name, "", numberOfObjects)));

		// Set a Pointer to the Object
		object = s_displayLayer->getGameObjects().back();

		pObjects = &s_displayLayer->m_layerData.m_objects;
		// Add Object to Objects Vector
		pObjects->push_back(Object(s_displayLayer->getGameObjects().size() - 1));
		pObjects->back().m_layer = s_displayLayer->m_layerData.m_3DLayers.front();
	}
	else
	{
		int numberOfObjects = s_displayLayer->get2DGameObjects().size();
		// Create a new GameObject in the GameObject Vector
		s_displayLayer->get2DGameObjects().push_back(std::make_shared<Engine::GameObject>(Engine::GameObject(name, "", numberOfObjects)));

		// Set a Pointer to the Object
		object = s_displayLayer->get2DGameObjects().back();

		pObjects = &s_displayLayer->m_layerData.m_2DObjects;
		// Add Object to Objects Vector
		pObjects->push_back(Object(s_displayLayer->get2DGameObjects().size() - 1));
		pObjects->back().m_layer = s_displayLayer->m_layerData.m_2DLayers.front();
		pObjects->back().m_in3DLayer = false;

		std::string text = "New Text";
		std::string font = "arial_narrow_7";
		int charSize = 32;

		pObjects->back().m_tempTexData.m_font = font;
		pObjects->back().m_tempTexData.m_size = charSize;
		strcpy(pObjects->back().m_tempTexData.m_text, text.c_str());

		// Make the text label
		std::shared_ptr<Engine::TextLabel> label;
		label.reset(Engine::TextLabel::create(font, charSize, text, glm::vec2(250.f, 150.f), 0.f, 1.f, glm::vec3(1.f, 1.f, 1.f)));
		// Make a compoent with the text label and add it to the gameobject
		s_displayLayer->getTexts().push_back(std::make_shared<Engine::TextComponent>(Engine::TextComponent(label)));
		pObjects->back().m_hasText = true;
		pObjects->back().m_textComponentVectorValue = s_displayLayer->getTexts().size() - 1;
		object->addComponent(s_displayLayer->getTexts().back());

		pObjects->back().m_textFont = font;
		pObjects->back().m_textCharSize = charSize;

		addMaterial("", object, true);
		addPosition(object, true);
		addTexture(object, true);
	}
}

// Add Components
// Add a Position Component
void ChangeObjects::addPosition(std::shared_ptr<Engine::GameObject>& object, bool UI)
{
	if (!UI)
	{
		// Set Deault Transform Information
		glm::vec3 translation(0.f, 0.f, 0.f); // Set Default Position
		glm::vec3 rotation(0.f, 0.f, 0.f); // Set Default Rotation
		glm::vec3 scale(1.f, 1.f, 1.f); // Set Default Scale

		// Add the Position Component to the PositionComponent Vector
		s_displayLayer->getPositions().push_back(std::make_shared<Engine::PositionComponent>(Engine::PositionComponent(translation, rotation, scale)));

		// Add the Position Component to the Object
		object->addComponent(s_displayLayer->getPositions().back()); // Add the Component
		s_displayLayer->m_layerData.m_objects.back().m_positionComponentVectorValue = s_displayLayer->getPositions().size() - 1; // Get Object Positon Component Index
	}
	else
	{
		s_displayLayer->getPositions().push_back(std::make_shared<Engine::PositionComponent>(Engine::PositionComponent(
			glm::vec3(s_displayLayer->getTexts().back()->getLabel()->getPosition(), 0),
			glm::vec3(0, s_displayLayer->getTexts().back()->getLabel()->getOrientation(), 0),
			glm::vec3(s_displayLayer->getTexts().back()->getLabel()->getScale()))));

		// Add the Position Component to the Object
		object->addComponent(s_displayLayer->getPositions().back()); // Add the Component
		s_displayLayer->m_layerData.m_2DObjects.back().m_positionComponentVectorValue = s_displayLayer->getPositions().size() - 1; // Get Object Positon Component Index
	}

	
}

void ChangeObjects::addMesh(const std::string& modelFilepath, std::shared_ptr<Engine::GameObject>& object)
{
	// Get the model from the resource manager
	std::shared_ptr<Engine::Model> model = s_displayLayer->getResources()->addModel(modelFilepath);
	// Add the mesh component to the components in the layer and to the object
	s_displayLayer->getMeshes().push_back(std::make_shared<Engine::MeshComponent>(Engine::MeshComponent(model)));
	object->addComponent(s_displayLayer->getMeshes().back());
	// Set mesh data
	s_displayLayer->m_layerData.m_objects.back().m_meshComponentVectorValue = s_displayLayer->getMeshes().size() - 1;
	s_displayLayer->m_layerData.m_objects.back().m_modelFilepath = modelFilepath;
	s_displayLayer->m_layerData.m_objects.back().m_hasMesh = true;
}
// Add a Material Component
void ChangeObjects::addMaterial(const std::string& name, std::shared_ptr<Engine::GameObject>& object, bool UI)
{
	if (!UI)
	{
		std::shared_ptr<Engine::Model> model = s_displayLayer->getMeshes().back()->getModel();
		std::shared_ptr<Engine::Shader> shader = s_displayLayer->getResources()->addShader("assets/shaders/lighting.glsl");

		// Create the material
		std::shared_ptr<Engine::VertexArray> VAO = s_displayLayer->getResources()->addVAO(s_displayLayer->m_layerData.m_objects.back().m_modelFilepath);
		VAO->setVertexBuffer(s_displayLayer->getResources()->addVBO(s_displayLayer->m_layerData.m_objects.back().m_modelFilepath, model->vertices, sizeof(float) * model->verticesSize, shader->getBufferLayout()));
		VAO->setIndexBuffer(s_displayLayer->getResources()->addIndexBuffer(s_displayLayer->m_layerData.m_objects.back().m_modelFilepath, model->indices, model->indicesSize));
		std::shared_ptr<Engine::Material> mat = s_displayLayer->getResources()->addMaterial(name + std::to_string(s_displayLayer->m_layerData.m_objects.back().m_objectVectorValue), shader, VAO);

		// Create a component with the material
		s_displayLayer->getMaterials().push_back(std::make_shared<Engine::MaterialComponent>(Engine::MaterialComponent(mat)));
		float dr = 1.0f;
		float dg = 1.0f;
		float db = 1.0f;
		float sr = 1.0f;
		float sg = 1.0f;
		float sb = 1.0f;
		float shininess = 32.f;

		// Set default values on the material component
		s_displayLayer->getMaterials().back()->setDiffuse(glm::vec3(dr, dg, db));
		s_displayLayer->getMaterials().back()->setSpecular(glm::vec3(sr, sg, sb));
		s_displayLayer->getMaterials().back()->setShininess(shininess);

		// Add the component to the object
		object->addComponent(s_displayLayer->getMaterials().back());
		s_displayLayer->m_layerData.m_objects.back().m_materialComponentVectorValue = s_displayLayer->getMaterials().size() - 1;

		// Finish Initializing Component
		s_displayLayer->m_layerData.m_objects.back().m_shaderFilepath = "assets/shaders/lighting.glsl";
		s_displayLayer->m_layerData.m_objects.back().m_hasMat = true; // Mark Object as Having a Material
	}
	else
	{
		auto mat = s_displayLayer->getTexts().back()->getLabel()->getMaterial();
		// Set the colour uniform
		mat->setDataElement("u_fontColour", (void*)&s_displayLayer->getTexts().back()->getLabel()->getColour()[0]);
		// Make a component with the material and add it to the object
		s_displayLayer->getMaterials().push_back(std::make_shared<Engine::MaterialComponent>(Engine::MaterialComponent(mat)));
		object->addComponent(s_displayLayer->getMaterials().back());

		s_displayLayer->m_layerData.m_2DObjects.back().m_materialComponentVectorValue = s_displayLayer->getMaterials().size() - 1;

		// Finish Initializing Component
		s_displayLayer->m_layerData.m_2DObjects.back().m_shaderFilepath = "assets/shaders/text2.glsl";
		s_displayLayer->m_layerData.m_2DObjects.back().m_hasMat = true; // Mark Object as Having a Material
	}
}

void ChangeObjects::addCollider(Object& object)
{
	// The object now has a collider
	object.m_hasCollider = true;
	// Reset data
	object.m_collider = ColliderData();
	s_displayLayer->m_layerData.m_numColliders++;
}

void ChangeObjects::addRigidbody(Object& object)
{
	// The object now has a rigidbody
	object.m_hasRigidbody = true;
	// Reset the data
	object.m_rigidBody = RigidBodyData();
	s_displayLayer->m_layerData.m_numRigidbodies++;
}

void ChangeObjects::addTexture(std::shared_ptr<Engine::GameObject>& displayObject, bool UI)
{
	if (!UI)
	{
		std::shared_ptr<Engine::Texture> tex = s_displayLayer->getResources()->addTexture("assets/textures/None.png");
		// Make a texture component with it and add it to the object
		s_displayLayer->getTextures().push_back(std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, tex)));

		displayObject->addComponent(s_displayLayer->getTextures().back());
		s_displayLayer->m_layerData.m_objects.back().m_textureComponentVectorValue = s_displayLayer->getTextures().size() - 1;

		s_displayLayer->m_layerData.m_objects.back().m_hasTexture = true;
	}
	else
	{
		std::shared_ptr<Engine::Texture> tex = s_displayLayer->getResources()->getFontTexture();
		// Make a texture component with both textures and add it to the object
		s_displayLayer->getTextures().push_back(std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, tex)));

		displayObject->addComponent(s_displayLayer->getTextures().back());
		s_displayLayer->m_layerData.m_2DObjects.back().m_textureComponentVectorValue = s_displayLayer->getTextures().size() - 1;

		s_displayLayer->m_layerData.m_2DObjects.back().m_hasTexture = true;
	}
}
// Add a Lua Script Component
void ChangeObjects::AddLuaScriptComponent(Object& object)
{
	// Add Lua Script Component
	object.bHasLuaScript = true; // Mark Object as Having Lua Script
	object.m_LuaScript = LuaScriptData(); // Create New Lua Script Object
	s_displayLayer->m_layerData.m_INumOfLuaScripts++; // Inrement Total Number of Lua Scripts
}

void ChangeObjects::makeJoint(const std::string& type)
{
	// Add a joint to the vector
	s_displayLayer->m_layerData.m_joints.push_back(Joint());
	s_displayLayer->m_layerData.m_joints.back().m_type = type;

	s_displayLayer->m_layerData.m_joints.back().m_jointVectorValue = s_displayLayer->m_layerData.m_joints.size() - 1;
}

// Remove Components
// Remove Lua Script Component
void ChangeObjects::RemoveLuaScriptComponent(Object& object)
{
	// Remove Lua Script Component
	object.bHasLuaScript = false; // Mark Object as Not Having Lua Script Component
	object.m_LuaScript = LuaScriptData(); // Default All Lua Script Data
	s_displayLayer->m_layerData.m_INumOfLuaScripts--; // Decrement Total Number of Lua Scripts
}

void ChangeObjects::changeName(int i, bool UI)
{
	// Get The Object
	std::shared_ptr<Engine::GameObject> object;
	int number;

	if (UI)
	{
		object = s_displayLayer->get2DGameObjects().at(i); // Get Pointer to Object
		number = i + s_displayLayer->getGameObjects().size() + s_displayLayer->m_layerData.m_joints.size();
	}
	else
	{
		object = s_displayLayer->getGameObjects().at(i); // Get Pointer to Object
		number = i;
	}
	ImGui::Text(("Object #: " + std::to_string(number)).c_str());
	
	char name[41]; // Create Name Array
	strcpy(name, object->name().c_str()); // Get Object Name

	// Create Object Input Name Widgit Header
	ImGui::Text("Name:");

	// Enter Object Name Widgit
	ImGui::PushItemWidth(200.f); // Set Widgit Width
	std::string sFInputName = "###Name" + std::to_string(number); // Set Widgit Name
	ImGui::InputText(sFInputName.c_str(), name, IM_ARRAYSIZE(name)); // Create Widget Name
	object->name() = name; // Set Object Name to Input Value
}

void ChangeObjects::changeLayer(int i, bool isObject, bool UI)
{
	ImGui::Text("Layer:");

	std::vector<Object>* pObjects;
	int number;
	if (UI)
	{
		pObjects = &s_displayLayer->m_layerData.m_2DObjects;
		number = i + s_displayLayer->getGameObjects().size() + s_displayLayer->m_layerData.m_joints.size();
	}
	else
	{
		pObjects = &s_displayLayer->m_layerData.m_objects;
		number = i;
	}

	// If it's an object
	if (isObject)
	{
		// Get a pointer to the object
		Object* object = &pObjects->at(i);

		std::string sWidgitName = "###Layer" + std::to_string(number);
		if (ImGui::BeginCombo(sWidgitName.c_str(), object->m_layer.c_str())) // Begin Creating List
		{
			std::vector<std::string> layerNames;
			// If it's in a 3D layer
			if (object->m_in3DLayer)
			{
				std::vector<std::string>::iterator it;
				for (it = s_displayLayer->m_layerData.m_3DLayers.begin(); it != s_displayLayer->m_layerData.m_3DLayers.end(); ++it)
				{
					bool selected = object->m_layer == *it;
					// Show a list of the 3D layers
					sWidgitName = *it + "###" + *it + std::to_string(number);
					if (ImGui::Selectable(sWidgitName.c_str(), selected)) // Create Selectable Item
					{
						object->m_layer = *it;
					}
				}
			}
			else
			{
				std::vector<std::string>::iterator it;
				for (it = s_displayLayer->m_layerData.m_2DLayers.begin(); it != s_displayLayer->m_layerData.m_2DLayers.end(); ++it)
				{
					bool selected = object->m_layer == *it;
					// Show a list of the 3D layers
					sWidgitName = *it + "###" + *it + std::to_string(number);
					if (ImGui::Selectable(sWidgitName.c_str(), selected)) // Create Selectable Item
					{
						object->m_layer = *it;
					}
				}
			}

			// Stop Creating List
			ImGui::EndCombo();
		}
	}
	else // If it's a joint
	{
		// Get a pointer to the joint
		Joint* joint = &s_displayLayer->m_layerData.m_joints.at(i);

		int num = i + pObjects->size();
		std::string sWidgitName = "###Layer" + std::to_string(num);
		if (ImGui::BeginCombo(sWidgitName.c_str(), joint->m_layer.c_str())) // Begin Creating List
		{
			std::vector<std::string> layerNames;
			// If it's in a 3D layer
			if (joint->m_in3DLayer)
			{
				std::vector<std::string>::iterator it;
				for (it = s_displayLayer->m_layerData.m_3DLayers.begin(); it != s_displayLayer->m_layerData.m_3DLayers.end(); ++it)
				{
					bool selected = joint->m_layer == *it;
					// List the 3D layers
					sWidgitName = *it + "###" + *it + std::to_string(num);
					if (ImGui::Selectable(sWidgitName.c_str(), selected)) // Create Selectable Item
					{
						joint->m_layer = *it;
					}
				}
			}

			// Stop Creating List
			ImGui::EndCombo();
		}
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Joints must be in the same layer as the objects they connect");
	}
}

void ChangeObjects::changeTag(int i, bool UI)
{
	ImGui::Text("Tag");

	Object* thingInWorld;
	int number;

	if (UI)
	{
		thingInWorld = &s_displayLayer->m_layerData.m_2DObjects.at(i);
		number = i + s_displayLayer->getGameObjects().size() + s_displayLayer->m_layerData.m_joints.size();
	}
	else
	{
		thingInWorld = &s_displayLayer->m_layerData.m_objects.at(i);
		number = i;
	}

	std::string sWidgitName = "###Tag" + std::to_string(number);
	// Make a dropdown menu with all the tags in the level
	if (ImGui::BeginCombo(sWidgitName.c_str(), thingInWorld->m_tag.c_str())) // Begin Creating List
	{
		std::vector<std::string>::iterator it;
		for (it = s_displayLayer->m_layerData.m_tags.begin(); it != s_displayLayer->m_layerData.m_tags.end(); ++it)
		{
			bool selected = thingInWorld->m_tag == *it;

			sWidgitName = *it + "###" + *it + std::to_string(number);
			// If one is selected
			if (ImGui::Selectable(sWidgitName.c_str(), selected)) // Create Selectable Item
			{
				thingInWorld->m_tag = *it; // Set it as the objects tag
			}
		}

		// Stop Creating List
		ImGui::EndCombo();
	}
}

// Object Transforms GUI
// Change Object Position GUI
void ChangeObjects::changePosition(int i, bool UI)
{
	std::vector<Object>* pObjects;

	if (UI)
	{
		pObjects = &s_displayLayer->m_layerData.m_2DObjects;
	}
	else
	{
		pObjects = &s_displayLayer->m_layerData.m_objects;
	}

	// Get the Object's Current Position
	i = pObjects->at(i).m_positionComponentVectorValue; // Get Object
	glm::vec3 position = s_displayLayer->getPositions().at(i)->getPosition(); // Get Position

	// Create Object Position GUI
	ImGui::PushItemWidth(64.f); // Set Widget Width
	ImGui::Text("Position:"); // Output GUI Section Title
	ImGui::Text(" X"); // Indicate X Pos Box
	ImGui::SameLine(); // Make Same Line as...
	std::string sFInputName = "###XPosition" + std::to_string(i); // Define Float Box Name String
	ImGui::InputFloat(sFInputName.c_str(), &position.x); // Create X Pos Float Input
	ImGui::SameLine(); // Make Same Line as...
	ImGui::Text(" Y"); // Indicate Y Pos Box
	ImGui::SameLine(); // Make Same Line as...
	sFInputName = "###YPosition" + std::to_string(i); // Define Float Box Name String
	ImGui::InputFloat(sFInputName.c_str(), &position.y); // Create Y Pos Float Input
	ImGui::SameLine(); // Make Same Line as...
	ImGui::Text(" Z"); // Indicate Z Pos Box
	ImGui::SameLine(); // Make Same Line as...
	sFInputName = "###ZPosition" + std::to_string(i); // Define Float Box Name String
	ImGui::InputFloat(sFInputName.c_str(), &position.z); // Create Z Pos Float Input
	ImGui::NewLine(); // Insert Line Break

	// Set New Object Position
	Engine::ComponentMessage msg(Engine::ComponentMessageType::PositionSet, (void*)&position); // Create Message
	s_displayLayer->getPositions().at(i)->receiveMessage(msg); // Send Message
}
// Change Object Rotation GUI
void ChangeObjects::changeRotation(int i, bool UI)
{
	std::vector<Object>* pObjects;

	if (UI)
	{
		pObjects = &s_displayLayer->m_layerData.m_2DObjects;
	}
	else
	{
		pObjects = &s_displayLayer->m_layerData.m_objects;
	}

	// Get Object Current Rotation
	i = pObjects->at(i).m_positionComponentVectorValue; // Get Object
	glm::vec3 rotation = s_displayLayer->getPositions().at(i)->getRotation(); // Get Object Rotation
	rotation = glm::degrees(rotation);

	// Create Object Rotation GUI
	ImGui::PushItemWidth(64.f); // Set Widget Width
	ImGui::Text("Rotation:"); // Output GUI Section Title
	ImGui::Text(" X"); // Indicate X Rotation Box
	ImGui::SameLine(); // Make Same Line as...
	std::string sFInputName = "###XRotation" + std::to_string(i); // Define Float Box Name String
	ImGui::InputFloat(sFInputName.c_str(), &rotation.x); // Create X Rotation Float Input
	ImGui::SameLine(); // Make Same Line as...
	ImGui::Text(" Y"); // Indicate Y Rotation Box
	ImGui::SameLine(); // Make Same Line as...
	sFInputName = "###YRotation" + std::to_string(i); // Define Float Box Name String
	ImGui::InputFloat(sFInputName.c_str(), &rotation.y); // Create Y Rotation Float Input
	ImGui::SameLine(); // Make Same Line as...
	ImGui::Text(" Z"); // Indicate Z Rotation Box
	ImGui::SameLine(); // Make Same Line as...
	sFInputName = "###ZRotation" + std::to_string(i); // Define Float Box Name String
	ImGui::InputFloat(sFInputName.c_str(), &rotation.z); // Create Z Rotation Float Input
	ImGui::NewLine(); // Insert Line Break

	// Set New Object Rotation
	Engine::ComponentMessage msg(Engine::ComponentMessageType::RotationSet, (void*)&rotation); // Create Message
	s_displayLayer->getPositions().at(i)->receiveMessage(msg); // Send Message
}
// Change Object Scale GUI
void ChangeObjects::changeScale(int i, bool UI)
{
	std::vector<Object>* pObjects;

	if (UI)
	{
		pObjects = &s_displayLayer->m_layerData.m_2DObjects;
	}
	else
	{
		pObjects = &s_displayLayer->m_layerData.m_objects;
	}

	// Get Object Scale
	i = pObjects->at(i).m_positionComponentVectorValue; // Get Object
	glm::vec3 scale = s_displayLayer->getPositions().at(i)->getScale(); // Get Object Scale

	// Create Object Scale GUI
	ImGui::PushItemWidth(64.f); // Set Widget Width
	ImGui::Text("Scale:"); // Output GUI Section Title
	ImGui::Text(" X"); // Indicate X Scale Box
	ImGui::SameLine(); // Make Same Line as...
	std::string sFInputName = "###XScale" + std::to_string(i); // Define Float Box Name String
	ImGui::InputFloat(sFInputName.c_str(), &scale.x); // Create X Scale Float Input
	ImGui::SameLine(); // Make Same Line as...
	ImGui::Text(" Y"); // Indicate Y Scale Box
	ImGui::SameLine(); // Make Same Line as...
	sFInputName = "###YScale" + std::to_string(i); // Define Float Box Name String
	ImGui::InputFloat(sFInputName.c_str(), &scale.y); // Create Y Scale Float Input
	ImGui::SameLine(); // Make Same Line as...
	ImGui::Text(" Z"); // Indicate Z Scale Box
	ImGui::SameLine(); // Make Same Line as...
	sFInputName = "###ZScale" + std::to_string(i); // Define Float Box Name String
	ImGui::InputFloat(sFInputName.c_str(), &scale.z); // Create Z Scale Float Input
	ImGui::NewLine(); // Insert Line Break

	// Set New Object Scale
	Engine::ComponentMessage msg(Engine::ComponentMessageType::ScaleSet, (void*)&scale); // Create Message
	s_displayLayer->getPositions().at(i)->receiveMessage(msg); // Send Message
}

// Change Lua Script GUI
void ChangeObjects::LuaScriptComponentGUI(int i, bool UI)
{
	std::vector<Object>* pObjects;
	int number;

	if (UI)
	{
		number = i + s_displayLayer->getGameObjects().size() + s_displayLayer->m_layerData.m_joints.size();
		pObjects = &s_displayLayer->m_layerData.m_2DObjects;
	}
	else
	{
		number = i;
		pObjects = &s_displayLayer->m_layerData.m_objects;
	}

	// Output Collapsable Header
	if (ImGui::CollapsingHeader("Lua Script"))
	{
		// Indent GUI
		ImGui::Indent(16.f);

		// Output Text Label
		ImGui::Text("Lua Script:");

		// Add Help Button
		ImGui::SameLine();
		std::string sWidgitName = "?###LuaScriptHelpButton" + std::to_string(number); // Set Widgit Name
		if (ImGui::Button(sWidgitName.c_str(),ImVec2(15.f, 16.f))) // If Button Pressed
		{
			// Set Window to Open or Close it
			if (bShowLuaHelpWindow) // If Window is Open
				bShowLuaHelpWindow = false; // Close Window
			else // Otherwise
			{
				// Set Window to Open
				bShowLuaHelpWindow = true;

				// Set Window Size
				ImGui::SetNextWindowSize(ImVec2(655.f, 400.f));
			}
		}

		sWidgitName = "###LuaScriptName" + std::to_string(number); // Define Filepath Input Box Namestring
		// List of options
		if (ImGui::BeginCombo(sWidgitName.c_str(), parseFilePath(pObjects->at(i).m_LuaScript.m_sScriptFilePath).c_str())) // Begin Creating List
		{
			// Option to make a new script
			sWidgitName = "New###NewScript" + std::to_string(number);
			if (ImGui::Selectable(sWidgitName.c_str(), false)) // Create Selectable Item
			{
				pObjects->at(i).m_LuaScript.m_makingNewScript = true;
			}
			ImGui::NewLine();
			// List existing scripts in script folder
			for (const auto& entry : std::filesystem::directory_iterator("assets/savedJSON/" + s_sProjectName + "/LuaScript/"))
			{
				std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
				if (fileType.compare(".txt") != 0)
				{
					std::string name = parseFilePath(entry.path().string());

					sWidgitName = name + "###" + name + std::to_string(number);
					bool selected = !entry.path().string().compare(pObjects->at(i).m_LuaScript.m_sScriptFilePath); // Check If Selected

					if (ImGui::Selectable(sWidgitName.c_str(), selected)) // Create Selectable Item
					{
						pObjects->at(i).m_LuaScript.m_sScriptFilePath = entry.path().string();
					}
				}
			}

			// Stop Creating List
			ImGui::EndCombo();
		}
		// If making a new script
		if (pObjects->at(i).m_LuaScript.m_makingNewScript)
		{
			ImGui::NewLine();
			ImGui::Text("Script Name:");
			ImGui::PushItemWidth(200.f); // Set Widget Width
			sWidgitName = "###LuaScriptNameInput" + std::to_string(number); // Define Filepath Input Box Namestring
			// Text input for script name
			ImGui::InputText(sWidgitName.c_str(), pObjects->at(i).m_LuaScript.m_newName, IM_ARRAYSIZE(pObjects->at(i).m_LuaScript.m_newName)); // Create Text Input Box

			sWidgitName = "Make Script###makeScript" + std::to_string(number); // Define Filepath Input Box Namestring
			if (ImGui::Button(sWidgitName.c_str()))
			{
				std::string newFilepath = "assets/savedJSON/" + s_sProjectName + "/LuaScript/" + pObjects->at(i).m_LuaScript.m_newName + ".lua";
				std::filesystem::copy("assets/LuaScript/DefaultScript.lua", newFilepath);
				pObjects->at(i).m_LuaScript.m_sScriptFilePath = newFilepath;

				pObjects->at(i).m_LuaScript.m_makingNewScript = false;
			}
		}

		// Open Script Button
		if (pObjects->at(i).m_LuaScript.m_sScriptFilePath != "") // If Script Exists
		{
			// Create Open Script Button
			ImGui::SameLine();
			sWidgitName = "Open###OpenLuaScriptButton" + std::to_string(number); // Set Widgit Name
			if (ImGui::Button(sWidgitName.c_str())) // If Button Pressed
			{
				// Get Filepath
				std::string sFilePath = std::filesystem::current_path().string() + "//" + pObjects->at(i).m_LuaScript.m_sScriptFilePath; // Get Filepath
				std::wstring wsFilePath = std::wstring(sFilePath.begin(), sFilePath.end()); // Convert to wstring
				LPCWSTR lpFilePath = wsFilePath.c_str(); // Convert to LPCWSTR

				// Get Folderpath
				std::size_t EndOfFolderPath = sFilePath.find_last_of("/\\"); // Get Position Where Folder File Path Ends
				std::string sFolderPath = sFilePath.substr(0, EndOfFolderPath); // Get Folder File Path
				std::wstring wsFolderPath = std::wstring(sFolderPath.begin(), sFolderPath.end()); // Convert to wstring
				LPCWSTR lpFolderPath = wsFolderPath.c_str(); // Convert to LPCWSTR

				// Open Script
				ShellExecuteW(NULL, L"open", lpFilePath, L"", lpFolderPath, SW_SHOW);
			}
		}

		// Show Help Window?
		if (bShowLuaHelpWindow) // If Button Pressed
		{
			// Create New Window
			ImGui::Begin("Lua Scripting Help:");

			// Basics Paragraph
#pragma region Basics
			ImGui::Text("Basics:");
			ImGui::Text("Lua is a lightweight, high-level, dynamically typed programming language. It is used");
			ImGui::Text("within our game engine as a scripting language. By giving an object a Lua script component");
			ImGui::Text("you can program it to perform certain actions during runtime, like responding to input");
			ImGui::Text("from the player, controlling events in the game and the physical behaviour of objects, or");
			ImGui::Text("even to create a complex Artifical Intelligence System for a Non-Player Character.");
#pragma endregion

			// How to use the Lua Script Component Paragraph
#pragma region HowToUse
			ImGui::NewLine();
			ImGui::Text("How to use the Lua Script Component:");
			ImGui::Text("By adding a Lua script component to an object you have already begun to implement a lua");
			ImGui::Text("script. Step two is to assign an actual Lua script file to this component that the will");
			ImGui::Text("contain the objects code. Click on the Lua script drop down list to open it. Here you can");
			ImGui::Text("either create a brand new Lua script file by clicking on the 'New' button, or if any other");
			ImGui::Text("scripts have been created in this project, select them from underneath. If you make a");
			ImGui::Text("new script file you will have to give it a name. Once a Lua script has been created, open");
			ImGui::Text("it from File Explorer in your preferred scripting interface and begin writing code.");
#pragma endregion

			// Understanding the Start and Update Functions Paragraph
#pragma region StartAndUpdate
			ImGui::NewLine();
			ImGui::Text("Understanding the Start and Update Functions:");
			ImGui::Text("Upon opening a Lua script component you will see two functions, start and update, already");
			ImGui::Text("defined in the code. Scripting in the Team Funk Level Editor is event based. The start");
			ImGui::Text("function is called once when the object with the attached Lua script component is first");
			ImGui::Text("created and is used to initialize the object. The update function is called once every");
			ImGui::Text("subsequent frame and will contain the main body of your code.");
#pragma endregion

			// Object Transforms Paragraph
#pragma region ObjectTransforms
			ImGui::NewLine();
			ImGui::Text("Object Transforms:");
			ImGui::Text("Every object in the editor has a transform component. It is used to control the object's");
			ImGui::Text("position, rotation and scale. You can access the x, y and z values for each directly or");
			ImGui::Text("from a LuaVector. Placeholder text is indicated with <>.");
			ImGui::NewLine();
			ImGui::Text("	Example:");
			ImGui::Text("	--------------------------------------------------------------------------------");
			ImGui::Text("	|                      Code                       |        Description         |");
			ImGui::Text("	|-------------------------------------------------|----------------------------|");
			ImGui::Text("	|         Transform.Position.<x, y or z>          | The x, y and z components  |");
			ImGui::Text("	|                                                 | of the objects position    |");
			ImGui::Text("	|-------------------------------------------------|----------------------------|");
			ImGui::Text("	|           Transform.Scale.<x, y or z>           | The x, y and z components  |");
			ImGui::Text("	|                                                 | of the objects scale       |");
			ImGui::Text("	|-------------------------------------------------|----------------------------|");
			ImGui::Text("	|         Transform.Rotation.<x, y or z>          | The x, y and z components  |");
			ImGui::Text("	|                                                 | of the objects rotation    |");
			ImGui::Text("	|-------------------------------------------------|----------------------------|");
			ImGui::Text("	|                                                 | Sets the object's rotation |");
			ImGui::Text("	|                                                 | to the values stored in    |");
			ImGui::Text("	|     Transform.Rotation.Set(<A Lua Vector>)      | the LuaVector. This        |");
			ImGui::Text("	|                                                 | function exists for        |");
			ImGui::Text("	|                                                 | position and scale as well |");
			ImGui::Text("	|-------------------------------------------------|----------------------------|");
			ImGui::Text("	|                                                 | Returns the object's scale |");
			ImGui::Text("	|            Transform.Position.Get()             | as a LuaVector. This       |");
			ImGui::Text("	|                                                 | function exists for        |");
			ImGui::Text("	|                                                 | rotation and scale as well |");
			ImGui::Text("	|-------------------------------------------------|----------------------------|");
			ImGui::Text("	|  Transform.Position.Translate(<A Lua Vector>)   | Translates the object by   |");
			ImGui::Text("	|                                                 | the input LuaVector        |");
			ImGui::Text("	|-------------------------------------------------|----------------------------|");
			ImGui::Text("	|    Transform.Rotation.Rotate(<A Lua Vector>)    | Rotates the object by the  |");
			ImGui::Text("	|                                                 | input LuaVector            |");
			ImGui::Text("	|-------------------------------------------------|----------------------------|");
			ImGui::Text("	|                                                 | Increases the object's     |");
			ImGui::Text("	| Transform.Scale.IncrementScale(<A Lua Vector>)  | scale by the input         |");
			ImGui::Text("	|                                                 | LuaVector                  |");
			ImGui::Text("	|-------------------------------------------------|----------------------------|");
			ImGui::Text("	|                                                 | Increases the object's     |");
			ImGui::Text("	|  Transform.Scale.ScaleByFactor(<A Lua Vector>)  | scale by a factor of the   |");
			ImGui::Text("	|                                                 | input LuaVector            |");
			ImGui::Text("	--------------------------------------------------------------------------------");
#pragma endregion

			// Lua Vectors
#pragma region LuaVectors
			// Lua Vector Object Paragraph
			ImGui::NewLine();
			ImGui::Text("Lua Vector Objects:");
			ImGui::Text("LuaVectors are used to store vector data in Lua. Each vector has an x, y and z component");
			ImGui::Text("and has vector arithmetic behaviour. Note that in Lua object behaviour is called with ':'.");
			ImGui::Text("Placeholder text is indicated with <>.");
			ImGui::NewLine();
			ImGui::Text("	-----------------------------------------------------------------------------------------");
			ImGui::Text("	|            Code             |                       Description                       |");
			ImGui::Text("	|-----------------------------|---------------------------------------------------------|");
			ImGui::Text("	| <A Lua Vector>.<x, y or z>  |       The x, y and z components of the LuaVector        |");
			ImGui::Text("	|-----------------------------|---------------------------------------------------------|");
			ImGui::Text("	| <A Lua Vector>:Set(1, 2, 3) | Function to set the values of a LuaVector. Notice that  |");
			ImGui::Text("	|                             | as this is object behaviour, it is called with a ':'    |");
			ImGui::Text("	|-----------------------------|---------------------------------------------------------|");
			ImGui::Text("	| <A Lua Vector>:Magnitude()  | Function to calculate and return the magnitude of the   |");
			ImGui::Text("	|                             | LuaVector                                               |");
			ImGui::Text("	|-----------------------------|---------------------------------------------------------|");
			ImGui::Text("	| <A Lua Vector>:Normalize()  |            Function to normalize a LuaVector            |");
			ImGui::Text("	|-----------------------------|---------------------------------------------------------|");
			ImGui::Text("	|                             | Function to scale the LuaVector by an input factor.     |");
			ImGui::Text("	|  <A Lua Vector>:ScaleBy(4)  | The example code will scale the LuaVector to four times |");
			ImGui::Text("	|                             | it's original length.                                   |");
			ImGui::Text("	-----------------------------------------------------------------------------------------");

			// Lua Vector Namespace Paragraph
			ImGui::NewLine();
			ImGui::Text("The Lua Vector Namespace:");
			ImGui::Text("Creating Lua Vector objects and other vector arithmetic functionality is in the Lua Vector");
			ImGui::Text("namespace. These functions are listed below.");
			ImGui::NewLine();
			ImGui::Text("	-----------------------------------------------------------------------------------------");
			ImGui::Text("	|                           Code                           |        Description         |");
			ImGui::Text("	|----------------------------------------------------------|----------------------------|");
			ImGui::Text("	|                                                          | Function to create and     |");
			ImGui::Text("	|                                                          | return a LuaVector object  |");
			ImGui::Text("	|                                                          | with input x, y and z      |");
			ImGui::Text("	|                LuaVector.Create(5, 7, 10)                | values. In the exmaple     |");
			ImGui::Text("	|                                                          | code the created LuaVector |");
			ImGui::Text("	|                                                          | will have x, y and z       |");
			ImGui::Text("	|                                                          | values 5, 7 and 10         |");
			ImGui::Text("	|                                                          | respectively.              |");
			ImGui::Text("	|----------------------------------------------------------|----------------------------|");
			ImGui::Text("	|                                                          | Function to perform        |");
			ImGui::Text("	|                                                          | component wise addition on |");
			ImGui::Text("	|    LuaVector.Addition(<Lua Vector 1>, <Lua Vector 2>)    | the input Lua Vectors and  |");
			ImGui::Text("	|                                                          | return the result as       |");
			ImGui::Text("	|                                                          | another Lua Vector object  |");
			ImGui::Text("	|----------------------------------------------------------|----------------------------|");
			ImGui::Text("	|                                                          | Function to perform        |");
			ImGui::Text("	|                                                          | component wise subtraction |");
			ImGui::Text("	|  LuaVector.Subtraction(<Lua Vector 1>, <Lua Vector 2>)   | on the input Lua Vectors   |");
			ImGui::Text("	|                                                          | and return the result as   |");
			ImGui::Text("	|                                                          | another Lua Vector object  |");
			ImGui::Text("	|----------------------------------------------------------|----------------------------|");
			ImGui::Text("	|                                                          | Function to perform        |");
			ImGui::Text("	|                                                          | component wise             |");
			ImGui::Text("	| LuaVector.Multiplication(<Lua Vector 1>, <Lua Vector 2>) | multiplication on the      |");
			ImGui::Text("	|                                                          | input Lua Vectors and      |");
			ImGui::Text("	|                                                          | return the result as       |");
			ImGui::Text("	|                                                          | another Lua Vector object  |");
			ImGui::Text("	|----------------------------------------------------------|----------------------------|");
			ImGui::Text("	|                                                          | Function to calculate and  |");
			ImGui::Text("	|   LuaVector.DotProduct(<Lua Vector 1>, <Lua Vector 2>)   | return the dot product of  |");
			ImGui::Text("	|                                                          | the two input vectors      |");
			ImGui::Text("	|----------------------------------------------------------|----------------------------|");
			ImGui::Text("	|  LuaVector.AngleBetween(<Lua Vector 1>, <Lua Vector 2>)  | Function to calculate and  |");
			ImGui::Text("	|                                                          | return the angle between   |");
			ImGui::Text("	|                                                          | the two input vectors      |");
			ImGui::Text("	-----------------------------------------------------------------------------------------");
#pragma endregion

			// Lua Colour
#pragma region LuaColour
			// Lua Colour Object Paragraph
			ImGui::NewLine();
			ImGui::Text("Lua Colour Objects:");
			ImGui::Text("The Lua Colour objects are used to define a colour. Each one has an r, g and b");
			ImGui::Text("component and can be set either from given RGB values or from a Hexadecimal string. The");
			ImGui::Text("engine uses RGB values as a decimal >= 0 and <= 1. So in order to get the LuaColour object");
			ImGui::Text("to store a colour with RGB values 35, 201, 124, you would need to set the LuaColour's r, g");
			ImGui::Text("and b to 0.137, 0.788, 0.486 respectively. Again, note that in Lua object behaviour is");
			ImGui::Text("called with ':'.");
			ImGui::NewLine();
			ImGui::Text("	----------------------------------------------------------------------------------------");
			ImGui::Text("	|                Code                |                   Description                   |");
			ImGui::Text("	|------------------------------------|-------------------------------------------------|");
			ImGui::Text("	|     <A Lua Colour>.<r, g or b>     | The r, g and b components of the LuaColour      |");
			ImGui::Text("	|                                    | object                                          |");
			ImGui::Text("	|------------------------------------|-------------------------------------------------|");
			ImGui::Text("	|                                    | Function to set the stored colour from input    |");
			ImGui::Text("	| <A Lua Colour>:SetFromRGB(1, 1, 1) | RGB values. As RGB values are given in the      |");
			ImGui::Text("	|                                    | range 0 - 1 the example code would set the      |");
			ImGui::Text("	|                                    | stored colour to white (255, 255, 255).         |");
			ImGui::Text("	|------------------------------------|-------------------------------------------------|");
			ImGui::Text("	|                                    | Function to set the stored colour from an input |");
			ImGui::Text("	|                                    | hexadecimal string. The RGB values taken from   |");
			ImGui::Text("	| <A Lua Colour>:SetFromHex(#ffffff) | the input hexadecimal string are divided by 255 |");
			ImGui::Text("	|                                    | in order to be within the range 0 - 1 and       |");
			ImGui::Text("	|                                    | therefore you do NOT need to perform this       |");
			ImGui::Text("	|                                    | division on the string itself.                  |");
			ImGui::Text("	----------------------------------------------------------------------------------------");

			// Lua Colour Namespace Paragraph
			ImGui::NewLine();
			ImGui::Text("Lua Colour Namespace:");
			ImGui::Text("Lua Colour object's can be created from the LuaColour namespace. Again the Lua Colour's");
			ImGui::Text("RGB values can be set either directly or by passing in a hexadecimal string.");
			ImGui::NewLine();
			ImGui::Text("	----------------------------------------------------------------------------------------");
			ImGui::Text("	|               Code               |                    Description                    |");
			ImGui::Text("	|----------------------------------|---------------------------------------------------|");
			ImGui::Text("	|                                  | Creates and returns a Lua Colour object with the  |");
			ImGui::Text("	| LuaColour.CreateFromRGB(1, 1, 1) | given RGB values. The example code creates a Lua  |");
			ImGui::Text("	|                                  | Colour object storing the colour white.           |");
			ImGui::Text("	|----------------------------------|---------------------------------------------------|");
			ImGui::Text("	|                                  | Creates and returns a Lua Colour object with the  |");
			ImGui::Text("	|                                  | RGB values from the given hexadecimal string. The |");
			ImGui::Text("	|                                  | calculated RGB values are divided by 255 in order |");
			ImGui::Text("	| LuaColour.CreateFromHex(#ffffff) | to be within the range 0 - 1. You do not need to  |");
			ImGui::Text("	|                                  | perform this division on the string itself. The   |");
			ImGui::Text("	|                                  | example code creates a Lua Colour object storing  |");
			ImGui::Text("	|                                  | the colour white.                                 |");
			ImGui::Text("	----------------------------------------------------------------------------------------");
#pragma endregion

			// Timestep 'Paragraph'
#pragma region Timestep
			ImGui::NewLine();
			ImGui::Text("Timestep:");
			ImGui::Text("It's important to know the elapsed time between frames, in order to program your game to");
			ImGui::Text("run smoothly. The timestep can be found within the time namespace");
			ImGui::NewLine();
			ImGui::Text("	                                        Time.Timestep                                    ");
#pragma endregion

			// Object Model Paragraph
#pragma region ObjectModel
			ImGui::NewLine();
			ImGui::Text("The Object's Model:");
			ImGui::Text("You can use Set function in the Model namespace to change the objects model.");
			ImGui::NewLine();
			ImGui::Text("                          Model.Set(<Filepath to the new model>)                          ");
			ImGui::NewLine();
			ImGui::Text("The filepath to the models given by default in the engine are below:");
			ImGui::NewLine();
			ImGui::Text("	-----------------------------");
			ImGui::Text("	|  Model   |      Code      |");
			ImGui::Text("	|----------|----------------|");
			ImGui::Text("	|   Cube   |   Model.Cube   |");
			ImGui::Text("	|----------|----------------|");
			ImGui::Text("	| Capsule  | Model.Capsule  |");
			ImGui::Text("	|----------|----------------|");
			ImGui::Text("	| Cylinder | Model.Cylinder |");
			ImGui::Text("	|----------|----------------|");
			ImGui::Text("	|   Quad   |   Model.Quad   |");
			ImGui::Text("	|----------|----------------|");
			ImGui::Text("	|  Sphere  |  Model.Sphere  |");
			ImGui::Text("	-----------------------------");
#pragma endregion

			// Using the Logger Functions Paragraph
#pragma region LoggerFunctions
			ImGui::NewLine();
			ImGui::Text("Using the Logger:");
			ImGui::Text("The editor has a built in logger object, that is used to output text to the console. The");
			ImGui::Text("following functions can be called from a Lua script to output a given string to the");
			ImGui::Text("console. As the output of the console window has no effect on the game, their ideal use is");
			ImGui::Text("for debugging purposes. Each function outputs text in a different format to represent the");
			ImGui::Text("intent behind the output.");
			ImGui::NewLine();
			ImGui::Text("	Functions:");
			ImGui::Text("	-----------------------------------------------------------");
			ImGui::Text("	|       Code      |              Description              |");
			ImGui::Text("	|-----------------|---------------------------------------|");
			ImGui::Text("	|   Logger.Info   | Outputs a given string to the console |");
			ImGui::Text("	|-----------------|---------------------------------------|");
			ImGui::Text("	|   Logger.Trace  | Outputs a given string to the console |");
			ImGui::Text("	|-----------------|---------------------------------------|");
			ImGui::Text("	|   Logger.Error  | Outputs a given string to the console |");
			ImGui::Text("	|-----------------|---------------------------------------|");
			ImGui::Text("	|   Logger.Warn   | Outputs a given string to the console |");
			ImGui::Text("	|-----------------|---------------------------------------|");
			ImGui::Text("	| Logger.Critical | Outputs a given string to the console |");
			ImGui::Text("	-----------------------------------------------------------");
#pragma endregion

			// Input Polling Paragraph
#pragma region InputPolling
			ImGui::NewLine();
			ImGui::Text("Input Polling:");
			ImGui::Text("In order for someone to actually be able to play your game, the game must be able to react");
			ImGui::Text("to their inputs. The editor's input polling functionality is in the input namespace. The");
			ImGui::Text("function keyPressed returns true or false depending on wether the key with the input key");
			ImGui::Text("code is currently being pressed.");
			ImGui::NewLine();
			ImGui::Text("                                     Input.KeyPressed                                     ");
			ImGui::NewLine();
			ImGui::Text("The input namespace contains another namespace 'keys' that holds the key codes for the");
			ImGui::Text("different keys on the keyboard. The table below shows how to get the key code for a given");
			ImGui::Text("key. Placeholder text is indicated with <>.");
			ImGui::NewLine();
			ImGui::Text("	Key Codes:");
			ImGui::Text("	----------------------------------------------------------------------");
			ImGui::Text("	|   Keyboard Key   |           READ ONLY Key Code Variable           |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	| <Any Letter Key> | Input.Keys.<The Letter (lowercase single char)> |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	| <Any Number Key> |    Input.Keys.num_<The Number (as a digit)>     |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|      Space       |                Input.Keys.space                 |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|     Escape       |                Input.Keys.escape                |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|      Enter       |                Input.Keys.enter                 |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|       Tab        |                 Input.Keys.tab                  |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|    Backspace     |              Input.Keys.backspace               |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|      Insert      |                Input.Keys.insert                |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|      Delete      |                Input.Keys.delete                |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|      Right       |                Input.Keys.right                 |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|       Left       |                 Input.Keys.left                 |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|        Up        |                  Input.Keys.up                  |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|       Down       |                 Input.Keys.down                 |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|    Left Shift    |              Input.Keys.left_shift              |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|    Left CTRL     |              Input.Keys.left_ctrl               |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|     Left Alt     |               Input.Keys.left_alt               |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|   Right Shift    |             Input.Keys.right_shift              |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|    Right CTRL    |              Input.Keys.right_ctrl              |");
			ImGui::Text("	|------------------|-------------------------------------------------|");
			ImGui::Text("	|    Right Alt     |              Input.Keys.right_alt               |");
			ImGui::Text("	----------------------------------------------------------------------");
			ImGui::NewLine();
			ImGui::Text("Similar to the keyPressed function, the input namespace also contains the function");
			ImGui::Text("mousePressed that returns true or false depending on wether the mouse button with the");
			ImGui::Text("given key code is currently being pressed.");
			ImGui::NewLine();
			ImGui::Text("                                    Input.MousePressed                                    ");
			ImGui::NewLine();
			ImGui::Text("While also technically considered to be key codes, the codes for the mouse buttons can be");
			ImGui::Text("found in the mouseButtons namespace within input.");
			ImGui::NewLine();
			ImGui::Text("	Mouse Button Codes:");
			ImGui::Text("	------------------------------------------------------");
			ImGui::Text("	|   Mouse Button   |   READ ONLY Key Code Variable   |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|       Left       |  Input.MouseButtons.Mouse_Left  |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|      Middle      | Input.MouseButtons.Mouse_Middle |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|      Right       | Input.MouseButtons.Mouse_Right  |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|       Last       |  Input.MouseButtons.Mouse_Last  |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|  Mouse Button 1  |   Input.MouseButtons.Mouse_1    |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|  Mouse Button 2  |   Input.MouseButtons.Mouse_2    |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|  Mouse Button 3  |   Input.MouseButtons.Mouse_3    |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|  Mouse Button 4  |   Input.MouseButtons.Mouse_4    |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|  Mouse Button 5  |   Input.MouseButtons.Mouse_5    |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|  Mouse Button 6  |   Input.MouseButtons.Mouse_6    |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|  Mouse Button 7  |   Input.MouseButtons.Mouse_7    |");
			ImGui::Text("	|------------------|---------------------------------|");
			ImGui::Text("	|  Mouse Button 8  |   Input.MouseButtons.Mouse_8    |");
			ImGui::Text("	------------------------------------------------------");
#pragma endregion

			// Material Component Paragraph
#pragma region MaterialComponent
			ImGui::NewLine();
			ImGui::Text("Material Component:");
			ImGui::Text("If the object has a Material component you can manipulate it with the following.");
			ImGui::Text("Placeholder text is indicated with <>.");
			ImGui::NewLine();
			ImGui::Text("	-----------------------------------------------------------------------------------------");
			ImGui::Text("	|                Code                 |                   Description                   |");
			ImGui::Text("	|-------------------------------------|-------------------------------------------------|");
			ImGui::Text("	|   Material.Diffuse.<r, g, or b>     | The red, green and blue components of the       |");
			ImGui::Text("	|                                     | material's diffuse value                        |");
			ImGui::Text("	|-------------------------------------|-------------------------------------------------|");
			ImGui::Text("	| Material.Diffuse.Set(<Lua Colour>)  | Sets the objects material diffuse to the values |");
			ImGui::Text("	|                                     | stored in the LuaColour object.                 |");
			ImGui::Text("	|-------------------------------------|-------------------------------------------------|");
			ImGui::Text("	|       Material.Diffuse.Get()        | Gets the objects material diffuse as a          |");
			ImGui::Text("	|                                     | LuaColour object                                |");
			ImGui::Text("	|-------------------------------------|-------------------------------------------------|");
			ImGui::Text("	|   Material.Specular.<r, g, or b>    | The red, green and blue components of the       |");
			ImGui::Text("   |                                     | material's specular value                       |");
			ImGui::Text("	|-------------------------------------|-------------------------------------------------|");
			ImGui::Text("	| Material.Specular.Set(<Lua Colour>) | Sets the objects material specular to the       |");
			ImGui::Text("	|                                     | values stored in the LuaColour object           |");
			ImGui::Text("	|-------------------------------------|-------------------------------------------------|");
			ImGui::Text("	|       Material.Specular.Get()       | Gets the objects material specular as a         |");
			ImGui::Text("	|                                     | LuaColour object                                |");
			ImGui::Text("	|-------------------------------------|-------------------------------------------------|");
			ImGui::Text("	|         Material.Shininess          | The shininess value of the material             |");
			ImGui::Text("	-----------------------------------------------------------------------------------------");
#pragma endregion

			// Texture Component Paragraph
#pragma region TextureComponent
			ImGui::NewLine();
			ImGui::Text("Texture Component:");
			ImGui::Text("If the object has a texture component, you can access it's height scale in Lua.");
			ImGui::NewLine();
			ImGui::Text("	----------------------------------------------------");
			ImGui::Text("	|        Code         |        Description         |");
			ImGui::Text("	|---------------------|----------------------------|");
			ImGui::Text("	| Texture.HeightScale | The texture's height scale |");
			ImGui::Text("	----------------------------------------------------");
#pragma endregion

			// Rigid Body Paragraph
#pragma region RidgidBody
			ImGui::NewLine();
			ImGui::Text("Rigid Body:");
			ImGui::Text("If the object with the Lua script attached to it has a Rigid Body component you have");
			ImGui::Text("access to the following. Placeholder text is indicated with <>.");
			ImGui::NewLine();
			ImGui::Text("	----------------------------------------------------------------------------------------");
			ImGui::Text("	|                            Code                             |      Description       |");
			ImGui::Text("	|-------------------------------------------------------------|------------------------|");
			ImGui::Text("	|                                                             | Applies the force of   |");
			ImGui::Text("	|            RigidBody.ApplyForce(<A Lua Vector>)             | the input LuaVector to |");
			ImGui::Text("	|                                                             | the center of the      |");
			ImGui::Text("	|                                                             | object's Rigid Body    |");
			ImGui::Text("	|-------------------------------------------------------------|------------------------|");
			ImGui::Text("	|                                                             | Applies a force of the |");
			ImGui::Text("	|                                                             | first input LuaVector  |");
			ImGui::Text("	| RigidBody.ApplyForceToPoint(<Lua Vector 1>, <Lua Vector 2>) | to the point of the    |");
			ImGui::Text("	|                                                             | second LuaVector       |");
			ImGui::Text("	|                                                             | relative to the        |");
			ImGui::Text("	|                                                             | object's rigid body    |");
			ImGui::Text("	|-------------------------------------------------------------|------------------------|");
			ImGui::Text("	|                                                             | Aplies a torque of     |");
			ImGui::Text("	|            RigidBody.ApplyTorque(<A Lua Vector>)            | the input LuaVector to |");
			ImGui::Text("	|                                                             | the object's rigid     |");
			ImGui::Text("	|                                                             | body                   |");
			ImGui::Text("	|-------------------------------------------------------------|------------------------|");
			ImGui::Text("	|                                                             | The Rigid Bodies       |");
			ImGui::Text("	|               RigidBody.Velocity.<x, y or z>                | current velocity x, y  |");
			ImGui::Text("	|                                                             | and z components       |");
			ImGui::Text("	|-------------------------------------------------------------|------------------------|");
			ImGui::Text("	|                                                             | Gets the Rigid Body's  |");
			ImGui::Text("	|                  RigidBody.Velocity.Get()                   | velocity as a          |");
			ImGui::Text("	|                                                             | LuaVector              |");
			ImGui::Text("	|-------------------------------------------------------------|------------------------|");
			ImGui::Text("	|                                                             | Sets the Rigid Body's  |");
			ImGui::Text("	|            RigidBody.Velocity.Set(<Lua Vector>)             | velocity to the values |");
			ImGui::Text("	|                                                             | of the given LuaVector |");
			ImGui::Text("	|-------------------------------------------------------------|------------------------|");
			ImGui::Text("	|           RigidBody.Velocity.Angular.<x, y or z>            | The Rigid Bodies       |");
			ImGui::Text("	|                                                             | angular velocity       |");
			ImGui::Text("	|-------------------------------------------------------------|------------------------|");
			ImGui::Text("	|                                                             | Gets the Rigid Body's  |");
			ImGui::Text("	|              RigidBody.Velocity.Angular.Get()               | angular velocity as a  |");
			ImGui::Text("	|                                                             | LuaVector              |");
			ImGui::Text("	|-------------------------------------------------------------|------------------------|");
			ImGui::Text("	|                                                             | Sets the Rigid Body's  |");
			ImGui::Text("	|        RigidBody.Velocity.Angular.Set(<Lua Vector>)         | angular velocity to    |");
			ImGui::Text("	|                                                             | the values of the      |");
			ImGui::Text("	|                                                             | given LuaVector        |");
			ImGui::Text("	----------------------------------------------------------------------------------------");
#pragma endregion

			// Light Component Paragraph
#pragma region LightComponent
			ImGui::NewLine();
			ImGui::Text("Light Component:");
			ImGui::Text("If the object has a Light component attached you have access to the following. Placeholder");
			ImGui::Text("text is indicated with <>.");
			ImGui::NewLine();
			ImGui::Text("	-----------------------------------------------------------------------------------------");
			ImGui::Text("	|                      Code                      |         Description                  |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|           Light.Ambient.<r, g or b>            | The red, green and blue values of    |");
			ImGui::Text("	|                                                | the light's Ambient colour           |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|        Light.Ambient.Set(<Lua Colour>)         | Sets the light's Ambient colour to   |");
			ImGui::Text("	|                                                | the input LuaColour                  |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|              Light.Ambient.Get()               | Gets the light's Ambient colour as a |");
			ImGui::Text("	|                                                | LuaColour                            |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|         Light.PointDiffuse.<r, g or b>         | The red, green and blue values of    |");
			ImGui::Text("   |                                                | the light's Point Diffuse colour     |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|      Light.PointDiffuse.Set(<Lua Colour>)      | Sets the light's Point Diffuse       |");
			ImGui::Text("	|                                                | colour to the input LuaColour        |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|            Light.PointDiffuse.Get()            | Gets the light's Point Diffuse       |");
			ImGui::Text("	|                                                | colour as a LuaColour                |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|        Light.PointSpecular.<r, g or b>         | The red, green and blue values of    |");
			ImGui::Text("   |                                                | the light's Point Specular colour    |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|     Light.PointSpecular.Set(<Lua Colour>)      | Sets the light's Point Specular      |");
			ImGui::Text("	|                                                | colour to the input LuaColour        |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|           Light.PointSpecular.Get()            | Gets the light's Point Specular      |");
			ImGui::Text("	|                                                | colour as a LuaColour                |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|                                                | The red, green and blue values of    |");
			ImGui::Text("   |     Light.DirectionalDiffuse.<r, g or b>       | the light's Directional Diffuse      |");
			ImGui::Text("	|                                                | light colour                         |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|   Light.DirectionalDiffuse.Set(<Lua Colour>)   | Sets the light's Directional Diffuse |");
			ImGui::Text("	|                                                | colour to the input LuaColour        |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|         Light.DirectionalDiffuse.Get()         | Gets the light's Directional Diffuse |");
			ImGui::Text("	|                                                | colour as a LuaColour                |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|                                                | The red, green and blue values of    |");
			ImGui::Text("   |     Light.DirectionalSpecular.<r, g or b>      | the light's Directional Specular     |");
			ImGui::Text("	|                                                | colour                               |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|                                                | Sets the light's Directional         |");
			ImGui::Text("	|  Light.DirectionalSpecular.Set(<Lua Colour>)   | Specular colour to the input         |");
			ImGui::Text("	|                                                | LuaColour                            |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|        Light.DirectionalSpecular.Get()         | Gets the light's Directional         |");
			ImGui::Text("	|                                                | Specular colour as a LuaColour       |");
			ImGui::Text("	|------------------------------------------------|--------------------------------------|");
			ImGui::Text("	|               Light.Attenuation                | The attenuation value of the light   |");
			ImGui::Text("	-----------------------------------------------------------------------------------------");
#pragma endregion

			// Raycasting
#pragma region Raycasting
			// Raycasting Paragraph
#pragma region Raycasting
			ImGui::NewLine();
			ImGui::Text("Raycasting:");
			ImGui::Text("Raycasting is the process of creating a ray from a position that moves a given length");
			ImGui::Text("through the game world in a specified direction checking for collisions with an object.");
			ImGui::Text("Each of the following code examples exist within the Raycasting namespace. Placeholder");
			ImGui::Text("text is indicated with <>.");
			ImGui::NewLine();
			ImGui::Text("	-----------------------------------------------------------------------------------------");
			ImGui::Text("	|                          Code                           |         Description         |");
			ImGui::Text("	|---------------------------------------------------------|-----------------------------|");
			ImGui::Text("	|                                                         | Will create a raycast       |");
			ImGui::Text("	|                                                         | object within the physics   |");
			ImGui::Text("	|                                                         | system. In the example code |");
			ImGui::Text("	|                                                         | the created ray will begin  |");
			ImGui::Text("	|                                                         | from the object with an     |");
			ImGui::Text("   |                                                         | offset of the first vector, |");
			ImGui::Text("	|                                                         | head in the direction of    |");
			ImGui::Text("	|  CreateRayFromMe(<Lua Vector 1>, <Lua Vector 2>, 100)   | the second and have a       |");
			ImGui::Text("	|                                                         | length of 100. The physics  |");
			ImGui::Text("	|                                                         | system will calculate the   |");
			ImGui::Text("	|                                                         | current result of the       |");
			ImGui::Text("	|                                                         | raycast after every frame.  |");
			ImGui::Text("	|                                                         | The function will return    |");
			ImGui::Text("	|                                                         | the index of the ray within |");
			ImGui::Text("	|                                                         | the physics system.         |");
			ImGui::Text("	|---------------------------------------------------------|-----------------------------|");
			ImGui::Text("	|                                                         | Will create a raycast       |");
			ImGui::Text("	|                                                         | object within the physics   |");
			ImGui::Text("	|                                                         | system. In the example code |");
			ImGui::Text("	|                                                         | the created ray will begin  |");
			ImGui::Text("	|                                                         | from the point of the first |");
			ImGui::Text("	|                                                         | LuaVector, head in the      |");
			ImGui::Text("	|                                                         | direction of the second     |");
			ImGui::Text("	| CreateRayFromPoint(<Lua Vector 1>, <Lua Vector 2>, 100) | LuaVector and have a length |");
			ImGui::Text("	|                                                         | of 100. The physics system  |");
			ImGui::Text("	|                                                         | will calculate the result   |");
			ImGui::Text("	|                                                         | of the raycast inbetween    |");
			ImGui::Text("	|                                                         | the current and next        |");
			ImGui::Text("	|                                                         | frames. The function will   |");
			ImGui::Text("	|                                                         | return the index of the ray |");
			ImGui::Text("	|                                                         | within the physics system.  |");
			ImGui::Text("	|---------------------------------------------------------|-----------------------------|");
			ImGui::Text("	|                                                         | Casts a ray without         |");
			ImGui::Text("	|                                                         | registering it with the     |");
			ImGui::Text("	|                                                         | physics system. In the      |");
			ImGui::Text("	|                                                         | example the cast ray will   |");
			ImGui::Text("	|                                                         | start from the object with  |");
			ImGui::Text("	|                                                         | an offset of the first      |");
			ImGui::Text("	|   CastRayFromMe(<Lua Vector 1>, <Lua Vector 2>, 100)    | LuaVector, move in the      |");
			ImGui::Text("	|                                                         | direction of the second     |");
			ImGui::Text("	|                                                         | LuaVector and have a length |");
			ImGui::Text("	|                                                         | of 100. Returns a           |");
			ImGui::Text("	|                                                         | RaycastHit object that      |");
			ImGui::Text("	|                                                         | holds information about the |");
			ImGui::Text("	|                                                         | ray hit                     |");
			ImGui::Text("	|---------------------------------------------------------|-----------------------------|");
			ImGui::Text("	|                                                         | Casts a ray without         |");
			ImGui::Text("	|                                                         | registering it with the     |");
			ImGui::Text("	|                                                         | physics system. In the      |");
			ImGui::Text("	|                                                         | example the cast ray will   |");
			ImGui::Text("	|                                                         | start at the point of the   |");
			ImGui::Text("	|  CastRayFromPoint(<Lua Vector 1>, <Lua Vector 2>, 100)  | first LuaVector, move in    |");
			ImGui::Text("	|                                                         | the direction of the second |");
			ImGui::Text("	|                                                         | LuaVector and have a length |");
			ImGui::Text("	|                                                         | of 100. Returns a           |");
			ImGui::Text("	|                                                         | RaycastHit object that      |");
			ImGui::Text("	|                                                         | holds information about the |");
			ImGui::Text("	|                                                         | ray hit.                    |");
			ImGui::Text("	|---------------------------------------------------------|-----------------------------|");
			ImGui::Text("	|                                                         | Will set the start position |");
			ImGui::Text("	|      SetRayStartPoint(<Ray Index>, <A Lua Vector>)      | of the ray with the given   |");
			ImGui::Text("	|                                                         | index to the position in    |");
			ImGui::Text("	|                                                         | the LuaVector               |");
			ImGui::Text("	|---------------------------------------------------------|-----------------------------|");
			ImGui::Text("	|                                                         | Will set the direction of   |");
			ImGui::Text("	|      SetRayDirection(<Ray Index>, <A Lua Vector>)       | the ray with the given      |");
			ImGui::Text("	|                                                         | index to the direction in   |");
			ImGui::Text("	|                                                         | the LuaVector               |");
			ImGui::Text("	|---------------------------------------------------------|-----------------------------|");
			ImGui::Text("	|                                                         | Will set the length of the  |");
			ImGui::Text("	|             SetRayLength(<Ray Index>, 500)              | ray with the given index to |");
			ImGui::Text("	|                                                         | 500                         |");
			ImGui::Text("	|---------------------------------------------------------|-----------------------------|");
			ImGui::Text("	|                                                         | Will return the length of   |");
			ImGui::Text("	|                GetRayLength(<Ray Index>)                | the ray with the given      |");
			ImGui::Text("	|                                                         | index                       |");
			ImGui::Text("	|---------------------------------------------------------|-----------------------------|");
			ImGui::Text("	|                 DisableRay(<Ray Index>)                 | Will disable the ray with   |");
			ImGui::Text("	|                                                         | the given index             |");
			ImGui::Text("	|---------------------------------------------------------|-----------------------------|");
			ImGui::Text("	|                 EnableRay(<Ray Index>)                  | Will enable the ray with    |");
			ImGui::Text("	|                                                         | the given index             |");
			ImGui::Text("	|---------------------------------------------------------|-----------------------------|");
			ImGui::Text("	|                                                         | Returns a RaycastHit object |");
			ImGui::Text("	|               GetRayHitInfo(<Ray Index>)                | that holds information      |");
			ImGui::Text("	|                                                         | about the collision of the  |");
			ImGui::Text("	|                                                         | ray with the given index    |");
			ImGui::Text("	-----------------------------------------------------------------------------------------");
#pragma endregion

			// Raycast Hit Object Paragraph
#pragma region RaycastHitObject
			ImGui::NewLine();
			ImGui::Text("RaycastHit:");
			ImGui::Text("The data from a raycasts collision with an object is stored in a RaycastHit object. The");
			ImGui::Text("collected data can be accessed as follows. Placeholder text is indicated with <>.");
			ImGui::NewLine();
			ImGui::Text("	-----------------------------------------------------------------------------------------");
			ImGui::Text("	|              Code               |                     Description                     |");
			ImGui::Text("	|---------------------------------|-----------------------------------------------------|");
			ImGui::Text("	|   <RaycastHit Object>.DidHit    | Boolean that holds wether the ray actually collided |");
			ImGui::Text("	|                                 | with an object                                      |");
			ImGui::Text("	|---------------------------------|-----------------------------------------------------|");
			ImGui::Text("	| <RaycastHit Object>.HitDistance | Float holding the distance from the ray's start     |");
			ImGui::Text("	|                                 | point the collision happened.                       |");
			ImGui::Text("	|---------------------------------|-----------------------------------------------------|");
			ImGui::Text("	| <RaycastHit Object>.ObjectName  | String that holds the name of the collided object   |");
			ImGui::Text("	|---------------------------------|-----------------------------------------------------|");
			ImGui::Text("	|  <RaycastHit Object>.ObjectTag  | String that holds the collided object's tag         |");
			ImGui::Text("	-----------------------------------------------------------------------------------------");
#pragma endregion
#pragma endregion
			
			// Prefabs
#pragma region Prefabs
			// Prefabs Namespace Paragraph
#pragma region PrefabNamespace
			ImGui::NewLine();
			ImGui::Text("Prefab Namespace:");
			ImGui::Text("The prefab system allows you to create game objects during runtime from a prefab object,");
			ImGui::Text("that acts as a blueprint from which the engine can create the object. The prefab namespace");
			ImGui::Text("has two functions for creating objects from prefabs:");
			ImGui::NewLine();
			ImGui::Text("	-----------------------------------------------------------------------------------------");
			ImGui::Text("	|                           Code                           |        Description         |");
			ImGui::Text("	|----------------------------------------------------------|----------------------------|");
			ImGui::Text("	|          Prefab.Instantiate(<An Object Prefab>)          | Creates a game object from |");
			ImGui::Text("	|                                                          | a prefab.                  |");
			ImGui::Text("	|----------------------------------------------------------|----------------------------|");
			ImGui::Text("	|                                                          | Creates a game object from |");
			ImGui::Text("	|                                                          | a prefab at the location   |");
			ImGui::Text("	|                                                          | given by the LuaVector.    |");
			ImGui::Text("	| Prefab.InstantiateAt(<An Object Prefab>, <A Lua Vector>) | Using this function        |");
			ImGui::Text("	|                                                          | ignores any position       |");
			ImGui::Text("	|                                                          | already assigned to the    |");
			ImGui::Text("	|                                                          | prefab.                    |");
			ImGui::Text("	-----------------------------------------------------------------------------------------");
			ImGui::NewLine();
			ImGui::Text("The prefab namespace is also used to create prefab objects. Prefabs can be created with");
			ImGui::Text("any of the default models given by the engine or a blank one can be created without one.");
			ImGui::Text("Either way, another model can be assigned to it later.");
			ImGui::NewLine();
			ImGui::Text("	---------------------------------------------------------------------------------");
			ImGui::Text("	|       Code        |                        Description                        |");
			ImGui::Text("	|-------------------|-----------------------------------------------------------|");
			ImGui::Text("	|  Prefab.Blank()   |    Creates and returns a prefab object without a model    |");
			ImGui::Text("	|-------------------|-----------------------------------------------------------|");
			ImGui::Text("	|   Prefab.Cube()   |   Creates and returns a prefab object with a cube model   |");
			ImGui::Text("	|-------------------|-----------------------------------------------------------|");
			ImGui::Text("	| Prefab.Capsule()  | Creates and returns a prefab object with a capsule model  |");
			ImGui::Text("	|-------------------|-----------------------------------------------------------|");
			ImGui::Text("	| Prefab.Cylinder() | Creates and returns a prefab object with a cylinder model |");
			ImGui::Text("	|-------------------|-----------------------------------------------------------|");
			ImGui::Text("	|   Prefab.Quad()   |   Creates and returns a prefab object with a quad model   |");
			ImGui::Text("	|-------------------|-----------------------------------------------------------|");
			ImGui::Text("	|  Prefab.Sphere()  |  Creates and returns a prefab object with a sphere model  |");
			ImGui::Text("	---------------------------------------------------------------------------------");
#pragma endregion 

			// Prefab Objects Paragraph
#pragma region PrefabObjects
			ImGui::NewLine();
			ImGui::Text("Prefab Objects:");
			ImGui::Text("A created prefab object holds all the information neccessary to create an object during");
			ImGui::Text("runtime. This information ");
			ImGui::NewLine();
			ImGui::Text("	-----------------------------------------------------------------------------------------");
			ImGui::Text("	|                            Code                             |       Description       |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                <A Prefab Object>.ObjectName                 | The name of the created |");
			ImGui::Text("	|                                                             | object                  |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                 <A Prefab Object>.ObjectTag                 | The tag attached to the |");
			ImGui::Text("	|                                                             | created object          |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | The filepath to the     |");
			ImGui::Text("	|                                                             | objects model. The      |");
			ImGui::Text("	|                                                             | Model namespace,        |");
			ImGui::Text("	|                   <A Prefab Object>.Model                   | contains the filepaths  |");
			ImGui::Text("	|                                                             | to all of the default   |");
			ImGui::Text("	|                                                             | models stored by the    |");
			ImGui::Text("	|                                                             | engine                  |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | A LuaVector object that |");
			ImGui::Text("	|                                                             | holds the prefabs       |");
			ImGui::Text("	|                                                             | position. This will     |");
			ImGui::Text("	|                                                             | only be used in you     |");
			ImGui::Text("	|                 <A Prefab Object>.Position                  | call the Instaniate     |");
			ImGui::Text("	|                                                             | function as the	        |");
			ImGui::Text("	|                                                             | InstantiateAt function  |");
			ImGui::Text("	|                                                             | will create the object  |");
			ImGui::Text("	|                                                             | at the given position.  |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | A LuaVector object that |");
			ImGui::Text("	|                 <A Prefab Object>.Rotation                  | holds the prefabs       |");
			ImGui::Text("	|                                                             | rotation                |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                   <A Prefab Object>.Scale                   | A LuaVector object that |");
			ImGui::Text("	|                                                             | holds the prefabs scale |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | A LuaColour object that |");
			ImGui::Text("	|              <A Prefab Object>.MaterialDiffuse              | holds the prefab's      |");
			ImGui::Text("	|                                                             | material's diffuse      |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | A LuaColour object that |");
			ImGui::Text("	|             <A Prefab Object>.MaterialSpecular              | holds the prefab's      |");
			ImGui::Text("	|                                                             | material's specular     |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|             <A Prefab Object>.MaterialShininess             | The shininess of the    |");
			ImGui::Text("	|                                                             | prefab's material       |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|              <A Prefab Object>.TextureFilepath              | The filepath to the     |");
			ImGui::Text("	|                                                             | object's texture        |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | Defines a dynamic rigid |");
			ImGui::Text("	|                                                             | body component that     |");
			ImGui::Text("	|                                                             | will be added to the    |");
			ImGui::Text("	|                                                             | created object. The     |");
			ImGui::Text("	|                                                             | input parameters are    |");
			ImGui::Text("	|                                                             | the body's bounciness,  |");
			ImGui::Text("	|                                                             | friction, roll          |");
			ImGui::Text("	| <A Prefab Object>:AddDynamicRigidBody(0.7, 0.15, 0.0, true) | resistance and whether  |");
			ImGui::Text("	|                                                             | or not it is affected   |");
			ImGui::Text("	|                                                             | by gravity              |");
			ImGui::Text("	|                                                             | respectively. The       |");
			ImGui::Text("	|                                                             | bounciness, friction    |");
			ImGui::Text("	|                                                             | and roll resistance     |");
			ImGui::Text("	|                                                             | must be with the range  |");
			ImGui::Text("	|                                                             | 0 <= x >= 1.            |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | Defines a static rigid  |");
			ImGui::Text("	|                                                             | body component that     |");
			ImGui::Text("	|                                                             | will be added to the    |");
			ImGui::Text("	|                                                             | created object. The     |");
			ImGui::Text("	|                                                             | input parameters are    |");
			ImGui::Text("	|                                                             | the body's bounciness,  |");
			ImGui::Text("	|    <A Prefab Object>:AddStaticRigidBody(0.7, 0.15, 0.0)     | friction and roll       |");
			ImGui::Text("	|                                                             | resistance              |");
			ImGui::Text("	|                                                             | respectively. The       |");
			ImGui::Text("	|                                                             | bounciness, friction    |");
			ImGui::Text("	|                                                             | and roll resistance     |");
			ImGui::Text("	|                                                             | must be with the range  |");
			ImGui::Text("	|                                                             | 0 <= x >= 1.            |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | Defines a kinematic     |");
			ImGui::Text("	|                                                             | rigid body component    |");
			ImGui::Text("	|                                                             | that will be added to   |");
			ImGui::Text("	|                                                             | the created object. The |");
			ImGui::Text("	|                                                             | input parameters are    |");
			ImGui::Text("	|                                                             | the body's bounciness,  |");
			ImGui::Text("	|   <A Prefab Object>:AddKinematicRigidBody(0.7, 0.15, 0.0)   | friction and roll       |");
			ImGui::Text("	|                                                             | resistance              |");
			ImGui::Text("	|                                                             | respectively. The       |");
			ImGui::Text("	|                                                             | bounciness, friction    |");
			ImGui::Text("	|                                                             | and roll resistance     |");
			ImGui::Text("	|                                                             | must be with the range  |");
			ImGui::Text("	|                                                             | 0 <= x >= 1.            |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | Defines a box collider  |");
			ImGui::Text("	|                                                             | component that will be  |");
			ImGui::Text("	|                                                             | added to the created    |");
			ImGui::Text("	|                                                             | object. The input       |");
			ImGui::Text("	|                                                             | parameters are the      |");
			ImGui::Text("	|                                                             | colliders half extents  |");
			ImGui::Text("	|   <A Prefab Object>:AddBoxCollider(<A Lua Vector>, 1, 1)    | in a lua vector, the    |");
			ImGui::Text("	|                                                             | colliders mass and it's |");
			ImGui::Text("	|                                                             | collison category       |");
			ImGui::Text("	|                                                             | respectively. The       |");
			ImGui::Text("	|                                                             | collider mass must be   |");
			ImGui::Text("	|                                                             | with the range 0 <= x   |");
			ImGui::Text("	|                                                             | >= 1.                   |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | Defines a sphere        |");
			ImGui::Text("	|                                                             | collider component that |");
			ImGui::Text("	|                                                             | will be added to the    |");
			ImGui::Text("	|         <A Prefab Object>:AddSphereCollider(0.5, 1)         | created object. The     |");
			ImGui::Text("	|                                                             | input parameters are    |");
			ImGui::Text("	|                                                             | the collider's radius   |");
			ImGui::Text("	|                                                             | and it's collision      |");
			ImGui::Text("	|                                                             | category respectively.  |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | Defines a capsule       |");
			ImGui::Text("	|                                                             | collider component that |");
			ImGui::Text("	|                                                             | will be added to the    |");
			ImGui::Text("	|                                                             | created object. The     |");
			ImGui::Text("	|       <A Prefab Object>:AddCapsuleCollider(0.5, 1, 1)       | input parameters are    |");
			ImGui::Text("	|                                                             | the raidus of the ends  |");
			ImGui::Text("	|                                                             | of the capusle, the     |");
			ImGui::Text("	|                                                             | length of the capsule   |");
			ImGui::Text("	|                                                             | and it's collision      |");
			ImGui::Text("	|                                                             | category respectively.  |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | Defines a convex mesh   |");
			ImGui::Text("	|                                                             | collider component that |");
			ImGui::Text("	|                                                             | will be added to the    |");
			ImGui::Text("	|                                                             | created object. The     |");
			ImGui::Text("	| <A Prefab Object>:AddConvexMeshCollider(<Mesh Filepath>, 1) | input parameters are    |");
			ImGui::Text("	|                                                             | the filepath to the     |");
			ImGui::Text("	|                                                             | mesh and the collider's |");
			ImGui::Text("	|                                                             | collision category      |");
			ImGui::Text("	|                                                             | respectively.           |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | Defines the collision   |");
			ImGui::Text("	|                                                             | categories that a       |");
			ImGui::Text("	|                                                             | collider will be able   |");
			ImGui::Text("	|                                                             | to collide with. The    |");
			ImGui::Text("	|                                                             | input booleans set      |");
			ImGui::Text("	|  <A Prefab Object>:SetCollidables(true, true, true, true)   | whether collisions can  |");
			ImGui::Text("	|                                                             | happen for each         |");
			ImGui::Text("	|                                                             | category sequentially.  |");
			ImGui::Text("	|                                                             | The example code will   |");
			ImGui::Text("	|                                                             | allow for collisions    |");
			ImGui::Text("	|                                                             | with all four collision |");
			ImGui::Text("	|                                                             | categories.             |");
			ImGui::Text("	|-------------------------------------------------------------|-------------------------|");
			ImGui::Text("	|                                                             | Defines a Lua Script    |");
			ImGui::Text("	|                                                             | component that will be  |");
			ImGui::Text("	|  <A Prefab Object>:AddLuaScript(<Filepath to the Script>)   | added to the created    |");
			ImGui::Text("	|                                                             | object. The input       |");
			ImGui::Text("	|                                                             | parameter is the        |");
			ImGui::Text("	|                                                             | filepath to the script. |");
			ImGui::Text("	-----------------------------------------------------------------------------------------");
#pragma endregion
#pragma endregion

			// End Window
			ImGui::End();
		}

		// Create Remove Component Button
		ImGui::NewLine(); // Add Line Break
		sWidgitName = "Remove Component###DeleteLuaScriptComponentButton" + std::to_string(number); // Set Widgit Name
		if (ImGui::Button(sWidgitName.c_str())) // Create Remove Component Button
			RemoveLuaScriptComponent(pObjects->at(i)); // Assign Remove Lua Script Function

		// Unindent GUI
		ImGui::Unindent(16.f);
	}
}

void ChangeObjects::changeMesh(int i)
{
	// Set Collapsable Header Name
	std::string sFInputName = "Mesh:###Mesh" + std::to_string(i);

	// Create Mesh GUI
	if (ImGui::CollapsingHeader(sFInputName.c_str()))
	{
		ImGui::Indent(16.f);
		// List models in default model folder and project model folder
		sFInputName = "###ModelChoice" + std::to_string(i); // Set Widget Name
		ImGui::Text("Model:");
		if (ImGui::BeginCombo(sFInputName.c_str(), parseFilePath(s_displayLayer->m_layerData.m_objects.at(i).m_modelFilepath).c_str())) // Begin Creating List
		{
			for (const auto& entry : std::filesystem::directory_iterator("assets/models/"))
			{
				std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
				if (fileType.compare(".mtl") != 0)
				{
					std::string name = parseFilePath(entry.path().string());

					sFInputName = name + "###" + name + std::to_string(i);
					bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_modelFilepath); // Check If Selected
					if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
					{
						s_displayLayer->m_layerData.m_objects.at(i).m_modelFilepath = entry.path().string();
						s_displayLayer->getMeshes().at(s_displayLayer->m_layerData.m_objects.at(i).m_meshComponentVectorValue)->setModel(s_displayLayer->m_layerData.m_objects.at(i).m_modelFilepath);
					}
				}
			}
			ImGui::NewLine();

			std::string filepath = "assets/savedJSON/" + s_sProjectName + "/models/";
			for (const auto& entry : std::filesystem::directory_iterator(filepath))
			{
				std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
				if (fileType.compare(".txt") != 0 && fileType.compare(".mtl") != 0)
				{
					std::string name = parseFilePath(entry.path().string());

					sFInputName = name + "###" + name + std::to_string(i);
					bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_modelFilepath); // Check If Selected
					if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
					{
						s_displayLayer->m_layerData.m_objects.at(i).m_modelFilepath = entry.path().string();
						s_displayLayer->getMeshes().at(s_displayLayer->m_layerData.m_objects.at(i).m_meshComponentVectorValue)->setModel(s_displayLayer->m_layerData.m_objects.at(i).m_modelFilepath);
					}
				}
			}

			// Stop Creating List
			ImGui::EndCombo();
		}

		ImGui::Unindent(16.f);
		ImGui::NewLine();
	}
}

// Change Material
void ChangeObjects::changeMaterial(int i)
{
	// Set Collapsable Header Name
	std::string sFInputName = "Material:###Mat" + std::to_string(i);

	// Create Material GUI
	if (ImGui::CollapsingHeader(sFInputName.c_str()))
	{
		// Indent GUI
		ImGui::Indent(16.f);

		// Get Object Material Information
		std::shared_ptr<Engine::MaterialComponent> mat = s_displayLayer->getMaterials().at(s_displayLayer->m_layerData.m_objects.at(i).m_materialComponentVectorValue); // Create Pointer to Material

		glm::vec3 diffuse = mat->getDiffuse(); // Get Material's Diffuse Value
		float editDiff[3] = { diffuse.x, diffuse.y, diffuse.z }; // Save Diffuse as Array of Floats for Colour Edit
		glm::vec3 specular = mat->getSpecular(); // Get Material's Specular Value
		float editSpec[3] = { specular.x, specular.y, specular.z }; // Save Specular as Array of Floats for Colour Edit
		float shininess = mat->getShininess(); // Get Materials Shininess Value
		float usedShininess = (shininess - 5.f) / 1500.f; // Get Shininess as Decimal for Float Slider

		// Create Material UI
		ImGui::PushItemWidth(300.f); // Set Widget Width
		ImGui::Text("Diffuse:"); // Indicate Diffuse Colour Input
		std::string fieldName = "###DiffuseColour" + std::to_string(i); // Define Colour Edit Name
		ImGui::ColorEdit3(fieldName.c_str(), editDiff); // Create Diffuse Colour Input
		ImGui::NewLine(); // Insert Line Break
		ImGui::Text("Specular:"); // Indicate Specular Colour Input
		fieldName = "###SpecularColour" + std::to_string(i); // Define Specular Colour Edit Name
		ImGui::ColorEdit3(fieldName.c_str(), editSpec); // Create Specular Colour Input
		ImGui::NewLine(); // Insert Line Break
		ImGui::Text("Shininess:"); // Indicate Shininess Float Slider
		fieldName = "###Shininess" + std::to_string(i); // Define Shininess Slider Name
		ImGui::SliderFloat(fieldName.c_str(), &usedShininess, 0.f, 1.f); // Create Shininess Float Slider
		ImGui::NewLine();

		sFInputName = "Textures###Textures" + std::to_string(i);
		if (ImGui::CollapsingHeader(sFInputName.c_str()))
		{
			changeTextures(i);
		}
		ImGui::NewLine();

		// Control Shininess
		shininess = (usedShininess * 1500.f) + 5.f; // Get Shininess on 5 to 1505 Scale

		// Set Object New Material
		mat->setDiffuse(glm::vec3(editDiff[0], editDiff[1], editDiff[2])); // Set Diffuse
		mat->setSpecular(glm::vec3(editSpec[0], editSpec[1], editSpec[2])); // Set Specular
		mat->setShininess(shininess); // Set Shininess

		// Unindent GUI
		ImGui::Unindent(16.f);
	}
}

void ChangeObjects::changeTextures(int i)
{
	// Make a dropdown menu for all the types of texture, listing all the textures found in the folders
	ImGui::Indent(16.f);
	std::string sFInputName = "###DiffuseTextureChoice" + std::to_string(i); // Set Widget Name
	ImGui::Text("Diffuse Texture:");
	if (ImGui::BeginCombo(sFInputName.c_str(), parseFilePath(s_displayLayer->m_layerData.m_objects.at(i).m_diffuseTextureFilepath).c_str())) // Begin Creating List
	{
		for (const auto& entry : std::filesystem::directory_iterator("assets/textures"))
		{
			std::string name = parseFilePath(entry.path().string());

			sFInputName = name + "###" + name + std::to_string(i);
			bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_diffuseTextureFilepath); // Check If Selected

			if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
			{
				s_displayLayer->m_layerData.m_objects.at(i).m_diffuseTextureFilepath = entry.path().string();

				std::shared_ptr<Engine::Texture> texture = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_diffuseTextureFilepath);
				s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->setDiffuse(texture);
			}
		}
		ImGui::NewLine();
		for (const auto& entry : std::filesystem::directory_iterator("assets/savedJSON/" + s_sProjectName + "/textures/"))
		{
			std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
			if (fileType.compare(".txt") != 0)
			{
				std::string name = parseFilePath(entry.path().string());

				sFInputName = name + "###" + name + std::to_string(i);
				bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_diffuseTextureFilepath); // Check If Selected

				if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
				{
					s_displayLayer->m_layerData.m_objects.at(i).m_diffuseTextureFilepath = entry.path().string();

					std::shared_ptr<Engine::Texture> texture = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_diffuseTextureFilepath);
					s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->setDiffuse(texture);
				}
			}
		}

		// Stop Creating List
		ImGui::EndCombo();
	}

	std::shared_ptr<Engine::Texture> tex = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_diffuseTextureFilepath);
	unsigned int texID = tex->getID();
	glm::vec2 texSize = glm::vec2(tex->getWidth(), tex->getHeight());

	// If a texture is selected, display it
	if (parseFilePath(s_displayLayer->m_layerData.m_objects.at(i).m_diffuseTextureFilepath).compare("None") != 0)
	{
		ImVec2 size(texSize.x, texSize.y);
		float aspect = size.x / size.y;
		size = ImVec2(100 * aspect, 100);
		ImGui::Image((void*)(intptr_t)texID, size);
	}
	ImGui::NewLine();

	sFInputName = "###SpecularTextureChoice" + std::to_string(i); // Set Widget Name
	ImGui::Text("Specular Texture:");
	if (ImGui::BeginCombo(sFInputName.c_str(), parseFilePath(s_displayLayer->m_layerData.m_objects.at(i).m_specularTextureFilepath).c_str())) // Begin Creating List
	{
		for (const auto& entry : std::filesystem::directory_iterator("assets/textures"))
		{
			std::string name = parseFilePath(entry.path().string());

			sFInputName = name + "###" + name + std::to_string(i);
			bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_specularTextureFilepath); // Check If Selected

			if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
			{
				s_displayLayer->m_layerData.m_objects.at(i).m_specularTextureFilepath = entry.path().string();

				std::shared_ptr<Engine::Texture> texture = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_specularTextureFilepath);
				s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->setSpecular(texture);
			}
		}
		ImGui::NewLine();
		for (const auto& entry : std::filesystem::directory_iterator("assets/savedJSON/" + s_sProjectName + "/textures/"))
		{
			std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
			if (fileType.compare(".txt") != 0)
			{
				std::string name = parseFilePath(entry.path().string());

				sFInputName = name + "###" + name + std::to_string(i);
				bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_specularTextureFilepath); // Check If Selected

				if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
				{
					s_displayLayer->m_layerData.m_objects.at(i).m_specularTextureFilepath = entry.path().string();

					std::shared_ptr<Engine::Texture> texture = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_specularTextureFilepath);
					s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->setSpecular(texture);
				}
			}
		}

		// Stop Creating List
		ImGui::EndCombo();
	}

	tex = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_specularTextureFilepath);
	texID = tex->getID();
	texSize = glm::vec2(tex->getWidth(), tex->getHeight());

	// If a texture is selected, display it
	if (parseFilePath(s_displayLayer->m_layerData.m_objects.at(i).m_specularTextureFilepath).compare("None") != 0)
	{
		ImVec2 size(texSize.x, texSize.y);
		float aspect = size.x / size.y;
		size = ImVec2(100 * aspect, 100);
		ImGui::Image((void*)texID, size);
	}
	ImGui::NewLine();

	sFInputName = "###NormalMapChoice" + std::to_string(i); // Set Widget Name
	ImGui::Text("Normal Map:");
	if (ImGui::BeginCombo(sFInputName.c_str(), parseFilePath(s_displayLayer->m_layerData.m_objects.at(i).m_normalMapFilepath).c_str())) // Begin Creating List
	{
		for (const auto& entry : std::filesystem::directory_iterator("assets/textures"))
		{
			std::string name = parseFilePath(entry.path().string());

			sFInputName = name + "###" + name + std::to_string(i);
			bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_normalMapFilepath); // Check If Selected

			if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
			{
				s_displayLayer->m_layerData.m_objects.at(i).m_normalMapFilepath = entry.path().string();

				std::shared_ptr<Engine::Texture> texture = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_normalMapFilepath);
				s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->setNormalMap(false, texture);

				s_displayLayer->m_layerData.m_objects.at(i).m_hasNormalMap = false;
			}
		}
		ImGui::NewLine();
		for (const auto& entry : std::filesystem::directory_iterator("assets/savedJSON/" + s_sProjectName + "/textures/"))
		{
			std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
			if (fileType.compare(".txt") != 0)
			{
				std::string name = parseFilePath(entry.path().string());

				sFInputName = name + "###" + name + std::to_string(i);
				bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_normalMapFilepath); // Check If Selected

				if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
				{
					s_displayLayer->m_layerData.m_objects.at(i).m_normalMapFilepath = entry.path().string();

					std::shared_ptr<Engine::Texture> texture = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_normalMapFilepath);
					s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->setNormalMap(true, texture);

					s_displayLayer->m_layerData.m_objects.at(i).m_hasNormalMap = true;
				}
			}
		}

		// Stop Creating List
		ImGui::EndCombo();
	}

	tex = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_normalMapFilepath);
	texID = tex->getID();
	texSize = glm::vec2(tex->getWidth(), tex->getHeight());

	// If a texture is selected, display it
	if (parseFilePath(s_displayLayer->m_layerData.m_objects.at(i).m_normalMapFilepath).compare("None") != 0)
	{
		ImVec2 size(texSize.x, texSize.y);
		float aspect = size.x / size.y;
		size = ImVec2(100 * aspect, 100);
		ImGui::Image((void*)texID, size);
	}
	ImGui::NewLine();

	sFInputName = "###DepthMapChoice" + std::to_string(i); // Set Widget Name
	ImGui::Text("Depth Map:");
	if (ImGui::BeginCombo(sFInputName.c_str(), parseFilePath(s_displayLayer->m_layerData.m_objects.at(i).m_depthMapFilepath).c_str())) // Begin Creating List
	{
		for (const auto& entry : std::filesystem::directory_iterator("assets/textures"))
		{
			std::string name = parseFilePath(entry.path().string());

			sFInputName = name + "###" + name + std::to_string(i);
			bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_depthMapFilepath); // Check If Selected

			if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
			{
				s_displayLayer->m_layerData.m_objects.at(i).m_depthMapFilepath = entry.path().string();

				std::shared_ptr<Engine::Texture> texture = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_depthMapFilepath);
				s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->setDepthMap(false, texture);

				s_displayLayer->m_layerData.m_objects.at(i).m_hasDepthMap = false;
			}
		}
		ImGui::NewLine();
		for (const auto& entry : std::filesystem::directory_iterator("assets/savedJSON/" + s_sProjectName + "/textures/"))
		{
			std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
			if (fileType.compare(".txt") != 0)
			{
				std::string name = parseFilePath(entry.path().string());

				sFInputName = name + "###" + name + std::to_string(i);
				bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_depthMapFilepath); // Check If Selected

				if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
				{
					s_displayLayer->m_layerData.m_objects.at(i).m_depthMapFilepath = entry.path().string();

					std::shared_ptr<Engine::Texture> texture = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_depthMapFilepath);
					s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->setDepthMap(true, texture);

					s_displayLayer->m_layerData.m_objects.at(i).m_hasDepthMap = true;
				}
			}
		}

		// Stop Creating List
		ImGui::EndCombo();
	}

	tex = s_displayLayer->getResources()->addTexture(s_displayLayer->m_layerData.m_objects.at(i).m_depthMapFilepath);
	texID = tex->getID();
	texSize = glm::vec2(tex->getWidth(), tex->getHeight());

	// If a texture is selected, display it and make a input field for the height scale
	if (parseFilePath(s_displayLayer->m_layerData.m_objects.at(i).m_depthMapFilepath).compare("None") != 0)
	{
		ImVec2 size(texSize.x, texSize.y);
		float aspect = size.x / size.y;
		size = ImVec2(100 * aspect, 100);
		ImGui::Image((void*)texID, size);

		float heightScale = s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->getHeightScale();
		sFInputName = "###HeightScale" + std::to_string(i);
		ImGui::Text("Height Scale:");
		ImGui::InputFloat(sFInputName.c_str(), &heightScale);
		s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->setHeightScale(heightScale);
	}

	ImGui::Unindent(16.f);
}

// Change Light
void ChangeObjects::changeLight(int i)
{
	// Set Collapsable Header Name
	std::string sFInputName = "Light:###Light" + std::to_string(i);

	// Create Light GUI
	if (ImGui::CollapsingHeader(sFInputName.c_str()))
	{
		// Indent GUI
		ImGui::Indent(16.f);

		// Get Object Light Information
		i = s_displayLayer->m_layerData.m_objects.at(i).m_lightComponentVectorValue; // Get Object
		std::shared_ptr<Engine::LightComponent> light = s_displayLayer->getLights().at(i); // Create Pointer to Light
		glm::vec3 ambient = light->getAmbient(); // Get Light's Diffuse Value
		float editAmb[3] = { ambient.x, ambient.y, ambient.z }; // Save Light as Array of Floats for Colour Edit

		glm::vec3 pointDiffuse = light->getPointDiffuse(); // Get Light's point Diffuse Value
		float editPointDiff[3] = { pointDiffuse.x, pointDiffuse.y, pointDiffuse.z }; // Save Light as Array of Floats for Colour Edit
		glm::vec3 pointSpecular = light->getPointSpecular(); // Get Light's point Specular Value
		float editPointSpec[3] = { pointSpecular.x, pointSpecular.y, pointSpecular.z }; // Save SpecLightular as Array of Floats for Colour Edit

		glm::vec3 dirDiffuse = light->getDirectionalDiffuse(); // Get Light's directional Diffuse Value
		float editDirDiff[3] = { dirDiffuse.x, dirDiffuse.y, dirDiffuse.z }; // Save Light as Array of Floats for Colour Edit
		glm::vec3 dirSpecular = light->getDirectionalSpecular(); // Get Light's directional Specular Value
		float editDirSpec[3] = { dirSpecular.x, dirSpecular.y, dirSpecular.z }; // Save SpecLightular as Array of Floats for Colour Edit

		float att = light->getAttenuation();

		// Create Light UI
		ImGui::PushItemWidth(300.f); // Set Widget Width
		ImGui::Text("Ambient:"); // Indicate Ambient Colour Input
		std::string fieldName = "###AmbientLight" + std::to_string(i); // Define Colour Edit Name
		ImGui::ColorEdit3(fieldName.c_str(), editAmb); // Create Ambient Colour Input
		ImGui::NewLine(); // Insert Line Break
		ImGui::Text("Point Light:");
		ImGui::Text("Diffuse:"); // Indicate Diffuse Colour Input
		fieldName = "###PointDiffuseLight" + std::to_string(i); // Define Colour Edit Name
		ImGui::ColorEdit3(fieldName.c_str(), editPointDiff); // Create Diffuse Colour Input
		ImGui::NewLine(); // Insert Line Break
		ImGui::Text("Specular:"); // Indicate Specular Colour Input
		fieldName = "###PointSpecularLight" + std::to_string(i); // Define Specular Colour Edit Name
		ImGui::ColorEdit3(fieldName.c_str(), editPointSpec); // Create Specular Colour Input

		ImGui::NewLine(); // Insert Line Break
		ImGui::Text("Range:"); // Indicate Attenuation Float Slider
		fieldName = "###Range" + std::to_string(i); // Define Attenuation Slider Name
		ImGui::SliderFloat(fieldName.c_str(), &att, 0.f, 0.8f); // Create Attenuation Float Slider

		ImGui::NewLine(); // Insert Line Break
		ImGui::Text("Directional Light:");
		ImGui::Text("Diffuse:"); // Indicate Diffuse Colour Input
		fieldName = "###DirectionalDiffuseLight" + std::to_string(i); // Define Colour Edit Name
		ImGui::ColorEdit3(fieldName.c_str(), editDirDiff); // Create Diffuse Colour Input
		ImGui::NewLine(); // Insert Line Break
		ImGui::Text("Specular:"); // Indicate Specular Colour Input
		fieldName = "###DirectionalSpecularLight" + std::to_string(i); // Define Specular Colour Edit Name
		ImGui::ColorEdit3(fieldName.c_str(), editDirSpec); // Create Specular Colour Input

		// Set Object New Light
		light->setAmbient(glm::vec3(editAmb[0], editAmb[1], editAmb[2])); // Set Ambient
		light->setPointDiffuse(glm::vec3(editPointDiff[0], editPointDiff[1], editPointDiff[2])); // Set Diffuse
		light->setPointSpecular(glm::vec3(editPointSpec[0], editPointSpec[1], editPointSpec[2])); // Set Specular
		light->setDirectionalDiffuse(glm::vec3(editDirDiff[0], editDirDiff[1], editDirDiff[2])); // Set Diffuse
		light->setDirectionalSpecular(glm::vec3(editDirSpec[0], editDirSpec[1], editDirSpec[2])); // Set Specular
		light->setAttenuation(att); // Set Shininess

		// Unindent GUI
		ImGui::Unindent(16.f);
	}
}

void ChangeObjects::changeCamera(int i)
{
	std::vector<Object>* pObjects = &s_displayLayer->m_layerData.m_objects;

	// Set Collapsable Header Name
	std::string sFInputName = "Camera:###Camera" + std::to_string(i);

	std::shared_ptr<Engine::CameraComponent> camera = s_displayLayer->getCameras().at(pObjects->at(i).m_cameraComponentVectorValue); // Create Pointer to Camera

	// Create Light GUI
	if (ImGui::CollapsingHeader(sFInputName.c_str()))
	{
		// Indent GUI
		ImGui::Indent(16.f);

		// Get Object Camera Information
		std::string type = pObjects->at(i).m_cameraType;
		int camValue = pObjects->at(i).m_cameraComponentVectorValue; // Get Object
		std::shared_ptr<Engine::CameraComponent> camera = s_displayLayer->getCameras().at(camValue); // Create Pointer to Camera

		// Make a dropdown for choosing the camera type
		std::string fieldName = "Camera Type###Type" + std::to_string(camValue);
		if (ImGui::BeginCombo(fieldName.c_str(), pObjects->at(i).m_cameraType.c_str()))
		{
			std::string fieldName = "Free 3D###free3D" + std::to_string(camValue);
			bool selected = !pObjects->at(i).m_cameraType.compare("free3D");
			if (ImGui::Selectable(fieldName.c_str(), selected))
			{
				pObjects->at(i).m_cameraType = "free3D";
			}
			fieldName = "1stPerson (Fixed Y Position)###1stPerson" + std::to_string(camValue);
			selected = !pObjects->at(i).m_cameraType.compare("1stPerson");
			if (ImGui::Selectable(fieldName.c_str(), selected))
			{
				pObjects->at(i).m_cameraType = "1stPerson";
			}
			fieldName = "Fixed 3D###fixed3D" + std::to_string(camValue);
			selected = !pObjects->at(i).m_cameraType.compare("fixed3D");
			if (ImGui::Selectable(fieldName.c_str(), selected))
			{
				pObjects->at(i).m_cameraType = "fixed3D";
			}

			ImGui::EndCombo();
		}

		// Show certain things depending on which type the camera is
		if (pObjects->at(i).m_cameraType.compare("free3D") == 0 ||
			pObjects->at(i).m_cameraType.compare("1stPerson") == 0 ||
			pObjects->at(i).m_cameraType.compare("fixed3D") == 0)
		{
			float fov = camera->getFOV();
			float nearClip = camera->getNearClip();
			float farClip = camera->getFarClip();

			fieldName = "Field of View###fov" + std::to_string(camValue);
			ImGui::SliderFloat(fieldName.c_str(), &fov, 1.f, 179.f);
			fieldName = "Near Clip###near" + std::to_string(camValue);
			ImGui::InputFloat(fieldName.c_str(), &nearClip);
			fieldName = "Far Clip###far" + std::to_string(camValue);
			ImGui::InputFloat(fieldName.c_str(), &farClip);

			camera->updateProjection(fov, nearClip, farClip);

			if (pObjects->at(i).m_cameraType.compare("free3D") == 0 ||
				pObjects->at(i).m_cameraType.compare("1stPerson") == 0)
			{
				bool updateMousePress = camera->getUpdateMousePress();

				fieldName = "Update When Mouse Button is Pressed###updateMouse" + std::to_string(camValue);
				ImGui::Checkbox(fieldName.c_str(), &updateMousePress);

				camera->setUpdateMousePress(updateMousePress);
			}
		}

		ImGui::NewLine();
		ImGui::Text("Layers Seen:");
		// Make a dropdown to choose which layers the camera can see
		fieldName = "###LayersSeen" + std::to_string(i);

		std::vector<std::string> layersSeen = camera->getLayersSeen();

		std::string preview;
		if (layersSeen.size() == 1)
		{
			preview = *layersSeen.begin();
		}
		else if (layersSeen.size() != (s_displayLayer->m_layerData.m_3DLayers.size()) && layersSeen.size() != 0)
		{
			preview = "Mixed";
		}
		else if (layersSeen.size() == (s_displayLayer->m_layerData.m_3DLayers.size()))
		{
			preview = "All";
		}
		else if (layersSeen.size() == 0)
		{
			preview = "None";
		}

		if (ImGui::BeginCombo(fieldName.c_str(), preview.c_str()))
		{
			for (auto& it : s_displayLayer->m_layerData.m_3DLayers)
			{
				fieldName = it + "###" + it + std::to_string(i);
				bool selected = false;

				for (auto& it2 : layersSeen)
				{
					if (it.compare(it2) == 0)
					{
						selected = true;
					}
				}
				if (ImGui::Selectable(fieldName.c_str(), selected))
				{
					if (selected)
					{
						std::vector<std::string>::iterator it2;

						for (it2 = layersSeen.begin(); it2 != layersSeen.end();)
						{
							if (it.compare(*it2) == 0)
							{
								layersSeen.erase(it2);
								it2 = layersSeen.begin();
							}
							if (it2 != layersSeen.end())
							{
								it2++;
							}
						}
					}
					else
					{
						layersSeen.push_back(it);
					}
				}
			}

			ImGui::EndCombo();
		}

		camera->setLayersSeen(layersSeen);

		ImGui::NewLine();

		// Unindent GUI
		ImGui::Unindent(16.f);
	}

	// Update Camera Position and Rotation
	glm::vec3 pos = s_displayLayer->getPositions().at(pObjects->at(i).m_positionComponentVectorValue)->getPosition(); // Get New Position
	glm::vec3 rot = s_displayLayer->getPositions().at(pObjects->at(i).m_positionComponentVectorValue)->getRotation(); // Get New Rotation
	camera->setPosAndRot(pos, glm::vec2(glm::degrees(rot.x), glm::degrees(rot.y))); // Set Position and Rotation
	camera->updateView(); // Update the View
}

void ChangeObjects::changeCollider(int i)
{
	// Set Collapsable Header Name
	std::string sFInputName = "Collider:###Collider" + std::to_string(i);

	// Create Collider GUI
	if (ImGui::CollapsingHeader(sFInputName.c_str()))
	{
		// Indent GUI
		ImGui::Indent(16.f);
		// Make a dropdown for choosing the collider shape
		std::string fieldName = "##Shape" + std::to_string(i);
		ImGui::Text("Collider Shape:");
		ImGui::PushItemWidth(100.f);
		if (ImGui::BeginCombo(fieldName.c_str(), s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.c_str()))
		{
			fieldName = "Box###box" + std::to_string(i);
			bool selected = !s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("box");
			if (ImGui::Selectable(fieldName.c_str(), selected))
			{
				s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape = "box";
			}
			fieldName = "Sphere###sphere" + std::to_string(i);
			selected = !s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("sphere");
			if (ImGui::Selectable(fieldName.c_str(), selected))
			{
				s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape = "sphere";
			}
			fieldName = "Capsule###capsule" + std::to_string(i);
			selected = !s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("capsule");
			if (ImGui::Selectable(fieldName.c_str(), selected))
			{
				s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape = "capsule";
			}
			fieldName = "Convex Mesh (Doesnt work)###convexMesh" + std::to_string(i);
			selected = !s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("convex mesh");
			if (ImGui::Selectable(fieldName.c_str(), selected))
			{
				s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape = "convex mesh";
			}

			ImGui::EndCombo();
		}

		// Create Mass Input Widget
		ImGui::NewLine(); // On a New Line
		ImGui::Text("Mass:"); // Mark as Mass Input
		fieldName = "###mass" + std::to_string(i); // Set the Widget Name
		ImGui::InputFloat(fieldName.c_str(), &(s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_mass)); // Create Float Input Widget

		// Output Relevant Collider GUI
		ImGui::NewLine(); // On a New Line
		if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("box") == 0) // If Using a Box Collider
		{
			// Get Scale
			glm::vec3 halfExtents = s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_halfExtents; // Get Half Extents
			halfExtents *= 2; // Make Representitve of Full Object Scale

			// Output GUI Section Title
			ImGui::Text("Size:");

			// Set Widget Width
			ImGui::PushItemWidth(64.f);

			// X Input
			ImGui::Text("X:"); // Mark as X Input
			ImGui::SameLine(); // Make Same Line as...
			fieldName = "###XColliderSize" + std::to_string(i); // Define Float Box Name String
			ImGui::InputFloat(fieldName.c_str(), &halfExtents.x); // Create X Size Float Input

			// Y Input
			ImGui::SameLine(); // Make Same Line as...
			ImGui::Text("Y:"); // Mark as Y Input
			ImGui::SameLine(); // Make Same Line as...
			fieldName = "###YColliderSize" + std::to_string(i); // Define Float Box Name String
			ImGui::InputFloat(fieldName.c_str(), &halfExtents.y); // Create Y Size Float Input

			// Z Input
			ImGui::SameLine(); // Make Same Line as...
			ImGui::Text("Z:"); // Mark as Z Input
			ImGui::SameLine(); // Make Same Line as...
			fieldName = "###ZColliderSize" + std::to_string(i); // Define Float Box Name String
			ImGui::InputFloat(fieldName.c_str(), &halfExtents.z); // Create Z Size Float Input

			// Get Scale as Half Extents Again
			halfExtents /= 2;

			// Apply Half Extents
			s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_halfExtents = halfExtents;
		}
		else if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("sphere") == 0) // If Using a Sphere Collider
		{
			// Get Radius
			float radius = s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_radius;

			// Output Change Radius GUI
			ImGui::Text("Radius:"); // Mark as Radius Input
			fieldName = "###XColliderRadius" + std::to_string(i); // Define Float Box Name String
			ImGui::InputFloat(fieldName.c_str(), &radius); // Create X Radius Float Input

			// Set New Radius
			s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_radius = radius;
		}
		else if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("capsule") == 0) // If Using a Capsule Collider
		{
			// Get Radius and Height
			float radius = s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_radius;
			float height = s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_height;

			// Output Change Radius and Height GUI
			ImGui::Text("Raidus:"); // Mark as Radius Input
			fieldName = "###XColliderRadius" + std::to_string(i); // Define Float Box Name String
			ImGui::InputFloat(fieldName.c_str(), &radius); // Create X Radius Float Input
			ImGui::NewLine();
			ImGui::Text("Height:"); // Mark as Height Input
			fieldName = "###XColliderHeight" + std::to_string(i); // Define Float Box Name String
			ImGui::InputFloat(fieldName.c_str(), &height); // Create X Height Float Input

			// Save New Radius and Height
			s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_radius = radius;
			s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_height = height;
		}
		else if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("convex mesh") == 0) // If Using a Capsule Collider
		{
			sFInputName = "###ColliderModelChoice" + std::to_string(i); // Set Widget Name
			ImGui::Text("Collider Model:");
			if (ImGui::BeginCombo(sFInputName.c_str(), parseFilePath(s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_modelFilepath).c_str())) // Begin Creating List
			{
				for (const auto& entry : std::filesystem::directory_iterator("assets/models/"))
				{
					std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
					if (fileType.compare(".mtl") != 0)
					{
						std::string name = parseFilePath(entry.path().string());

						sFInputName = name + "###" + name + std::to_string(i);
						bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_modelFilepath); // Check If Selected
						if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
						{
							s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_modelFilepath = entry.path().string();
						}
					}
				}
				ImGui::NewLine();

				std::string filepath = "assets/savedJSON/" + s_sProjectName + "/models/";
				for (const auto& entry : std::filesystem::directory_iterator(filepath))
				{
					std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
					if (fileType.compare(".txt") != 0)
					{
						std::string fileType = entry.path().string().substr(entry.path().string().rfind("."), entry.path().string().length());
						if (fileType.compare(".mtl") != 0)
						{
							std::string name = parseFilePath(entry.path().string());

							sFInputName = name + "###" + name + std::to_string(i);
							bool selected = !entry.path().string().compare(s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_modelFilepath); // Check If Selected
							if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
							{
								s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_modelFilepath = entry.path().string();
							}
						}
					}
				}

				// Stop Creating List
				ImGui::EndCombo();
			}
		}

		if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("capsule") != 0 &&
			s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("convex mesh") != 0)
		{
			// Using Object Scale for Size
			ImGui::Text("Use Object Scale "); // Mark Checkbox Function
			ImGui::SameLine(); // Same Line as...
			fieldName = "###scaleForSize" + std::to_string(i); // Set Widget Name
			ImGui::Checkbox(fieldName.c_str(), &(s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_useScaleForSize)); // Create Checkbox
		}
		else
		{
			if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("capsule") == 0)
				s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_useScaleForSize = false;
			if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("convex mesh") == 0)
				s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_useScaleForSize = true;
		}

		ImGui::NewLine();
		ImGui::Text("Collision Category:");
		sFInputName = "###CollisionCat" + std::to_string(i);
		unsigned int currentCat = s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_category;
		std::string catName = "Category " + std::to_string(currentCat);
		
		ImGui::Indent(16.f);
		if (ImGui::BeginCombo(sFInputName.c_str(), catName.c_str())) // Begin Creating List
		{
			for (int j = 1; j < 5; j++)
			{
				bool selected = false;
				sFInputName = "Category " + std::to_string(j);
				if (catName.compare(sFInputName) == 0)
					selected = true;

				if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
				{
					s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_category = j;
				}
			}
			
			// Stop Creating List
			ImGui::EndCombo();
		}
		ImGui::Unindent(16.f);

		ImGui::NewLine();
		ImGui::Text("Can Collide With:");
		sFInputName = "###CanCollideWith" + std::to_string(i);
		
		int num = 0;
		for (int j = 0; j < 4; j++)
		{
			if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_collideCat[j])
			{
				catName = "Category " + std::to_string(j + 1);
				num++;
			}
		}
		if (num != 1)
			catName = "Mixed";
		if (num == 0)
			catName = "None";
		if (num == 4)
			catName = "All";

		ImGui::Indent(16.f);
		if (ImGui::BeginCombo(sFInputName.c_str(), catName.c_str())) // Begin Creating List
		{
			for (int j = 0; j < 4; j++)
			{
				sFInputName = "Category " + std::to_string(j + 1);

				if (ImGui::Selectable(sFInputName.c_str(), s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_collideCat[j])) // Create Selectable Item
				{
					s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_collideCat[j] = !s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_collideCat[j];
				}
			}

			// Stop Creating List
			ImGui::EndCombo();
		}
		ImGui::Unindent(16.f);

		// Create Remove Component Button
		ImGui::NewLine(); // Add Line Break
		fieldName = "Remove Component###DeleteColliderComponentButton" + std::to_string(i); // Set Widgit Name
		if (ImGui::Button(fieldName.c_str())) // Create Remove Component Button
		{
			// Remove Component
			s_displayLayer->m_layerData.m_objects.at(i).m_hasCollider = false; // Mark as Not Having Collider
			s_displayLayer->m_layerData.m_objects.at(i).m_collider = ColliderData(); // Set Collider Data to Default
			s_displayLayer->m_layerData.m_numColliders--; // Decrease Number of Colliders
		}

		// Unindent GUI
		ImGui::Unindent(16.f);


		glm::vec3 objectScale = s_displayLayer->getPositions().at(s_displayLayer->m_layerData.m_objects.at(i).m_positionComponentVectorValue)->getScale();
		if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("box") == 0)
		{
			glm::vec3 scale = s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_halfExtents;
			scale.x *= 2;
			scale.y *= 2;
			scale.z *= 2;
			Engine::ComponentMessage msg(Engine::ComponentMessageType::ScaleSet, (void*)&scale); // Create Message
			s_displayLayer->getPositions().at(i)->receiveMessage(msg); // Send Message

			s_displayLayer->getMeshes().at(s_displayLayer->m_layerData.m_objects.at(i).m_meshComponentVectorValue)->setModel("assets/models/Cube.obj");
		}
		else if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("sphere") == 0) // If Using Sphere Collider
		{
			float radius = s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_radius;
			glm::vec3 scale = glm::vec3(radius);
			scale.x *= 2;
			scale.y *= 2;
			scale.z *= 2;
			Engine::ComponentMessage msg(Engine::ComponentMessageType::ScaleSet, (void*)&scale); // Create Message
			s_displayLayer->getPositions().at(i)->receiveMessage(msg); // Send Message

			s_displayLayer->getMeshes().at(s_displayLayer->m_layerData.m_objects.at(i).m_meshComponentVectorValue)->setModel("assets/models/Sphere.obj");
		}
		else if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("capsule") == 0) // If Using Capsule Collider
		{
			float radius = s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_radius;
			float height = s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_height;
			height += (radius + radius);
			glm::vec3 scale = glm::vec3(radius * 2, height / 2, radius * 2);
			Engine::ComponentMessage msg(Engine::ComponentMessageType::ScaleSet, (void*)&scale); // Create Message
			s_displayLayer->getPositions().at(i)->receiveMessage(msg); // Send Message

			s_displayLayer->getMeshes().at(s_displayLayer->m_layerData.m_objects.at(i).m_meshComponentVectorValue)->setModel("assets/models/Capsule.obj");
		}
		else if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("convex mesh") == 0) // If Using convex mesh Collider
		{
			s_displayLayer->getMeshes().at(s_displayLayer->m_layerData.m_objects.at(i).m_meshComponentVectorValue)->setModel(s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_modelFilepath);
		}
		// Show the collider in the scene, over the object
		glm::vec3 objectColour = s_displayLayer->getMaterials().at(s_displayLayer->m_layerData.m_objects.at(i).m_materialComponentVectorValue)->getDiffuse();
		s_displayLayer->getMaterials().at(s_displayLayer->m_layerData.m_objects.at(i).m_materialComponentVectorValue)->setDiffuse(glm::vec3(0, 1, 0));

		std::shared_ptr<Engine::Texture> objectTex = s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->getTexture();
		s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->setDiffuse(s_displayLayer->getResources()->addTexture("assets/textures/None.png"));

		s_displayLayer->m_pFrameBuffer->defaultBind();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		s_displayLayer->getRenderer()->submit(s_displayLayer->getMaterials().at(s_displayLayer->m_layerData.m_objects.at(i).m_materialComponentVectorValue)->getMaterial());
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_CULL_FACE);

		s_displayLayer->m_pFrameBuffer->defaultUnbind();

		Engine::ComponentMessage msg(Engine::ComponentMessageType::ScaleSet, (void*)&objectScale); // Create Message
		s_displayLayer->getPositions().at(i)->receiveMessage(msg); // Send Message
		s_displayLayer->getMaterials().at(s_displayLayer->m_layerData.m_objects.at(i).m_materialComponentVectorValue)->setDiffuse(objectColour);
		s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_objects.at(i).m_textureComponentVectorValue)->setDiffuse(objectTex);
		s_displayLayer->getMeshes().at(s_displayLayer->m_layerData.m_objects.at(i).m_meshComponentVectorValue)->setModel(s_displayLayer->m_layerData.m_objects.at(i).m_modelFilepath);
	}

	// Using Scale For Size?
	if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_useScaleForSize) // If We're Using the Objects Scale For It's Collider
	{
		// Get Scale For Relevant Collider
		if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("box") == 0) // If Using Box Collider
		{
			// Set Scale
			s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_halfExtents = s_displayLayer->getPositions().at(s_displayLayer->m_layerData.m_objects.at(i).m_positionComponentVectorValue)->getScale(); // Get Scale
			s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_halfExtents.x /= 2; // Get X Scale Half Extent
			s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_halfExtents.y /= 2; // Get Y Scale Half Extent
			s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_halfExtents.z /= 2; // Get Z Scale Half Extent
		}
		else if (s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_shape.compare("sphere") == 0) // If Using Sphere Collider
		{
			// Set Scale
			s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_radius = s_displayLayer->getPositions().at(s_displayLayer->m_layerData.m_objects.at(i).m_positionComponentVectorValue)->getScale().x; // Get X Scale
			s_displayLayer->m_layerData.m_objects.at(i).m_collider.m_radius /= 2; // Set X Scale Half Extent as Radius
		}
	}
}

void ChangeObjects::changeRigidbody(int i)
{
	// Set Collapsable Header Name
	std::string sFInputName = "Rigidbody:###Rigidbody" + std::to_string(i);

	// Create Collider GUI
	if (ImGui::CollapsingHeader(sFInputName.c_str()))
	{
		// Indent GUI
		ImGui::Indent(16.f);

		// Create Ridgid Body Type Dropdown List
		sFInputName = "###Type" + std::to_string(i); // Set Widget Name
		ImGui::Text("Ridgidbody Type:"); // Mark as RidgidBody Dropdown
		if (ImGui::BeginCombo(sFInputName.c_str(), s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_type.c_str())) // Begin Creating List
		{
			// Define Dynamic Body Input
			sFInputName = "Dynamic###Dynamic" + std::to_string(i); // Set Input Name
			bool selected = !s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_type.compare("dynamic"); // Check If Selected
			if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
				s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_type = "dynamic"; // If Selected Set Body Type to Dynamic

			// Define Static Body Input
			sFInputName = "Static###Static" + std::to_string(i); // Set Input Name
			selected = !s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_type.compare("static"); // Check If Selected
			if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable item
				s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_type = "static"; // If Selected Set Body Type to Static

			// Define Kinematic Body Input
			sFInputName = "Kinematic###Kinematic" + std::to_string(i); // Set Input Name
			selected = !s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_type.compare("kinematic"); // Check If Selected
			if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
				s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_type = "kinematic"; // If Selected Set Body Type to Kinematic

			// Stop Creating List
			ImGui::EndCombo();
		}

		if (s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_type.compare("dynamic") == 0) // If Using a Dynamic Body
		{
			// Create Using Gravity Checkbox
			ImGui::Text("Uses Gravity "); // Mark as Uses Gravity Input
			ImGui::SameLine(); // Make Same Line as...
			sFInputName = "###gravity" + std::to_string(i); // Set Widget Name
			ImGui::Checkbox(sFInputName.c_str(), &(s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_gravity)); // Create Checkbox
		}

		// Line Break
		ImGui::NewLine();

		// Output Body Material GUI
		ImGui::Text("Material:"); // Mark as Material Section of GUI
		ImGui::Indent(16.f); // Indent
		ImGui::Text("Bounciness:"); // Mark Next Input as the Bodies Bounciness Value
		sFInputName = "###bounce" + std::to_string(i); // Set the Widget Name
		ImGui::InputFloat(sFInputName.c_str(), &(s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_bounce)); // Create the Input Float Widget
		ImGui::NewLine(); // On a New Line
		ImGui::Text("Friction:"); // Mark Next Input as the Bodies Friction Value
		sFInputName = "###friction" + std::to_string(i); // Set the Widget Name
		ImGui::InputFloat(sFInputName.c_str(), &(s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_friction)); // Create the Input Float Widget
		if (s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_type.compare("dynamic") == 0) // If Using a Dynamic RidgidBody 
		{
			// Create Roll Resist Widget
			ImGui::NewLine(); // On a New Line
			ImGui::Text("Roll Resist:"); // Mark as Roll Resist Input Float
			sFInputName = "###rollResist" + std::to_string(i); // Set Widget Name
			ImGui::InputFloat(sFInputName.c_str(), &(s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody.m_rollResist)); // Create Input Float
		}
		ImGui::Unindent(16.f); // Unindent GUI

		// Create Remove Component Button
		ImGui::NewLine(); // Add Line Break
		sFInputName = "Remove Component###DeleteRigidbodyComponentButton" + std::to_string(i); // Set Widgit Name
		if (ImGui::Button(sFInputName.c_str())) // Create Remove Component Button
		{
			// Remove RidgidBody Component
			s_displayLayer->m_layerData.m_objects.at(i).m_hasRigidbody = false; // Mark Object as Not Having a RidgidBody
			s_displayLayer->m_layerData.m_objects.at(i).m_rigidBody = RigidBodyData(); // Set RidgidBody Data to Defualt
			s_displayLayer->m_layerData.m_numRigidbodies--; // Decrement Total Number of RigidBodies in Simulation
		}

		// Unindent GUI
		ImGui::Unindent(16.f);
	}
}

void ChangeObjects::changeJoint(int i)
{
	std::string type = s_displayLayer->m_layerData.m_joints.at(i).m_type;
	std::string sFInputName = type + " Joint";

	// Make list to choose the type of joint
	if (ImGui::BeginCombo(sFInputName.c_str(), type.c_str())) // Begin Creating List
	{
		sFInputName = "Ball and Socket";
		bool selected = sFInputName == type;
		if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
		{
			s_displayLayer->m_layerData.m_joints.at(i).m_type = sFInputName;
		}

		sFInputName = "Hinge";
		selected = sFInputName == type;
		if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
		{
			s_displayLayer->m_layerData.m_joints.at(i).m_type = sFInputName;
		}

		sFInputName = "Fixed";
		selected = sFInputName == type;
		if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
		{
			s_displayLayer->m_layerData.m_joints.at(i).m_type = sFInputName;
		}

		sFInputName = "Slider";
		selected = sFInputName == type;
		if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
		{
			s_displayLayer->m_layerData.m_joints.at(i).m_type = sFInputName;
		}

		// Stop Creating List
		ImGui::EndCombo();
	}
	ImGui::NewLine();

	if (type.compare("Ball and Socket") == 0 || type.compare("Hinge") == 0 || type.compare("Fixed") == 0)
	{
		glm::vec3 position = s_displayLayer->m_layerData.m_joints.at(i).m_position;

		// Create Object Position GUI
		ImGui::PushItemWidth(64.f); // Set Widget Width
		ImGui::Text("Position:"); // Output GUI Section Title
		ImGui::Text(" X"); // Indicate X Pos Box
		ImGui::SameLine(); // Make Same Line as...
		sFInputName = "###XPosition" + std::to_string(i); // Define Float Box Name String
		ImGui::InputFloat(sFInputName.c_str(), &position.x); // Create X Pos Float Input
		ImGui::SameLine(); // Make Same Line as...
		ImGui::Text(" Y"); // Indicate Y Pos Box
		ImGui::SameLine(); // Make Same Line as...
		sFInputName = "###YPosition" + std::to_string(i); // Define Float Box Name String
		ImGui::InputFloat(sFInputName.c_str(), &position.y); // Create Y Pos Float Input
		ImGui::SameLine(); // Make Same Line as...
		ImGui::Text(" Z"); // Indicate Z Pos Box
		ImGui::SameLine(); // Make Same Line as...
		sFInputName = "###ZPosition" + std::to_string(i); // Define Float Box Name String
		ImGui::InputFloat(sFInputName.c_str(), &position.z); // Create Z Pos Float Input
		ImGui::NewLine();

		s_displayLayer->m_layerData.m_joints.at(i).m_position = position;
	}
	
	char name[41]; // Create Name Array
	strcpy(name, s_displayLayer->m_layerData.m_joints.at(i).m_bodyAName.c_str()); // Get Object Name

	// Define Global Widgit Name Holder Variable
	sFInputName;

	// Create Object Input Name Widgit Header
	ImGui::Text("Body A Name:");

	// Enter Object Name Widgit
	ImGui::PushItemWidth(200.f); // Set Widgit Width
	sFInputName = "###BodyAName" + std::to_string(s_displayLayer->m_layerData.m_joints.at(i).m_jointVectorValue); // Set Widgit Name
	ImGui::InputText(sFInputName.c_str(), name, IM_ARRAYSIZE(name)); // Create Widget Name
	s_displayLayer->m_layerData.m_joints.at(i).m_bodyAName = name; // Set Object Name to Input Value
	ImGui::NewLine(); // Add Line Break


	strcpy(name, s_displayLayer->m_layerData.m_joints.at(i).m_bodyBName.c_str()); // Get Object Name

	// Define Global Widgit Name Holder Variable
	sFInputName;

	// Create Object Input Name Widgit Header
	ImGui::Text("Body B Name:");

	// Enter Object Name Widgit
	ImGui::PushItemWidth(200.f); // Set Widgit Width
	sFInputName = "###BodyBName" + std::to_string(s_displayLayer->m_layerData.m_joints.at(i).m_jointVectorValue); // Set Widgit Name
	ImGui::InputText(sFInputName.c_str(), name, IM_ARRAYSIZE(name)); // Create Widget Name
	s_displayLayer->m_layerData.m_joints.at(i).m_bodyBName = name; // Set Object Name to Input Value
	ImGui::NewLine();

	ImGui::Text("Bodies Can Collide:");
	sFInputName = "###BodiesCanCollide" + std::to_string(s_displayLayer->m_layerData.m_joints.at(i).m_jointVectorValue); // Set Widgit Name
	ImGui::Checkbox(sFInputName.c_str(), &s_displayLayer->m_layerData.m_joints.at(i).m_bBodiesCanCollide); // To choose if the bodies on the joint can collide
	ImGui::NewLine();

	if (type.compare("Hinge") == 0)
	{
		glm::vec3 axis = s_displayLayer->m_layerData.m_joints.at(i).m_axis;

		// Create Object Axis GUI
		ImGui::PushItemWidth(64.f); // Set Widget Width
		ImGui::Text("Axis:"); // Output GUI Section Title
		ImGui::Text(" X"); // Indicate X Axis Box
		ImGui::SameLine(); // Make Same Line as...
		sFInputName = "###XAxis" + std::to_string(i); // Define Float Box Name String
		ImGui::InputFloat(sFInputName.c_str(), &axis.x); // Create X Axis Float Input
		ImGui::SameLine(); // Make Same Line as...
		ImGui::Text(" Y"); // Indicate Y Axis Box
		ImGui::SameLine(); // Make Same Line as...
		sFInputName = "###YAxis" + std::to_string(i); // Define Float Box Name String
		ImGui::InputFloat(sFInputName.c_str(), &axis.y); // Create Y Axis Float Input
		ImGui::SameLine(); // Make Same Line as...
		ImGui::Text(" Z"); // Indicate Z Axis Box
		ImGui::SameLine(); // Make Same Line as...
		sFInputName = "###ZAxis" + std::to_string(i); // Define Float Box Name String
		ImGui::InputFloat(sFInputName.c_str(), &axis.z); // Create Z Axis Float Input
		ImGui::NewLine();

		s_displayLayer->m_layerData.m_joints.at(i).m_axis = axis;
	}
	// If it's a hinge or a slider joint
	if (type.compare("Hinge") == 0 || type.compare("Slider") == 0)
	{
		// Choose if it is limited
		ImGui::Text("Limited:");
		sFInputName = "###Limited" + std::to_string(s_displayLayer->m_layerData.m_joints.at(i).m_jointVectorValue); // Set Widgit Name
		ImGui::Checkbox(sFInputName.c_str(), &s_displayLayer->m_layerData.m_joints.at(i).m_bIsLimited);

		if (s_displayLayer->m_layerData.m_joints.at(i).m_bIsLimited)
		{
			ImGui::Indent(16.f);
			// Set limit values
			ImGui::Text("Minimum Limit:");
			sFInputName = "###MinLimit" + std::to_string(s_displayLayer->m_layerData.m_joints.at(i).m_jointVectorValue);
			ImGui::InputFloat(sFInputName.c_str(), &s_displayLayer->m_layerData.m_joints.at(i).m_fMinLimit);
			ImGui::NewLine();
			ImGui::Text("Maximum Limit:");
			sFInputName = "###MaxLimit" + std::to_string(s_displayLayer->m_layerData.m_joints.at(i).m_jointVectorValue);
			ImGui::InputFloat(sFInputName.c_str(), &s_displayLayer->m_layerData.m_joints.at(i).m_fMaxLimit);

			ImGui::Unindent(16.f);
			// Control limit value range
			if (type.compare("Hinge") == 0)
			{
				s_displayLayer->m_layerData.m_joints.at(i).m_fMinLimit = glm::clamp(s_displayLayer->m_layerData.m_joints.at(i).m_fMinLimit, -360.f, 0.f);
				s_displayLayer->m_layerData.m_joints.at(i).m_fMaxLimit = glm::clamp(s_displayLayer->m_layerData.m_joints.at(i).m_fMaxLimit, 0.f, 360.f);
			}
			if (type.compare("Slider") == 0)
			{
				if (s_displayLayer->m_layerData.m_joints.at(i).m_fMinLimit > 0)
				{
					s_displayLayer->m_layerData.m_joints.at(i).m_fMinLimit = 0;
				}
				if (s_displayLayer->m_layerData.m_joints.at(i).m_fMaxLimit < 0)
				{
					s_displayLayer->m_layerData.m_joints.at(i).m_fMaxLimit = 0;
				}
			}
		}
	}

	ImGui::NewLine(); // Add Line Break
}

void ChangeObjects::changeText(int i)
{
	std::string sFInputName = "Text:###Text" + std::to_string(i);

	// Create Collider GUI
	if (ImGui::CollapsingHeader(sFInputName.c_str()))
	{
		// Indent GUI
		ImGui::Indent(16.f);

		int textNum = s_displayLayer->m_layerData.m_2DObjects.at(i).m_textComponentVectorValue;
		std::shared_ptr<Engine::TextLabel> label = s_displayLayer->getTexts().at(textNum)->getLabel();

		int posNum = s_displayLayer->m_layerData.m_2DObjects.at(i).m_positionComponentVectorValue; // Get Object

		// Different transform edit for 2D thing
		sFInputName = "Transform:###TextTransform";
		if (ImGui::CollapsingHeader(sFInputName.c_str()))
		{
			ImGui::Indent(16.f);

			// Get the Object's Current Position
			glm::vec3 position = s_displayLayer->getPositions().at(posNum)->getPosition(); // Get Position

			// Only change x and y positions
			// Create Object Position GUI
			ImGui::PushItemWidth(64.f); // Set Widget Width
			ImGui::Text("Position:"); // Output GUI Section Title
			ImGui::Text(" X"); // Indicate X Pos Box
			ImGui::SameLine(); // Make Same Line as...
			std::string sFInputName = "###XPosition" + std::to_string(posNum); // Define Float Box Name String
			ImGui::InputFloat(sFInputName.c_str(), &position.x); // Create X Pos Float Input
			ImGui::SameLine(); // Make Same Line as...
			ImGui::Text(" Y"); // Indicate Y Pos Box
			ImGui::SameLine(); // Make Same Line as...
			sFInputName = "###YPosition" + std::to_string(posNum); // Define Float Box Name String
			ImGui::InputFloat(sFInputName.c_str(), &position.y); // Create Y Pos Float Input
			ImGui::NewLine(); // Insert Line Break

			// Set New Object Position
			Engine::ComponentMessage msg(Engine::ComponentMessageType::PositionSet, (void*)&position); // Create Message
			s_displayLayer->getPositions().at(posNum)->receiveMessage(msg); // Send Message

			label->setPosition(glm::vec2(position.x, position.y));


			glm::vec3 rotation = s_displayLayer->getPositions().at(posNum)->getRotationDeg(); // Get rotation

			// Only change one rotation
			// Create Object rotation GUI
			ImGui::PushItemWidth(64.f); // Set Widget Width
			ImGui::Text("Rotation:"); // Output GUI Section Title
			sFInputName = "###TextRotation" + std::to_string(posNum); // Define Float Box Name String
			ImGui::InputFloat(sFInputName.c_str(), &rotation.y); // Create X Pos Float Input
			ImGui::NewLine();

			Engine::ComponentMessage msg2(Engine::ComponentMessageType::RotationSet, (void*)&rotation); // Create Message
			s_displayLayer->getPositions().at(posNum)->receiveMessage(msg2); // Send Message

			label->setOrientation(rotation.y);


			glm::vec3 scale = s_displayLayer->getPositions().at(posNum)->getScale(); // Get scale

			// Only have one scale number applied to all 3 axis
			// Create Object scale GUI
			ImGui::PushItemWidth(64.f); // Set Widget Width
			ImGui::Text("Scale:"); // Output GUI Section Title
			sFInputName = "###TextScale" + std::to_string(posNum); // Define Float Box Name String
			ImGui::InputFloat(sFInputName.c_str(), &scale.x); // Create X Pos Float Input
			ImGui::NewLine();

			scale.y = scale.x;
			scale.z = scale.x;

			Engine::ComponentMessage msg3(Engine::ComponentMessageType::ScaleSet, (void*)&scale); // Create Message
			s_displayLayer->getPositions().at(posNum)->receiveMessage(msg3); // Send Message

			label->setScale(scale.x);

			ImGui::Unindent(16.f);
		}
		// Change text characteristics
		sFInputName = "Text:###TextChange";
		if (ImGui::CollapsingHeader(sFInputName.c_str()))
		{
			ImGui::Indent(16.f);

			ImGui::Text("Text:");
			sFInputName = "###EditText" + std::to_string(textNum);
			ImGui::InputText(sFInputName.c_str(), s_displayLayer->m_layerData.m_2DObjects.at(i).m_tempTexData.m_text, IM_ARRAYSIZE(s_displayLayer->m_layerData.m_2DObjects.at(i).m_tempTexData.m_text));
			ImGui::NewLine();

			ImGui::Text("Font:");
			std::string font = s_displayLayer->m_layerData.m_2DObjects.at(i).m_tempTexData.m_font;
			sFInputName = "###Font" + std::to_string(textNum);
			std::string currentFont = font;
			if (ImGui::BeginCombo(sFInputName.c_str(), currentFont.c_str())) // Begin Creating List
			{
				for (auto it : s_displayLayer->m_layerData.m_fontsAndSizes)
				{
					sFInputName = parseFilePath(it.first);
					bool selected = sFInputName == currentFont;
					if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
					{
						s_displayLayer->m_layerData.m_2DObjects.at(i).m_tempTexData.m_font = sFInputName;
					}
				}

				// Stop Creating List
				ImGui::EndCombo();
			}
			ImGui::NewLine();

			ImGui::Text("Size:");
			int size = s_displayLayer->m_layerData.m_2DObjects.at(i).m_tempTexData.m_size;
			sFInputName = "###Size" + std::to_string(textNum);
			std::string currentSize = std::to_string(size);

			std::vector<int> fontSizes;

			for (auto& it : s_displayLayer->m_layerData.m_fontsAndSizes)
			{
				if (parseFilePath(it.first).compare(s_displayLayer->m_layerData.m_2DObjects.at(i).m_tempTexData.m_font) == 0)
				{
					fontSizes = it.second;
				}
			}
			bool equal = false;
			for (auto it : fontSizes)
			{
				equal = it == size;
				if (equal)
					break;
			}
			if (!equal)
			{
				currentSize = std::to_string(fontSizes.front());
				s_displayLayer->m_layerData.m_2DObjects.at(i).m_tempTexData.m_size = fontSizes.front();
			}

			if (ImGui::BeginCombo(sFInputName.c_str(), currentSize.c_str())) // Begin Creating List
			{
				for (auto it : fontSizes)
				{
					sFInputName = std::to_string(it);
					bool selected = sFInputName == currentSize;
					if (ImGui::Selectable(sFInputName.c_str(), selected)) // Create Selectable Item
					{
						s_displayLayer->m_layerData.m_2DObjects.at(i).m_tempTexData.m_size = it;
					}
				}

				// Stop Creating List
				ImGui::EndCombo();
			}
			ImGui::NewLine();

			sFInputName = "Change Text###ChangeText" + std::to_string(textNum);
			if (ImGui::Button(sFInputName.c_str()))
			{
				s_displayLayer->m_layerData.m_2DObjects.at(i).m_textFont = s_displayLayer->m_layerData.m_2DObjects.at(i).m_tempTexData.m_font;
				s_displayLayer->m_layerData.m_2DObjects.at(i).m_textCharSize = s_displayLayer->m_layerData.m_2DObjects.at(i).m_tempTexData.m_size;

				label->setFont(s_displayLayer->m_layerData.m_2DObjects.at(i).m_textFont);
				label->setCharSize(s_displayLayer->m_layerData.m_2DObjects.at(i).m_textCharSize);
				label->setText(s_displayLayer->m_layerData.m_2DObjects.at(i).m_tempTexData.m_text);

				label->makeMaterial();

				auto mat = label->getMaterial();
				// Set the colour uniform
				mat->setDataElement("u_fontColour", (void*)&s_displayLayer->getTexts().back()->getLabel()->getColour()[0]);
				s_displayLayer->getMaterials().at(s_displayLayer->m_layerData.m_2DObjects.at(i).m_materialComponentVectorValue)->setMaterial(mat);

				s_displayLayer->getPositions().at(posNum)->onUpdate(0.f);

				s_displayLayer->getTextures().at(s_displayLayer->m_layerData.m_2DObjects.at(i).m_textureComponentVectorValue)->setDiffuse(s_displayLayer->getResources()->getFontTexture());
			}
			ImGui::NewLine();


			glm::vec3 colour = label->getColour();
			float editColour[3] = { colour.x, colour.y, colour.z };
			// Change the colour of the text
			ImGui::Text("Text Colour:");
			sFInputName = "###NewTextColour" + std::to_string(textNum);
			ImGui::ColorEdit3(sFInputName.c_str(), editColour);

			colour = glm::vec3(editColour[0], editColour[1], editColour[2]);

			label->setColour(colour);

			ImGui::Unindent(16.f);
		}

		ImGui::Unindent(16.f);
	}
}

std::string ChangeObjects::parseFilePath(const std::string& str)
{
	int i = str.rfind('/', str.length()); // Find the position in the string of the last occurence of '/'
	int j = str.rfind('\\', str.length()); // Find the position in the string of the last occurence of '\'
	int k = str.rfind('.', str.length()); // Find the position in the string of the last occurence of '.'

	if (i != std::string::npos && i > j) // If '/' and '.' are found
	{
		// Return a string with the name of the folder
		return str.substr(i + 1, (str.length() - i) - (str.length() - (k - 1)));
	}
	else if (j != std::string::npos) // If '/' and '.' are found
	{
		// Return a string with the name of the folder
		return str.substr(j + 1, (str.length() - j) - (str.length() - (k - 1)));
	}
	else // If '/' and '.' were not found
	{
		// Log an error
		LOG_INFO("Problem finding name of file: {0}, using full filepath", str);
		return str; // Return the full filepath
	}
}