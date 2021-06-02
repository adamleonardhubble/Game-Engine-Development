#pragma once

#include <Engine.h>

#include "json.hpp"
#include "layers/displayLayer.h"

#include "editorCamera.h"

#include "systems/log.h"

class EditorJSONLoader
{
public:
	//! Function which loads a layer
	/*!
	\param filepath The location of the JSON file being loaded
	\param layer The layer that the file is being loaded to
	*/
	static void load(const std::string& filepath, DisplayLayer& layer)
	{
		// Get the data from the JSON file
		std::fstream handle(filepath, std::ios::in);
		if (!handle.is_open()) {
			LOG_CRITICAL("Could not open file: {0}", filepath);
		}
		// Make the JSON loader to get all of the data
		nlohmann::json layerJSON;
		handle >> layerJSON;

		if (layerJSON.count("Asyncload") > 0)
		{
			// If there are shaders in the layer
			if (layerJSON["Asyncload"].count("shaders") > 0)
			{
				// Add them all to the resource manager
				for (auto& filepath : layerJSON["Asyncload"]["shaders"])
				{
					if (filepath.count("filepath") > 0) layer.getResources()->addShader(filepath["filepath"].get<std::string>());
				}
			}
			// If there are any textures in the layer
			if (layerJSON["Asyncload"].count("textures") > 0)
			{
				// Add them all to the resource manager
				for (auto& filepath : layerJSON["Asyncload"]["textures"])
				{
					if (filepath.count("filepath") > 0) layer.getResources()->addTexture(filepath["filepath"].get<std::string>());
				}
			}
			// If there are models in the layer
			if (layerJSON["Asyncload"].count("models") > 0)
			{
				// Add them all to the resource manager
				for (auto& filepath : layerJSON["Asyncload"]["models"])
				{
					if (filepath.count("filepath") > 0) layer.getResources()->addModel(filepath["filepath"].get<std::string>());
				}
			}
			// If there are any fonts used in the layer
			if (layerJSON["Asyncload"].count("fonts") > 0)
			{
				// Make a map with all of the fonts and sizes
				std::unordered_multimap<std::string, unsigned int> fonts;
				for (auto& filepath : layerJSON["Asyncload"]["fonts"])
				{
					std::string font = filepath["filepath"].get<std::string>();
					int size = filepath["charSize"].get<int>();

					fonts.insert(std::make_pair(font, size));

					layer.m_layerData.m_fontsAndSizes[font].push_back(size);
				}
				layer.m_layerData.m_fonts = fonts;
				// Populate the character texture with all the characters in all the font and size pairs
				if (!fonts.empty()) 
					layer.getResources()->populateCharacters(fonts);
			}
		}

		if (layerJSON.count("3DLayers") > 0)
		{
			for (auto& layerName : layerJSON["3DLayers"])
			{
				layer.m_layerData.m_3DLayers.push_back(layerName["name"].get<std::string>());
			}
		}
		if (layerJSON.count("2DLayers") > 0)
		{
			for (auto& layerName : layerJSON["2DLayers"])
			{
				layer.m_layerData.m_2DLayers.push_back(layerName["name"].get<std::string>());
			}
		}
		if (layerJSON.count("tags") > 0)
		{
			for (auto& tag : layerJSON["tags"])
			{
				layer.m_layerData.m_tags.push_back(tag["tag"].get<std::string>());
			}
		}

		if (layerJSON.count("camera") > 0)
		{
			layer.getCamera().reset(new EditorCamera());
			float xPos = layerJSON["camera"]["position"]["x"].get<float>();
			float yPos = layerJSON["camera"]["position"]["y"].get<float>();
			float zPos = layerJSON["camera"]["position"]["z"].get<float>();
			float yaw = layerJSON["camera"]["rotation"]["yaw"].get<float>();
			float pitch = layerJSON["camera"]["rotation"]["pitch"].get<float>();
			layer.getCamera()->init(45.f, 1.3333333f, 0.1f, 100.f, glm::vec3(xPos, yPos, zPos), glm::vec2(yaw, pitch));
		}
		if (layerJSON.count("displaySize") > 0)
		{
			layer.m_displayWindowMultiplier = layerJSON["displaySize"].get<float>();
		}
		else
		{
			layer.m_displayWindowMultiplier = 1;
		}
		// If the layer has a renderer
		if (layerJSON.count("renderer") > 0)
		{
			// Make a renderer for the layer depending on the type in the JSON file
			std::string type = layerJSON["renderer"]["type"].get<std::string>();
			if (type.compare("Basic3D") == 0) layer.getRenderer().reset(Engine::Renderer::createBasic3D());
		}
		// If there is memory info (for objects)
		if (layerJSON.count("MemoryInfo") > 0)
		{
			// Set the sizes of all of the vectors containing the gameobjects and all of the components on the objects in the layer
			layer.getGameObjects().resize(layerJSON["MemoryInfo"]["gameObjects"].get<int>());
			layer.get2DGameObjects().resize(layerJSON["MemoryInfo"]["2DGameobjects"].get<int>());
			layer.m_layerData.m_objects.resize(layerJSON["MemoryInfo"]["gameObjects"].get<int>());
			layer.m_layerData.m_2DObjects.resize(layerJSON["MemoryInfo"]["2DGameobjects"].get<int>());
			layer.m_layerData.m_joints.resize(layerJSON["MemoryInfo"]["joints"].get<int>());
			layer.getMaterials().resize(layerJSON["MemoryInfo"]["materials"].get<int>());
			layer.getMeshes().resize(layerJSON["MemoryInfo"]["mesh"].get<int>());
			layer.getTexts().resize(layerJSON["MemoryInfo"]["text"].get<int>());
			layer.getPositions().resize(layerJSON["MemoryInfo"]["position"].get<int>());
			layer.getTextures().resize(layerJSON["MemoryInfo"]["texture"].get<int>());
			layer.getLights().resize(layerJSON["MemoryInfo"]["lights"].get<int>());
			layer.getCameras().resize(layerJSON["MemoryInfo"]["cameras"].get<int>());
		}

		// If there are gameobjects in the layer
		if (layerJSON.count("GameObjects") > 0)
		{
			// Indexes to set the correct values in the vectors
			int goIndex = 0;
			int textIndex = 0;
			int materialsIndex = 0;
			int meshIndex = 0;
			int positionsIndex = 0;
			int texturesIndex = 0;
			int lightIndex = 0;
			int cameraIndex = 0;

			// For all gameobjects in the layer
			for (auto& object : layerJSON["GameObjects"])
			{
				goIndex = object["index"].get<int>();
				// Create gameobject in the layer
				std::string name = "Name";
				if (object.count("name") > 0) name = object["name"].get<std::string>();
				
				std::string layerName = object["layerName"].get<std::string>();

				std::shared_ptr<Engine::GameObject> gameObject;

				std::vector<Object>* pObjects;

				std::vector<std::string>::iterator it;
				for (it = layer.m_layerData.m_3DLayers.begin(); it != layer.m_layerData.m_3DLayers.end(); ++it)
				{
					if (*it == layerName)
					{
						//goIndex -= layer.m_layerData.m_2DObjects.size();
						layer.m_layerData.m_objects.at(goIndex) = Object(goIndex);

						layer.m_layerData.m_objects.at(goIndex).m_in3DLayer = true;

						layer.getGameObjects().at(goIndex) = (std::make_shared<Engine::GameObject>(Engine::GameObject(name, "", goIndex)));
						gameObject = layer.getGameObjects().at(goIndex);

						layer.m_layerData.m_objects.at(goIndex).m_tag = object["tag"].get<std::string>();
						layer.m_layerData.m_objects.at(goIndex).m_layer = layerName;

						pObjects = &layer.m_layerData.m_objects;
						break;
					}
				}
				for (it = layer.m_layerData.m_2DLayers.begin(); it != layer.m_layerData.m_2DLayers.end(); ++it)
				{
					if (*it == layerName)
					{
						goIndex -= layer.m_layerData.m_objects.size();
						layer.m_layerData.m_2DObjects.at(goIndex) = Object(goIndex);

						layer.m_layerData.m_2DObjects.at(goIndex).m_in3DLayer = false;

						layer.get2DGameObjects().at(goIndex) = (std::make_shared<Engine::GameObject>(Engine::GameObject(name, "", goIndex)));
						gameObject = layer.get2DGameObjects().at(goIndex);

						layer.m_layerData.m_2DObjects.at(goIndex).m_tag = object["tag"].get<std::string>();
						layer.m_layerData.m_2DObjects.at(goIndex).m_layer = layerName;

						pObjects = &layer.m_layerData.m_2DObjects;
						break;
					}
				}
				
				// Check which components need adding
				// If there is a text component
				if (object.count("text") > 0) {
					// Get all of the information needed to make a text label from the JSON file
					std::string text = object["text"]["text"].get<std::string>();
					std::string font = object["text"]["font"].get<std::string>();
					int charSize = object["text"]["charSize"].get<int>();
					float posX = object["text"]["position"]["x"].get<float>();
					float posY = object["text"]["position"]["y"].get<float>();
					float rot = object["text"]["rotation"].get<float>();
					float scale = object["text"]["scale"].get<float>();
					float r = object["text"]["colour"]["r"].get<float>();
					float g = object["text"]["colour"]["g"].get<float>();
					float b = object["text"]["colour"]["b"].get<float>();

					pObjects->at(goIndex).m_tempTexData.m_font = font;
					pObjects->at(goIndex).m_tempTexData.m_size = charSize;
					strcpy(pObjects->at(goIndex).m_tempTexData.m_text, text.c_str());

					// Make the text label
					std::shared_ptr<Engine::TextLabel> label;
					label.reset(Engine::TextLabel::create(font, charSize, text, glm::vec2(posX, posY), rot, scale, glm::vec3(r, g, b)));

					pObjects->at(goIndex).m_hasText = true;
					pObjects->at(goIndex).m_textComponentVectorValue = textIndex;
					pObjects->at(goIndex).m_textFont = font;
					pObjects->at(goIndex).m_textCharSize = charSize;

					// Make a compoent with the text label and add it to the gameobject
					layer.getTexts().at(textIndex) = std::make_shared<Engine::TextComponent>(Engine::TextComponent(label));
					gameObject->addComponent(layer.getTexts().at(textIndex));
					textIndex++;
				}
				if (object.count("mesh") > 0) {
					std::shared_ptr<Engine::Model> model = layer.getResources()->addModel(object["mesh"]["model"]);

					layer.getMeshes().at(meshIndex) = std::make_shared<Engine::MeshComponent>(Engine::MeshComponent(model));
					gameObject->addComponent(layer.getMeshes().at(meshIndex));

					pObjects->at(goIndex).m_meshComponentVectorValue = meshIndex;
					pObjects->at(goIndex).m_modelFilepath = object["mesh"]["model"].get<std::string>();
					pObjects->at(goIndex).m_hasMesh = true;
					meshIndex++;
				}
				// If there is a material on the object
				if (object.count("material") > 0) {
					if (object["material"]["shader"].get<std::string>().compare("text") == 0) // If it is text
					{
						// Get the material made for the text label
						auto& mat = layer.getTexts().at(textIndex - 1)->getLabel()->getMaterial();
						// Set the colour uniform
						mat->setDataElement("u_fontColour", (void*)&layer.getTexts().at(textIndex - 1)->getLabel()->getColour()[0]);
						// Make a component with the material and add it to the object
						layer.getMaterials().at(materialsIndex) = std::make_shared<Engine::MaterialComponent>(Engine::MaterialComponent(mat));
						gameObject->addComponent(layer.getMaterials().at(materialsIndex));

						pObjects->at(goIndex).m_materialComponentVectorValue = materialsIndex;
						pObjects->at(goIndex).m_hasMat = true;

						materialsIndex++;
					}
					// If it is a model
					else if (object.count("mesh") > 0)
					{
						std::shared_ptr<Engine::Model> model = layer.getMeshes().at(meshIndex - 1)->getModel();
						std::shared_ptr<Engine::Shader> shader = layer.getResources()->addShader(object["material"]["shader"]);
						// Create the material
						std::shared_ptr<Engine::VertexArray> VAO = layer.getResources()->addVAO(object["mesh"]["model"]);
						VAO->setVertexBuffer(layer.getResources()->addVBO(object["mesh"]["model"], model->vertices, sizeof(float) * model->verticesSize, shader->getBufferLayout()));
						VAO->setIndexBuffer(layer.getResources()->addIndexBuffer(object["mesh"]["model"], model->indices, model->indicesSize));
						std::shared_ptr<Engine::Material> mat = layer.getResources()->addMaterial(name + std::to_string(goIndex), shader, VAO);
						// Create a component with the material
						layer.getMaterials().at(materialsIndex) = std::make_shared<Engine::MaterialComponent>(Engine::MaterialComponent(mat));
						if (object["material"].count("material"))
						{
							// Get the material values for light calculations
							auto& matValues = object["material"]["material"];
							float dr = matValues["diffuse"]["r"].get<float>();
							float dg = matValues["diffuse"]["g"].get<float>();
							float db = matValues["diffuse"]["b"].get<float>();
							float sr = matValues["specular"]["r"].get<float>();
							float sg = matValues["specular"]["g"].get<float>();
							float sb = matValues["specular"]["b"].get<float>();
							float shininess = matValues["shininess"].get<float>();
							// Set the values on the material component
							layer.getMaterials().at(materialsIndex)->setDiffuse(glm::vec3(dr, dg, db));
							layer.getMaterials().at(materialsIndex)->setSpecular(glm::vec3(sr, sg, sb));
							layer.getMaterials().at(materialsIndex)->setShininess(shininess);
						}
						// Add the component to the object
						gameObject->addComponent(layer.getMaterials().at(materialsIndex));

						pObjects->at(goIndex).m_materialComponentVectorValue = materialsIndex;
						pObjects->at(goIndex).m_shaderFilepath = object["material"]["shader"].get<std::string>();
						pObjects->at(goIndex).m_hasMat = true;

						materialsIndex++;
					}
				}

				// If there is a position component
				if (object.count("position") > 0) {
					// If it is a text object
					if (object["position"]["type"].get<std::string>().compare("text") == 0)
					{
						// Make the position component using the transform from the text label
						layer.getPositions().at(positionsIndex) = std::make_shared<Engine::PositionComponent>(Engine::PositionComponent(
							glm::vec3(layer.getTexts().at(textIndex - 1)->getLabel()->getPosition(), 0),
							glm::vec3(0, 0, layer.getTexts().at(textIndex - 1)->getLabel()->getOrientation()),
							glm::vec3(layer.getTexts().at(textIndex - 1)->getLabel()->getScale())));
					}
					else if (object["position"]["type"].get<std::string>().compare("values") == 0) // If it is not a text object
					{
						// Get the transform vales from the JSON file and make the component
						glm::vec3 translation(object["position"]["trans"]["x"].get<float>(), object["position"]["trans"]["y"].get<float>(), object["position"]["trans"]["z"].get<float>());
						glm::vec3 rotation(object["position"]["rot"]["x"].get<float>(), object["position"]["rot"]["y"].get<float>(), object["position"]["rot"]["z"].get<float>());
						glm::vec3 scale(object["position"]["scale"]["x"].get<float>(), object["position"]["scale"]["y"].get<float>(), object["position"]["scale"]["z"].get<float>());
						layer.getPositions().at(positionsIndex) = std::make_shared<Engine::PositionComponent>(Engine::PositionComponent(translation, rotation, scale));
					}
					// Add the component to the object
					gameObject->addComponent(layer.getPositions().at(positionsIndex));

					pObjects->at(goIndex).m_positionComponentVectorValue = positionsIndex;

					positionsIndex++;
				}

				// If there is a texture component
				if (object.count("texture") > 0) {
					// Get the needed texture (text or not) from the resource manager
					std::shared_ptr<Engine::Texture> tex;
					if (object["texture"]["diffuse"].get<std::string>().compare("text") == 0) 
					{
						tex = layer.getResources()->getFontTexture();
						// Make a texture component with both textures and add it to the object
						layer.getTextures().at(texturesIndex) = std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, tex));
					}
					else
					{
						tex = layer.getResources()->addTexture(object["texture"]["diffuse"].get<std::string>());

						std::string normalFileName = parseFilePath(object["texture"]["normal"].get<std::string>());
						std::string depthFileName = parseFilePath(object["texture"]["depth"].get<std::string>());

						if (normalFileName.compare("None") == 0 && depthFileName.compare("None") == 0)
						{
							std::shared_ptr<Engine::Texture> specTex = layer.getResources()->addTexture(object["texture"]["specular"].get<std::string>());
							// Make a texture component with both textures and add it to the object
							layer.getTextures().at(texturesIndex) = std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, specTex));
							pObjects->at(goIndex).m_specularTextureFilepath = object["texture"]["specular"].get<std::string>();
						}
						else
						{
							if (normalFileName.compare("None") != 0 && depthFileName.compare("None") == 0)
							{
								std::shared_ptr<Engine::Texture> specTex = layer.getResources()->addTexture(object["texture"]["specular"].get<std::string>());
								std::shared_ptr<Engine::Texture> normMap = layer.getResources()->addTexture(object["texture"]["normal"].get<std::string>());
								// Make a texture component with both textures and add it to the object
								layer.getTextures().at(texturesIndex) = std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, specTex, normMap));
								pObjects->at(goIndex).m_specularTextureFilepath = object["texture"]["specular"].get<std::string>();
								pObjects->at(goIndex).m_normalMapFilepath = object["texture"]["normal"].get<std::string>();
								pObjects->at(goIndex).m_hasNormalMap = true;
							}
							else if (normalFileName.compare("None") == 0 && depthFileName.compare("None") != 0)
							{
								std::shared_ptr<Engine::Texture> specTex = layer.getResources()->addTexture(object["texture"]["specular"].get<std::string>());
								std::shared_ptr<Engine::Texture> depthMap = layer.getResources()->addTexture(object["texture"]["depth"].get<std::string>());
								float heightScale = object["texture"]["heightScale"].get<float>();
								// Make a texture component with both textures and add it to the object
								layer.getTextures().at(texturesIndex) = std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, specTex, depthMap, heightScale));
								pObjects->at(goIndex).m_specularTextureFilepath = object["texture"]["specular"].get<std::string>();
								pObjects->at(goIndex).m_depthMapFilepath = object["texture"]["depth"].get<std::string>();
								pObjects->at(goIndex).m_hasDepthMap = true;
							}
							else if (normalFileName.compare("None") != 0 && depthFileName.compare("None") != 0)
							{
								std::shared_ptr<Engine::Texture> specTex = layer.getResources()->addTexture(object["texture"]["specular"].get<std::string>());
								std::shared_ptr<Engine::Texture> normMap = layer.getResources()->addTexture(object["texture"]["normal"].get<std::string>());
								std::shared_ptr<Engine::Texture> depthMap = layer.getResources()->addTexture(object["texture"]["depth"].get<std::string>());
								float heightScale = object["texture"]["heightScale"].get<float>();
								// Make a texture component with both textures and add it to the object
								layer.getTextures().at(texturesIndex) = std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, specTex, normMap, depthMap, heightScale));
								pObjects->at(goIndex).m_specularTextureFilepath = object["texture"]["specular"].get<std::string>();
								pObjects->at(goIndex).m_normalMapFilepath = object["texture"]["normal"].get<std::string>();
								pObjects->at(goIndex).m_depthMapFilepath = object["texture"]["depth"].get<std::string>();
								pObjects->at(goIndex).m_hasNormalMap = true;
								pObjects->at(goIndex).m_hasDepthMap = true;
							}
						}
					}
					gameObject->addComponent(layer.getTextures().at(texturesIndex));

					pObjects->at(goIndex).m_textureComponentVectorValue = texturesIndex;
					pObjects->at(goIndex).m_diffuseTextureFilepath = object["texture"]["diffuse"].get<std::string>();
					pObjects->at(goIndex).m_hasTexture = true;

					texturesIndex++;
				}

				// If There is a Light Component
				if (object.count("light") > 0) {
					std::shared_ptr<Engine::UniformBuffer> UBO = layer.getResources()->getUBO("Light");

					std::vector<void*> uboData;

					float ar = object["light"]["ambient"]["r"].get<float>();
					float ag = object["light"]["ambient"]["g"].get<float>();
					float ab = object["light"]["ambient"]["b"].get<float>();

					float pdr = object["light"]["pointdiffuse"]["r"].get<float>();
					float pdg = object["light"]["pointdiffuse"]["g"].get<float>();
					float pdb = object["light"]["pointdiffuse"]["b"].get<float>();
					float psr = object["light"]["pointspecular"]["r"].get<float>();
					float psg = object["light"]["pointspecular"]["g"].get<float>();
					float psb = object["light"]["pointspecular"]["b"].get<float>();

					float ddr = object["light"]["directionaldiffuse"]["r"].get<float>();
					float ddg = object["light"]["directionaldiffuse"]["g"].get<float>();
					float ddb = object["light"]["directionaldiffuse"]["b"].get<float>();
					float dsr = object["light"]["directionalspecular"]["r"].get<float>();
					float dsg = object["light"]["directionalspecular"]["g"].get<float>();
					float dsb = object["light"]["directionalspecular"]["b"].get<float>();

					float att = object["light"]["attenuation"].get<float>();

					layer.getData().push_back(new glm::vec3(ar, ag, ab));
					glm::vec3* ambient = (glm::vec3*)layer.getData().back();
					uboData.push_back((void*)&(*(glm::vec3*)layer.getData().back())[0]);

					layer.getData().push_back(new glm::vec3(pdr, pdg, pdb));
					glm::vec3* pointDiffuse = (glm::vec3*)layer.getData().back();
					uboData.push_back((void*)&(*(glm::vec3*)layer.getData().back())[0]);

					layer.getData().push_back(new glm::vec3(psr, psg, psb));
					glm::vec3* pointSpecular = (glm::vec3*)layer.getData().back();
					uboData.push_back((void*)&(*(glm::vec3*)layer.getData().back())[0]);


					layer.getData().push_back(new glm::vec3(ddr, ddg, ddb));
					glm::vec3* dirDiffuse = (glm::vec3*)layer.getData().back();
					uboData.push_back((void*)&(*(glm::vec3*)layer.getData().back())[0]);

					layer.getData().push_back(new glm::vec3(dsr, dsg, dsb));
					glm::vec3* dirSpecular = (glm::vec3*)layer.getData().back();
					uboData.push_back((void*)&(*(glm::vec3*)layer.getData().back())[0]);


					layer.getData().push_back(new glm::vec3(1.f, att, (att / 100.f) * 3.5f));
					glm::vec3* attenuation = (glm::vec3*)layer.getData().back();
					uboData.push_back((void*)&(*(glm::vec3*)layer.getData().back())[0]);

					// Set the scene data in the layer to this data
					(*layer.getSceneData())[UBO] = uboData;

					UBO = layer.getResources()->getUBO("LightPos");

					std::vector<void*> uboData2;
					uboData2.push_back((void*)&layer.getPositions().at(positionsIndex - 1)->getPosition());
					uboData2.push_back((void*)&layer.getPositions().at(positionsIndex - 1)->getRotationDeg());
					uboData2.push_back((void*)&layer.getCamPos());

					// Set the scene data in the layer to this data
					(*layer.getSceneData())[UBO] = uboData2;

					layer.getLights().at(lightIndex) = std::make_shared<Engine::LightComponent>(
						Engine::LightComponent(ambient, pointDiffuse, pointSpecular, dirDiffuse, dirSpecular, attenuation));

					gameObject->addComponent(layer.getLights().at(lightIndex));

					pObjects->at(goIndex).m_lightComponentVectorValue = lightIndex;
					pObjects->at(goIndex).m_hasLight = true;
					lightIndex++;
				}

				// If There is a Camera Component
				if (object.count("camera") > 0) {
					std::string type = object["camera"]["type"].get<std::string>();

					if (pObjects->at(goIndex).m_in3DLayer)
					{
						float fov = object["camera"]["fov"].get<float>();
						float aspect = object["camera"]["aspect"].get<float>();
						float nearClip = object["camera"]["nearClip"].get<float>();
						float farClip = object["camera"]["farClip"].get<float>();
						bool mouse = object["camera"]["updateMousePress"].get<bool>();

						float rotX = layer.getPositions().at(positionsIndex - 1)->getRotation().x;
						float rotY = layer.getPositions().at(positionsIndex - 1)->getRotation().y;

						if (type.compare("free3D") == 0)
						{
							layer.getCameras().at(cameraIndex) = std::make_shared<Engine::FreeMoving3DCameraComponent>(Engine::FreeMoving3DCameraComponent(
								fov, aspect, nearClip, farClip, layer.getPositions().at(positionsIndex - 1)->getPosition(),
								glm::vec2(glm::degrees(rotX), glm::degrees(rotY)), mouse));
						}
						else if (type.compare("1stPerson") == 0)
						{
							layer.getCameras().at(cameraIndex) = std::make_shared<Engine::FPSCameraComponent>(Engine::FPSCameraComponent(
								fov, aspect, nearClip, farClip, layer.getPositions().at(positionsIndex - 1)->getPosition(),
								glm::vec2(glm::degrees(rotX), glm::degrees(rotY)), mouse));
						}
						else if (type.compare("fixed3D") == 0)
						{
							layer.getCameras().at(cameraIndex) = std::make_shared<Engine::Fixed3DCameraComponent>(Engine::Fixed3DCameraComponent(
								fov, aspect, nearClip, farClip, layer.getPositions().at(positionsIndex - 1)->getPosition(),
								glm::vec2(glm::degrees(rotX), glm::degrees(rotY))));
						}

						std::vector<std::string> layersSeen;
						for (auto& it : object["camera"]["layers"])
						{
							layersSeen.push_back(it.get<std::string>());
						}

						layer.getCameras().at(cameraIndex)->setLayersSeen(layersSeen);
					}
					else
					{
						float left = object["camera"]["left"].get<float>();
						float top = object["camera"]["top"].get<float>();
						float width = object["camera"]["width"].get<float>();
						float height = object["camera"]["height"].get<float>();

						if (type.compare("fixed2D") == 0)
						{
							layer.getCameras().at(cameraIndex) = std::make_shared<Engine::FixedOrthoCameraComponent>(
								Engine::FixedOrthoCameraComponent(left, top, width, height));

							std::vector<std::string> layersSeen;
							for (auto& it : layer.m_layerData.m_2DLayers)
							{
								layersSeen.push_back(it);
							}

							layer.getCameras().at(cameraIndex)->setLayersSeen(layersSeen);
						}
					}

					gameObject->addComponent(layer.getCameras().at(cameraIndex));

					pObjects->at(goIndex).m_cameraComponentVectorValue = cameraIndex;
					pObjects->at(goIndex).m_hasCamera = true;
					pObjects->at(goIndex).m_cameraType = type;

					cameraIndex++;
				}

				// If There is a Collider Component
				if (object.count("collider") > 0) {
					std::string shape = object["collider"]["shape"].get<std::string>();
					pObjects->at(goIndex).m_collider.m_shape = shape;
					pObjects->at(goIndex).m_hasCollider = true;
					pObjects->at(goIndex).m_collider.m_useScaleForSize = object["collider"]["scaleForSize"].get<bool>();

					Engine::CollisionCategory collisionCat = object["collider"]["collisionCategory"].get<Engine::CollisionCategory>();
					if (collisionCat == 1)
					{
						pObjects->at(goIndex).m_collider.m_category = 1;
					}
					if (collisionCat == 2)
					{
						pObjects->at(goIndex).m_collider.m_category = 2;
					}
					if (collisionCat == 4)
					{
						pObjects->at(goIndex).m_collider.m_category = 3;
					}
					if (collisionCat == 8)
					{
						pObjects->at(goIndex).m_collider.m_category = 4;
					}

					int canCollideWith[4] = { 0, 0, 0, 0 };
					int i = 0;
					for (auto& category : object["collider"]["canCollideWith"])
					{
						canCollideWith[i] = category.get<int>();
						i++;
					}
					for (int j = 0; j < 4; j++)
					{
						if (canCollideWith[j] == 1)
						{
							pObjects->at(goIndex).m_collider.m_collideCat[0] = true;
						}
						if (canCollideWith[j] == 2)
						{
							pObjects->at(goIndex).m_collider.m_collideCat[1] = true;
						}
						if (canCollideWith[j] == 4)
						{
							pObjects->at(goIndex).m_collider.m_collideCat[2] = true;
						}
						if (canCollideWith[j] == 8)
						{
							pObjects->at(goIndex).m_collider.m_collideCat[3] = true;
						}
					}

					if (shape.compare("box") == 0)
					{
						pObjects->at(goIndex).m_collider.m_halfExtents.x = object["collider"]["halfExtents"]["x"].get<float>();
						pObjects->at(goIndex).m_collider.m_halfExtents.y = object["collider"]["halfExtents"]["y"].get<float>();
						pObjects->at(goIndex).m_collider.m_halfExtents.z = object["collider"]["halfExtents"]["z"].get<float>();
					}
					else if (shape.compare("sphere") == 0)
					{
						pObjects->at(goIndex).m_collider.m_radius = object["collider"]["radius"].get<float>();
					}
					else if (shape.compare("capsule") == 0)
					{
						pObjects->at(goIndex).m_collider.m_radius = object["collider"]["radius"].get<float>();
						pObjects->at(goIndex).m_collider.m_height = object["collider"]["height"].get<float>();
					}
					else if (shape.compare("convex mesh") == 0)
					{
						pObjects->at(goIndex).m_collider.m_modelFilepath = object["collider"]["model"].get<std::string>();
					}
					pObjects->at(goIndex).m_collider.m_mass = object["collider"]["mass"].get<float>();

					layer.m_layerData.m_numColliders++;
				}

				// If There is a RidgidBody Component
				if (object.count("rigidbody") > 0) {
					std::string type = object["rigidbody"]["type"].get<std::string>();
					pObjects->at(goIndex).m_rigidBody.m_type = type;
					pObjects->at(goIndex).m_hasRigidbody = true;
					pObjects->at(goIndex).m_rigidBody.m_gravity = object["rigidbody"]["gravity"].get<bool>();
					pObjects->at(goIndex).m_rigidBody.m_bounce = object["rigidbody"]["material"]["bounciness"].get<float>();
					pObjects->at(goIndex).m_rigidBody.m_friction = object["rigidbody"]["material"]["friction"].get<float>();
					pObjects->at(goIndex).m_rigidBody.m_rollResist = object["rigidbody"]["material"]["rollingResistance"].get<float>();

					layer.m_layerData.m_numRigidbodies++;
				}

				// If There is a Lua Script Component
				///*
				if (object.count("luascript") > 0) {
					pObjects->at(goIndex).bHasLuaScript = true;
					pObjects->at(goIndex).m_LuaScript = LuaScriptData();
					pObjects->at(goIndex).m_LuaScript.m_sScriptFilePath = object["luascript"]["Filepath"].get<std::string>();
					
					layer.m_layerData.m_INumOfLuaScripts++;
				}
				//*/

				if (layerJSON.count("Joints") > 0)
				{
					int jointIndex = 0;

					for (auto& joint : layerJSON["Joints"])
					{
						std::string type = joint["type"].get<std::string>();
						float ax = joint["anchorpoint"]["x"].get<float>();
						float ay = joint["anchorpoint"]["y"].get<float>();
						float az = joint["anchorpoint"]["z"].get<float>();

						std::string bodyNameA = joint["bodyA"].get<std::string>();
						std::string bodyNameB = joint["bodyB"].get<std::string>();

						float axisX = joint["axis"]["x"].get<float>();
						float axisY = joint["axis"]["y"].get<float>();
						float axisZ = joint["axis"]["z"].get<float>();

						bool isLimited = joint["isLimited"].get<bool>();
						float maxLimit = joint["maxLimit"].get<float>();
						float minLimit = joint["minLimit"].get<float>();

						bool canCollide = joint["canCollide"].get<bool>();

						layer.m_layerData.m_joints.at(jointIndex).m_jointVectorValue = jointIndex;
						layer.m_layerData.m_joints.at(jointIndex).m_type = type;
						layer.m_layerData.m_joints.at(jointIndex).m_position = glm::vec3(ax, ay, az);
						layer.m_layerData.m_joints.at(jointIndex).m_bodyAName = bodyNameA;
						layer.m_layerData.m_joints.at(jointIndex).m_bodyBName = bodyNameB;

						layer.m_layerData.m_joints.at(jointIndex).m_bBodiesCanCollide = canCollide;
						layer.m_layerData.m_joints.at(jointIndex).m_axis = glm::vec3(axisX, axisY, axisZ);

						layer.m_layerData.m_joints.at(jointIndex).m_bIsLimited = isLimited;
						layer.m_layerData.m_joints.at(jointIndex).m_fMaxLimit = maxLimit;
						layer.m_layerData.m_joints.at(jointIndex).m_fMinLimit = minLimit;
						
						layer.m_layerData.m_joints.at(jointIndex).m_layer = joint["layerName"].get<std::string>();

						std::vector<std::string>::iterator it;
						for (it = layer.m_layerData.m_3DLayers.begin(); it != layer.m_layerData.m_3DLayers.end(); ++it)
						{
							if (*it == layer.m_layerData.m_joints.at(jointIndex).m_layer)
							{
								layer.m_layerData.m_joints.at(jointIndex).m_in3DLayer = true;
								break;
							}
						}

						jointIndex++;
					}
				}
			}
		}
		else LOG_INFO("None");
		// If there are UBOs in the layer
		if (layerJSON.count("UBOs") > 0)
		{
			// For each one
			for (auto& object : layerJSON["UBOs"])
			{
				// Get the name
				std::string uboName = object["name"].get<std::string>();
				// Get the UBO from the resource manager (make from the shader)
				std::shared_ptr<Engine::UniformBuffer> UBO = layer.getResources()->getUBO(uboName);
				std::vector<void*> uboData;
				if (object.count("data") > 0)
				{
					// For all the data in the UBO
					for (auto& object2 : object["data"])
					{
						void* ptr = nullptr;
						auto& type = object2["type"];
						// If it is a pointer, set it to point to the correct variable in the layer
						if (type == "pointer")
						{
							if (object2["var"].get<std::string>().compare("CAM_VIEWPROJ") == 0) ptr = (void*)&layer.getCamViewProj();
							if (object2["var"].get<std::string>().compare("ORHTOCAM_VIEWPROJ") == 0) ptr = (void*)&layer.getOrthoCamViewProj();
							if (object2["var"].get<std::string>().compare("CAM_POS") == 0) ptr = (void*)&layer.getCamPos();
						}
						// If it is a Float3
						if (type == "Float3")
						{
							// Add the data to the layer and set the pointer to the data
							layer.getData().push_back(new glm::vec3(object2["x"].get<float>(), object2["y"].get<float>(), object2["z"].get<float>()));
							ptr = (void*)&(*(glm::vec3*)layer.getData().back())[0];
						}
						uboData.push_back(ptr);
					}
				}
				// Set the scene data in the layer to this data
				(*layer.getSceneData())[UBO] = uboData;
			}
		}
		// If there are render commands in the layer
		if (layerJSON.count("RendererCommands") > 0)
		{
			std::string stages[6] = { "init", "predraw", "postdraw", "predrawUI", "postdrawUI", "exit" };
			// For each of the stages
			for (int i = 0; i < 6; i++)
			{
				// If there is a command for the stage
				if (layerJSON["RendererCommands"].count(stages[i]) > 0)
				{
					// For each one
					for (auto& object : layerJSON["RendererCommands"][stages[i]])
					{
						std::string type = object["type"].get<std::string>();

						std::shared_ptr<Engine::RenderCommand> command;

						bool keepAlive = true; // Adding to shared ptrs to leave deletion to the smart ptr
						// Make the command based on the type
						if (type.compare("ClearDepthColourBuffer") == 0)
						{
							command.reset(Engine::RenderCommand::clearDepthColourBufferCommand(keepAlive));
						}

						if (type.compare("SetClearColour") == 0)
						{
							float r = object["r"].get<float>();
							float g = object["g"].get<float>();
							float b = object["b"].get<float>();
							float a = object["a"].get<float>();
							command.reset(Engine::RenderCommand::setClearColourCommand(r, g, b, a, keepAlive));
						}

						if (type.compare("SetDepthTestLess") == 0)
						{
							bool enabled = object["enabled"].get<bool>();
							command.reset(Engine::RenderCommand::setDepthTestLessCommand(enabled, keepAlive));
						}

						if (type.compare("SetBackfaceCulling") == 0)
						{
							bool enabled = object["enabled"].get<bool>();
							command.reset(Engine::RenderCommand::setBackfaceCullingCommand(enabled, keepAlive));
						}

						if (type.compare("SetOneMinusAlphaBlending") == 0)
						{
							bool enabled = object["enabled"].get<bool>();
							command.reset(Engine::RenderCommand::setOneMinusAlphaBlendingCommand(enabled, keepAlive));
						}
						// Add it to the vector of commands for that stage
						if (i == 0) layer.getInitCommands().push_back(command);
						if (i == 1) layer.getPredrawCommands().push_back(command);
						if (i == 2) layer.getPostdrawCommands().push_back(command);
						if (i == 3) layer.getPredrawUICommands().push_back(command);
						if (i == 4) layer.getPostdrawUICommands().push_back(command);
						if (i == 5) layer.getExitCommands().push_back(command);
					}
				}
			}
		}
	}

	static std::string parseFilePath(const std::string& str)
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
};