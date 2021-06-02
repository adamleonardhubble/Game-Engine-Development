/** \file JSONLoader.h
*/
#pragma once

#include <fstream>
#include <reactphysics3d.h>

#include "json.hpp"
#include "JSONlayer.h"
#include "rendering/text/textLabel.h"
#include "cameras/freeOrthoCameraController2D.h"
#include "cameras/FPSCameraControllerEuler.h"
#include "cameras/fixedOrthoCameraController2D.h"
#include "cameras/cameraController3rdPerson.h"
#include "cameras/freeMoving3DCameraController.h"
#include "components/cameraComponents/freeMoving3DCameraComponent.h"
#include "components/cameraComponents/FPSCameraComponent.h"
#include "components/cameraComponents/fixed3DCameraComponent.h"
#include "components/cameraComponents/fixedOrthoCameraComponent.h"
#include "components/joints/ballSocketJointComponent.h"
#include "components/joints/hingeJoint.h"
#include "components/joints/fixedJoint.h"
#include "components/joints/sliderJoint.h"
#include "components/LuaScriptComponent.h"

#include "systems/log.h"
#include "systems/timer.h"

#include "assimpLoader.h"
// Temporary until we introduce ASSIMP
#include "textLoader.h"

#include <thread>

namespace Engine
{
	/**
	\class JSONLoader
	\brief Class which loads a layer from a JSON file
	*/
	class JSONLoader
	{
	public:
		//! Function to load all shaders for a layer
		/*!
		\param filepaths A vector containing all shader filepaths
		*/
		static void loadShaders(const std::vector<std::string>& filepaths)
		{
			// If there are shaders in the layer
			if (!filepaths.empty())
			{
				// Add them all to the resource manager
				for (auto& filepath : filepaths)
				{
					ResourceManager::addShader(filepath);
				}
			}
		}
		//! Function to load all textures for a layer
		/*!
		\param filepaths A vector containing all texture filepaths
		*/
		static void loadTextures(const std::vector<std::string>& filepaths)
		{
			// If there are any textures in the layer
			if (!filepaths.empty())
			{
				// Add them all to the resource manager
				for (auto& filepath : filepaths)
				{
					ResourceManager::addTexture(filepath);
				}
			}
		}
		//! Function to load all models for a layer
		/*!
		\param filepaths A vector containing all model filepaths
		*/
		static void loadModels(const std::vector<std::string>& filepaths)
		{
			// If there are models in the layer
			if (!filepaths.empty())
			{
				// Load models on separate threads
				std::vector<std::thread> loadThreads(filepaths.size());
				int i = 0;
				// Add them all to the resource manager
				for (auto& filepath : filepaths)
				{
					loadThreads[i] = std::thread(&ResourceManager::addModel, filepath);
					i++;
				}
				for (auto& thread : loadThreads)
				{
					thread.join();
				}
			}
		}
		//! Function to load all fonts for a layer
		/*!
		\param filepaths A vector containing pairs of font filepaths and sizes
		*/
		static void loadFonts(const std::vector<std::pair<std::string, int>>& filepaths)
		{
			// If there are any fonts used in the layer
			if (!filepaths.empty())
			{
				// Make a map with all of the fonts and sizes
				std::unordered_multimap<std::string, unsigned int> fonts;
				for (auto& filepath : filepaths)
				{
					fonts.insert(std::make_pair(filepath.first, filepath.second));
				}
				// Populate the character texture with all the characters in all the font and size pairs
				if (!fonts.empty()) ResourceManager::populateCharacters(fonts);
			}
		}
		//! Function to load all shaders, models, textures and fonts for all layers in a level
		/*!
		\param filepaths A map containing layer names and filepaths, so that the filepaths can be used
		*/
		static void AsyncLoad(const std::map<std::string, std::string>& filepaths)
		{
			std::vector<std::string> models;
			std::vector<std::string> shaders;
			std::vector<std::string> textures;
			std::vector<std::pair<std::string, int>> fonts;

			for (auto& filepath : filepaths)
			{
				std::string path = filepath.first;
				path.erase(0, 1);

				std::fstream handle(path, std::ios::in);
				if (!handle.is_open()) {
					LOG_CRITICAL("Could not open file: {0}", path);
				}
				// Make the JSON loader to get all of the data
				nlohmann::json layerJSON;
				handle >> layerJSON;

				// Make vectors of filepaths for each type of thing being loaded

				if (layerJSON.count("Asyncload") > 0)
				{
					if (layerJSON["Asyncload"].count("models") > 0)
					{
						for (auto& modelFilepath : layerJSON["Asyncload"]["models"])
						{
							std::string actualPath = modelFilepath["filepath"].get<std::string>();
							if (!std::count(models.begin(), models.end(), actualPath))
							{
								models.push_back(actualPath);
							}
						}
					}

					if (layerJSON["Asyncload"].count("shaders") > 0)
					{
						for (auto& shaderFilepath : layerJSON["Asyncload"]["shaders"])
						{
							std::string actualPath = shaderFilepath["filepath"].get<std::string>();
							if (!std::count(shaders.begin(), shaders.end(), actualPath))
							{
								shaders.push_back(actualPath);
							}
						}
					}

					if (layerJSON["Asyncload"].count("textures") > 0)
					{
						for (auto& textureFilepath : layerJSON["Asyncload"]["textures"])
						{
							std::string actualPath = textureFilepath["filepath"].get<std::string>();
							if (!std::count(textures.begin(), textures.end(), actualPath))
							{
								textures.push_back(actualPath);
							}
						}
					}

					if (layerJSON["Asyncload"].count("fonts") > 0)
					{
						for (auto& fontFilepath : layerJSON["Asyncload"]["fonts"])
						{
							std::string actualPath = fontFilepath["filepath"].get<std::string>();
							int fontSize = fontFilepath["charSize"].get<int>();

							if (!std::count(fonts.begin(), fonts.end(), std::make_pair(actualPath, fontSize)))
							{
								fonts.push_back(std::make_pair(actualPath, fontSize));
							}
						}
					}
				}
			}
			
			// Create a separate thread to load models
			std::thread modelThread(&JSONLoader::loadModels, models);

			// Load shaders, textures and fonts on this thread
			loadShaders(shaders);
			loadTextures(textures);
			loadFonts(fonts);
			// Sync with the model loading thread
			modelThread.join();
		}

		//! Function which loads a layer
		/*!
		\param filepath The location of the JSON file being loaded
		\param layer The layer that the file is being loaded to
		*/
		static void load(const std::string& filepath, JSONLayer& layer)
		{
			std::string path = filepath;
			path.erase(0, 1);

			// Get the data from the JSON file
			std::fstream handle(path, std::ios::in);
			if (!handle.is_open()) {
				LOG_CRITICAL("Could not open file: {0}", path);
			}
			// Make the JSON loader to get all of the data
			nlohmann::json layerJSON;
			handle >> layerJSON;

			// If the layer has a camera (not attached to a player component)
			if (layerJSON.count("camera") > 0)
			{
				// Get the type of camera
				std::string type = layerJSON["camera"]["type"].get<std::string>();
				// For 3D cameras, get from the JSON file if it will be updating at the start, make the camera and initialize the camera with the
				// fov, aspect ratio, near and far clip planes, and the start position, all with data from the JSON file
				// For 2D cameras, get from the JSON file if it will be updating at the start, make the camera and initialize the camera with the
				// top and left positions, and the width and height of the view area
				if (type.compare("1stPerson") == 0)
				{
					bool update = layerJSON["camera"]["update"].get<bool>();
					layer.getCamera().reset(new FPSCameraControllerEuler(update));
					float fov = layerJSON["camera"]["fov"].get<float>();
					float aspectRatio = layerJSON["camera"]["aspectRatio"].get<float>();
					float nearClip = layerJSON["camera"]["nearClip"].get<float>();
					float farClip = layerJSON["camera"]["farClip"].get<float>();
					float xPos = layerJSON["camera"]["position"]["x"].get<float>();
					float yPos = layerJSON["camera"]["position"]["y"].get<float>();
					float zPos = layerJSON["camera"]["position"]["z"].get<float>();
					layer.getCamera()->init(fov, aspectRatio, nearClip, farClip, glm::vec3(xPos, yPos, zPos));
				}
				else if (type.compare("Free3D") == 0)
				{
					bool update = layerJSON["camera"]["update"].get<bool>();
					layer.getCamera().reset(new Free3DCameraController(update));
					float fov = layerJSON["camera"]["fov"].get<float>();
					float aspectRatio = layerJSON["camera"]["aspectRatio"].get<float>();
					float nearClip = layerJSON["camera"]["nearClip"].get<float>();
					float farClip = layerJSON["camera"]["farClip"].get<float>();
					float xPos = layerJSON["camera"]["position"]["x"].get<float>();
					float yPos = layerJSON["camera"]["position"]["y"].get<float>();
					float zPos = layerJSON["camera"]["position"]["z"].get<float>();
					layer.getCamera()->init(fov, aspectRatio, nearClip, farClip, glm::vec3(xPos, yPos, zPos));
				}
				else if (type.compare("FreeOrtho2D") == 0)
				{
					bool update = layerJSON["camera"]["update"].get<bool>();
					layer.getCamera().reset(new FreeOrthoCameraController2D(update));
					float top = layerJSON["camera"]["top"].get<float>();
					float left = layerJSON["camera"]["left"].get<float>();
					float width = layerJSON["camera"]["width"].get<float>();
					float height = layerJSON["camera"]["height"].get<float>();
					layer.getCamera()->init(left, top, width, height);
				}
				else if (type.compare("FixedOrtho2D") == 0)
				{
					layer.getCamera().reset(new FixedOrthoCameraController2D());
					float top = layerJSON["camera"]["top"].get<float>();
					float left = layerJSON["camera"]["left"].get<float>();
					float width = layerJSON["camera"]["width"].get<float>();
					float height = layerJSON["camera"]["height"].get<float>();
					layer.getCamera()->init(left, top, width, height);
				}
				else if (type.compare("3rdPerson") == 0)
				{
					bool update = layerJSON["camera"]["update"].get<bool>();
					layer.getCamera().reset(new CameraController3rdPerson(update));
					float fov = layerJSON["camera"]["fov"].get<float>();
					float aspectRatio = layerJSON["camera"]["aspectRatio"].get<float>();
					float nearClip = layerJSON["camera"]["nearClip"].get<float>();
					float farClip = layerJSON["camera"]["farClip"].get<float>();
					float xPos = layerJSON["camera"]["position"]["x"].get<float>();
					float yPos = layerJSON["camera"]["position"]["y"].get<float>();
					float zPos = layerJSON["camera"]["position"]["z"].get<float>();
					layer.getCamera()->init(fov, aspectRatio, nearClip, farClip, glm::vec3(xPos, yPos, zPos));
				}
			}
			// If the layer has a renderer
			if (layerJSON.count("renderer") > 0)
			{
				// Make a renderer for the layer depending on the type in the JSON file
				std::string type = layerJSON["renderer"]["type"].get<std::string>();
				if (type.compare("Basic3D") == 0) layer.getRenderer().reset(Renderer::createBasic3D());
				if (type.compare("BasicText2D") == 0) layer.getRenderer().reset(Renderer::createText());
			}
			if (layerJSON.count("3DLayer") > 0)
			{
				bool is3D = layerJSON["3DLayer"].get<bool>();
				layer.setIf3DLayer(is3D);
			}
			// If there is memory info (for objects)
			if (layerJSON.count("MemoryInfo") > 0)
			{
				// Set the sizes of all of the vectors containing the gameobjects and all of the components on the objects in the layer
				layer.getGameObjects().resize(layerJSON["MemoryInfo"]["gameObjects"].get<int>());
				layer.getMaterials().resize(layerJSON["MemoryInfo"]["materials"].get<int>());
				layer.getMeshes().resize(layerJSON["MemoryInfo"]["mesh"].get<int>());
				layer.getTexts().resize(layerJSON["MemoryInfo"]["text"].get<int>());
				layer.getPositions().resize(layerJSON["MemoryInfo"]["position"].get<int>());
				layer.getVelocities().resize(layerJSON["MemoryInfo"]["velocity"].get<int>());
				layer.getTextures().resize(layerJSON["MemoryInfo"]["texture"].get<int>());
				layer.getColliders().resize(layerJSON["MemoryInfo"]["colliders"].get<int>());
				int rigidbodies = layerJSON["MemoryInfo"]["rigidbody"].get<int>();
				if (rigidbodies > 0) // If there are rigidbodies in the layer
					layer.getDynamicSystem()->start(); // Start the dynamics physics system
				layer.getRigidbodies().resize(rigidbodies);
				layer.getControllers().resize(layerJSON["MemoryInfo"]["controllers"].get<int>());
				layer.getLights().resize(layerJSON["MemoryInfo"]["lights"].get<int>());
				layer.getCameras().resize(layerJSON["MemoryInfo"]["cameras"].get<int>());
				layer.getLuaScripts().resize(layerJSON["MemoryInfo"]["luascript"].get<int>());
				layer.getJoints().resize(layerJSON["MemoryInfo"]["joints"].get<int>());
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
				int velocitiesIndex = 0;
				int texturesIndex = 0;
				int collidersIndex = 0;
				int rigidBodiesIndex = 0;
				int controllersIndex = 0;
				int lightIndex = 0;
				int cameraIndex = 0;
				int luascriptIndex = 0;
				// For all gameobjects in the layer
				for (auto& object : layerJSON["GameObjects"])
				{
					int objectIndex = goIndex;
					if (object.count("index") > 0)
						objectIndex = object["index"].get<int>();

					// Create gameobject in the layer
					std::string name = "Name";
					if (object.count("name") > 0) name = object["name"].get<std::string>();
					std::string tag = object["tag"].get<std::string>();
					layer.getGameObjects().at(goIndex) = (std::make_shared<GameObject>(GameObject(name, tag, objectIndex)));
					auto gameObject = layer.getGameObjects().at(goIndex);
					goIndex++;


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
						// Make the text label
						std::shared_ptr<TextLabel> label;
						label.reset(TextLabel::create(font, charSize, text, glm::vec2(posX, posY), rot, scale, glm::vec3(r, g, b)));
						// Make a compoent with the text label and add it to the gameobject
						layer.getTexts().at(textIndex) = std::make_shared<TextComponent>(TextComponent(label));
						gameObject->addComponent(layer.getTexts().at(textIndex));
						textIndex++;
					}
					if (object.count("mesh") > 0) {
						std::shared_ptr<Engine::Model> model = layer.getResources()->addModel(object["mesh"]["model"]);

						layer.getMeshes().at(meshIndex) = std::make_shared<Engine::MeshComponent>(Engine::MeshComponent(model));
						gameObject->addComponent(layer.getMeshes().at(meshIndex));
						meshIndex++;
					}
					// If there is a material on the object
					if (object.count("material") > 0) {
						if (object["material"]["shader"].get<std::string>().compare("text") == 0) // If it is text
						{
							// Get the material made for the text label
							auto& mat = layer.getTexts().at(textIndex - 1)->getLabel()->getMaterial();
							// Add the label colour to the layer data
							layer.getData().push_back((void*)new glm::vec3(layer.getTexts().at(textIndex - 1)->getLabel()->getColour()));
							// Set the colour uniform
							mat->setDataElement("u_fontColour", (void*)&(*(glm::vec3*)layer.getData().back())[0]);
							// Make a component with the material and add it to the object
							layer.getMaterials().at(materialsIndex) = std::make_shared<MaterialComponent>(MaterialComponent(mat));
							gameObject->addComponent(layer.getMaterials().at(materialsIndex));
							materialsIndex++;
						}
						// If it is a model
						else if (object.count("mesh") > 0)
						{
							std::shared_ptr<Engine::Model> model = layer.getMeshes().at(meshIndex - 1)->getModel();
							std::shared_ptr<Shader> shader = layer.getResources()->addShader(object["material"]["shader"]);
							// Create the material
							std::shared_ptr<VertexArray> VAO = layer.getResources()->addVAO(object["mesh"]["model"]);
							VAO->setVertexBuffer(layer.getResources()->addVBO(object["mesh"]["model"], model->vertices, sizeof(float) * model->verticesSize, shader->getBufferLayout()));
							VAO->setIndexBuffer(layer.getResources()->addIndexBuffer(object["mesh"]["model"], model->indices, model->indicesSize));
							std::shared_ptr<Material> mat = layer.getResources()->addMaterial(name + std::to_string(goIndex - 1), shader, VAO);
							// Create a component with the material
							layer.getMaterials().at(materialsIndex) = std::make_shared<MaterialComponent>(MaterialComponent(mat));
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
							materialsIndex++;
						}
					}
					// If there is a position component
					if (object.count("position") > 0) {
						// If it is a text object
						if (object["position"]["type"].get<std::string>().compare("text") == 0)
						{
							// Make the position component using the transform from the text label
							layer.getPositions().at(positionsIndex) = std::make_shared<PositionComponent>(PositionComponent(
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
							layer.getPositions().at(positionsIndex) = std::make_shared<PositionComponent>(PositionComponent(translation, rotation, scale));
						}
						// Add the component to the object
						gameObject->addComponent(layer.getPositions().at(positionsIndex));
						positionsIndex++;
					}
					// If there is a velocity component
					if (object.count("velocity") > 0) {
						// Get the velocity values from the JSON file
						glm::vec3 linear(object["velocity"]["linear"]["x"].get<float>(), object["velocity"]["linear"]["y"].get<float>(), object["velocity"]["linear"]["z"].get<float>());
						glm::vec3 angular(object["velocity"]["angular"]["x"].get<float>(), object["velocity"]["angular"]["y"].get<float>(), object["velocity"]["angular"]["z"].get<float>());
						// Make a velocity component and add it to the object
						layer.getVelocities().at(velocitiesIndex) = std::make_shared<VelocityComponent>(VelocityComponent(linear, angular));
						gameObject->addComponent(layer.getVelocities().at(velocitiesIndex));
						velocitiesIndex++;
					}
					// If there is a texture component
					if (object.count("texture") > 0) {
						// Get the needed texture (text or not) from the resource manager
						std::shared_ptr<Texture> tex;
						if (object["texture"]["diffuse"].get<std::string>().compare("text") == 0)
						{
							tex = layer.getResources()->getFontTexture();
							// Make a texture component with both textures and add it to the object
							layer.getTextures().at(texturesIndex) = std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, tex));
						}
						else
						{
							tex = layer.getResources()->addTexture(object["texture"]["diffuse"].get<std::string>());

							std::string normalFileName = ResourceManager::parseFilePath(object["texture"]["normal"].get<std::string>());
							std::string depthFileName = ResourceManager::parseFilePath(object["texture"]["depth"].get<std::string>());

							if (normalFileName.compare("None") == 0 && depthFileName.compare("None") == 0)
							{
								std::shared_ptr<Engine::Texture> specTex = layer.getResources()->addTexture(object["texture"]["specular"].get<std::string>());
								// Make a texture component with both textures and add it to the object
								layer.getTextures().at(texturesIndex) = std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, specTex));
							}
							else
							{
								if (normalFileName.compare("None") != 0 && depthFileName.compare("None") == 0)
								{
									std::shared_ptr<Engine::Texture> specTex = layer.getResources()->addTexture(object["texture"]["specular"].get<std::string>());
									std::shared_ptr<Engine::Texture> normMap = layer.getResources()->addTexture(object["texture"]["normal"].get<std::string>());
									// Make a texture component with both textures and add it to the object
									layer.getTextures().at(texturesIndex) = std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, specTex, normMap));
								}
								else if (normalFileName.compare("None") == 0 && depthFileName.compare("None") != 0)
								{
									std::shared_ptr<Engine::Texture> specTex = layer.getResources()->addTexture(object["texture"]["specular"].get<std::string>());
									std::shared_ptr<Engine::Texture> depthMap = layer.getResources()->addTexture(object["texture"]["depth"].get<std::string>());
									float heightScale = object["texture"]["heightScale"].get<float>();
									// Make a texture component with both textures and add it to the object
									layer.getTextures().at(texturesIndex) = std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, specTex, depthMap, heightScale));
								}
								else if (normalFileName.compare("None") != 0 && depthFileName.compare("None") != 0)
								{
									std::shared_ptr<Engine::Texture> specTex = layer.getResources()->addTexture(object["texture"]["specular"].get<std::string>());
									std::shared_ptr<Engine::Texture> normMap = layer.getResources()->addTexture(object["texture"]["normal"].get<std::string>());
									std::shared_ptr<Engine::Texture> depthMap = layer.getResources()->addTexture(object["texture"]["depth"].get<std::string>());
									float heightScale = object["texture"]["heightScale"].get<float>();
									// Make a texture component with both textures and add it to the object
									layer.getTextures().at(texturesIndex) = std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, specTex, normMap, depthMap, heightScale));
								}
							}
						}
						gameObject->addComponent(layer.getTextures().at(texturesIndex));
						texturesIndex++;
					}
					// If there is a collider component
					if (object.count("collider") > 0) {
						float mass = object["collider"]["mass"].get<float>();
						CollisionCategory collisionCat = object["collider"]["collisionCategory"].get<CollisionCategory>();

						unsigned short canCollideWith = 0;
						for (auto& category : object["collider"]["canCollideWith"])
						{
							canCollideWith |= category.get<unsigned short>();
						}

						// If it is a box collider
						if (object["collider"]["shape"].get<std::string>().compare("box") == 0)
						{
							// Get the half extents and make the component
							float x = object["collider"]["halfExtents"]["x"].get<float>();
							float y = object["collider"]["halfExtents"]["y"].get<float>();
							float z = object["collider"]["halfExtents"]["z"].get<float>();

							glm::vec3 halfExtents = glm::vec3(x, y, z);

							layer.getColliders().at(collidersIndex) = std::make_shared<BoxCollisionComponent>(BoxCollisionComponent(
								halfExtents, mass, collisionCat, canCollideWith));
						}
						else if (object["collider"]["shape"].get<std::string>().compare("sphere") == 0) // If it is a sphere collider
						{
							// Get the radius and make the component
							float radius = object["collider"]["radius"].get<float>();

							layer.getColliders().at(collidersIndex) = std::make_shared<SphereCollisionComponent>(SphereCollisionComponent(
								radius, mass, collisionCat, canCollideWith));
						}
						else if (object["collider"]["shape"].get<std::string>().compare("capsule") == 0) // If it is a capsule collider
						{
							// Get the radius and height and make the component
							float radius = object["collider"]["radius"].get<float>();
							float height = object["collider"]["height"].get<float>();

							layer.getColliders().at(collidersIndex) = std::make_shared<CapsuleCollisionComponent>(CapsuleCollisionComponent(
								radius, height, mass, collisionCat, canCollideWith));
						}
						else if (object["collider"]["shape"].get<std::string>().compare("convex mesh") == 0)
						{
							// Get the radius and height and make the component
							std::string filepath = object["collider"]["model"].get<std::string>();

							std::shared_ptr<Model> model = layer.getResources()->addModel(filepath);

							layer.getColliders().at(collidersIndex) = std::make_shared<ConvexMeshCollisionComponent>(ConvexMeshCollisionComponent(
								mass, model, layer.getPositions().at(positionsIndex - 1)->getScale(), collisionCat, canCollideWith));
						}
						// Add the component to the object
						gameObject->addComponent(layer.getColliders().at(collidersIndex));
						collidersIndex++;
					}
					// If there is a rigidbody component
					if (object.count("rigidbody") > 0) {
						rp3d::BodyType type;
						bool usesGravity = false;
						// Set the body type
						auto strType = object["rigidbody"]["type"].get<std::string>();
						if (strType.compare("dynamic") == 0)
						{
							type = rp3d::BodyType::DYNAMIC;
							// If it is dynamic, get is it uses gravity
							usesGravity = object["rigidbody"]["gravity"].get<bool>();
						}
						else if (strType.compare("static") == 0)    type = rp3d::BodyType::STATIC;
						else if (strType.compare("kinematic") == 0) type = rp3d::BodyType::KINEMATIC;
						// Make the component
						layer.getRigidbodies().at(rigidBodiesIndex) = std::make_shared<RigidBodyComponent>(RigidBodyComponent(
							layer.getDynamicSystem()->getWorld(), 
							layer.getPositions().at(positionsIndex - 1)->getPosition(), 
							layer.getPositions().at(positionsIndex - 1)->getRotation(),
							layer.getDynamicSystem()->getTimeAccumulator(), type, usesGravity,
							layer.getDynamicSystem()->getTimestep()));
						// Set the the values in material of the body
						rp3d::Material* mat = layer.getRigidbodies().at(rigidBodiesIndex)->getPhysicsMaterial();
						float bounce = object["rigidbody"]["material"]["bounciness"].get<float>();
						float friction = object["rigidbody"]["material"]["friction"].get<float>();
						float rolling = object["rigidbody"]["material"]["rollingResistance"].get<float>();

						mat->setBounciness(rp3d::decimal(bounce));
						mat->setFrictionCoefficient(rp3d::decimal(friction));
						mat->setRollingResistance(rp3d::decimal(rolling));
						// Add the component to the object
						gameObject->addComponent(layer.getRigidbodies().at(rigidBodiesIndex));
						rigidBodiesIndex++;
					}
					// If there is a texture switch component
					if (object.count("textureSwitch") > 0) {
						// Make the component with the textures and add it to the object
						std::shared_ptr<Texture> tex1 = layer.getResources()->addTexture(object["textureSwitch"]["tex1"].get<std::string>());
						std::shared_ptr<Texture> tex2 = layer.getResources()->addTexture(object["textureSwitch"]["tex2"].get<std::string>());
						layer.getControllers().at(controllersIndex) = std::make_shared<TextureSwitchComponent>(TextureSwitchComponent(tex1, tex2));
						gameObject->addComponent(layer.getControllers().at(controllersIndex));
						controllersIndex++;
					}
					// If there is an oscillate component
					if (object.count("oscillate") > 0) {
						OscilateComponent::state state;
						// Get the starting state
						auto stateStr = object["oscillate"]["state"].get<std::string>();
						if (stateStr.compare("down") == 0) state = OscilateComponent::state::DOWN;
						if (stateStr.compare("stopped") == 0) state = OscilateComponent::state::STOPPED;
						if (stateStr.compare("up") == 0) state = OscilateComponent::state::UP;
						float time = object["oscillate"]["time"].get<float>();
						// Make the component and add it to the object
						layer.getControllers().at(controllersIndex) = std::make_shared<OscilateComponent>(OscilateComponent(state, time));
						gameObject->addComponent(layer.getControllers().at(controllersIndex));
						controllersIndex++;
					}
					// If there is a rotate component
					if (object.count("rotate") > 0) {
						// Make the component and add it to the object
						layer.getControllers().at(controllersIndex) = std::make_shared<RotateComponent>(RotateComponent());
						gameObject->addComponent(layer.getControllers().at(controllersIndex));
						controllersIndex++;
					}
					// If there is a player component
					if (object.count("player") > 0) {
						// If the component needs a camera(s) attached
						if (object["player"].count("cameras") > 0)
						{
							bool update;
							std::pair<std::shared_ptr<CameraController>, std::shared_ptr<CameraController>> cameras = 
								std::make_pair(nullptr, nullptr);
							// For each possible camera
							for (auto& cam : object["player"]["cameras"])
							{
								// Get the type of camera (can only be 3D cameras)
								std::string type = cam["type"].get<std::string>();
								// Make the camera like above
								if (type.compare("1stPerson") == 0)
								{
									update = object["player"]["update"].get<bool>();
									layer.getCamera().reset(new FPSCameraControllerEuler(update));
									float fov = cam["fov"].get<float>();
									float aspectRatio = cam["aspectRatio"].get<float>();
									float nearClip = cam["nearClip"].get<float>();
									float farClip = cam["farClip"].get<float>();
									layer.getCamera()->init(fov, aspectRatio, nearClip, farClip, layer.getPositions().at(positionsIndex - 1)->getPosition());
								}
								else if (type.compare("3rdPerson") == 0)
								{
									update = object["player"]["update"].get<bool>();
									layer.getCamera().reset(new CameraController3rdPerson(update));
									float fov = cam["fov"].get<float>();
									float aspectRatio = cam["aspectRatio"].get<float>();
									float nearClip = cam["nearClip"].get<float>();
									float farClip = cam["farClip"].get<float>();
									layer.getCamera()->init(fov, aspectRatio, nearClip, farClip, layer.getPositions().at(positionsIndex - 1)->getPosition());
								}
								// Set the camera pointers on the component
								if (cameras.first == nullptr) cameras.first = layer.getCamera();
								else cameras.second = layer.getCamera();
							}
							bool which = object["player"]["which"].get<bool>();
							// Make the component and add it to the object (there can only be one of these in the layer)
							std::shared_ptr<PlayerComponent> player = std::make_shared<PlayerComponent>(PlayerComponent(
								update, layer.getPositions().at(positionsIndex - 1)->getPosition(), cameras, which));
							layer.getPlayer() = player;
							gameObject->addComponent(layer.getPlayer());
							layer.getCamera() = layer.getPlayer()->getCurrentCamera();
						}
					}

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
						(*layer.getLightSceneData())[UBO] = uboData;

						UBO = layer.getResources()->getUBO("LightPos");

						std::vector<void*> uboData2;
						uboData2.push_back((void*)&layer.getPositions().at(positionsIndex - 1)->getPosition());
						uboData2.push_back((void*)&layer.getPositions().at(positionsIndex - 1)->getRotationDeg());
						uboData2.push_back((void*)&layer.getCamPos());

						// Set the scene data in the layer to this data
						(*layer.getLightSceneData())[UBO] = uboData2;

						layer.getLights().at(lightIndex) = std::make_shared<Engine::LightComponent>(
							Engine::LightComponent(ambient, pointDiffuse, pointSpecular, dirDiffuse, dirSpecular, attenuation));
						gameObject->addComponent(layer.getLights().at(lightIndex));
						lightIndex++;
					}

					if (object.count("camera") > 0) {
						std::string type = object["camera"]["type"].get<std::string>();

						if (layer.getIf3DLayer())
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
								layer.getCameras().at(cameraIndex) = std::make_shared<FreeMoving3DCameraComponent>(FreeMoving3DCameraComponent(
									fov, aspect, nearClip, farClip, layer.getPositions().at(positionsIndex - 1)->getPosition(),
									glm::vec2(glm::degrees(rotX), glm::degrees(rotY)), mouse));
							}
							else if (type.compare("1stPerson") == 0)
							{
								layer.getCameras().at(cameraIndex) = std::make_shared<FPSCameraComponent>(FPSCameraComponent(
									fov, aspect, nearClip, farClip, layer.getPositions().at(positionsIndex - 1)->getPosition(),
									glm::vec2(glm::degrees(rotX), glm::degrees(rotY)), mouse));
							}
							else if (type.compare("fixed3D") == 0)
							{
								layer.getCameras().at(cameraIndex) = std::make_shared<Fixed3DCameraComponent>(Fixed3DCameraComponent(
									fov, aspect, nearClip, farClip, layer.getPositions().at(positionsIndex - 1)->getPosition(),
									glm::vec2(glm::degrees(rotX), glm::degrees(rotY))));
							}
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
							}
						}
						std::vector<std::string> layersSeen;
						for (auto& it : object["camera"]["layers"])
						{
							layersSeen.push_back(it.get<std::string>());
						}

						layer.getCameras().at(cameraIndex)->setLayersSeen(layersSeen);

						gameObject->addComponent(layer.getCameras().at(cameraIndex));

						if (layer.getIf3DLayer())
							layer.setCurrentCamera(layer.getCameras().at(0));
						else
							layer.setCurrent2DCamera(layer.getCameras().at(cameraIndex));

						cameraIndex++;
					}
					if (object.count("luascript") > 0)
					{
						// Get Script Name
						std::string sScriptName = object["luascript"]["Filepath"].get<std::string>();

						// Make Input Struct
						LuaScriptComponent<DynamicPhysicsSystem>::LuaScriptVariables InputParameters;
						InputParameters.vInputObjectPosition = &layer.getPositions().at(positionsIndex - 1)->getPosition();
						InputParameters.vInputObjectRotation = &layer.getPositions().at(positionsIndex - 1)->getRotationDeg();
						InputParameters.vInputObjectScale = &layer.getPositions().at(positionsIndex - 1)->getScale();
						InputParameters.MyJSONLayer = &layer;

						// Get RidgidBody Component?
						if (object.count("rigidbody") > 0)
							InputParameters.MyRigidBodyComponent = layer.getRigidbodies().at(rigidBodiesIndex - 1);
						else
							InputParameters.MyRigidBodyComponent = nullptr;
						
						// Get Light Component Component?
						if (object.count("light") > 0)
							InputParameters.MyLightComponent = layer.getLights().at(lightIndex - 1);
						else
							InputParameters.MyLightComponent = nullptr;

						// Get Material Component?
						if (object.count("materials") > 0)
							InputParameters.MyMaterialComponent = layer.getMaterials().at(materialsIndex - 1);
						else
							InputParameters.MyMaterialComponent = nullptr;

						// Get Mesh Component?
						if (object.count("mesh") > 0)
							InputParameters.MyMeshComponent = layer.getMeshes().at(meshIndex - 1);
						else
							InputParameters.MyMeshComponent = nullptr;

						// Get Texture Component?
						if (object.count("texture") > 0)
							InputParameters.MyTextureComponent = layer.getTextures().at(texturesIndex - 1);
						else
							InputParameters.MyTextureComponent = nullptr;

						// Make Component
						layer.getLuaScripts().at(luascriptIndex) = std::make_shared<LuaScriptComponent<DynamicPhysicsSystem>>(LuaScriptComponent<DynamicPhysicsSystem>(sScriptName, InputParameters));
						gameObject->addComponent(layer.getLuaScripts().at(luascriptIndex));
						luascriptIndex++;
					}
				}
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

						std::shared_ptr<RigidBodyComponent> a;
						std::shared_ptr<RigidBodyComponent> b;

						std::shared_ptr<GameObject> objectA;

						for (auto& object : layer.getGameObjects())
						{
							if (object->name().compare(bodyNameA) == 0)
							{
								a = std::dynamic_pointer_cast<RigidBodyComponent>((*object->getComponent<RigidBodyComponent>()));
								objectA = object;
							}
							if (object->name().compare(bodyNameB) == 0)
								b = std::dynamic_pointer_cast<RigidBodyComponent>((*object->getComponent<RigidBodyComponent>()));
						}

						if (type.compare("Ball and Socket") == 0)
						{
							layer.getJoints().at(jointIndex) = std::make_shared<BallSocketJointComponent>(BallSocketJointComponent(
								layer.getDynamicSystem()->getWorld(),
								glm::vec3(ax, ay, az),
								a->getBody(),
								b->getBody(),
								canCollide));
						}
						if (type.compare("Hinge") == 0)
						{
							layer.getJoints().at(jointIndex) = std::make_shared<HingeJointComponent>(HingeJointComponent(
								layer.getDynamicSystem()->getWorld(),
								glm::vec3(ax, ay, az),
								glm::vec3(axisX, axisY, axisZ),
								a->getBody(),
								b->getBody(),
								isLimited,
								minLimit,
								maxLimit,
								canCollide));
						}
						if (type.compare("Fixed") == 0)
						{
							layer.getJoints().at(jointIndex) = std::make_shared<FixedJointComponent>(FixedJointComponent(
								layer.getDynamicSystem()->getWorld(),
								glm::vec3(ax, ay, az),
								a->getBody(),
								b->getBody(),
								canCollide));
						}
						if (type.compare("Slider") == 0)
						{
							rp3d::Vector3 posA = a->getBody()->getTransform().getPosition();
							rp3d::Vector3 posB = b->getBody()->getTransform().getPosition();

							rp3d::Vector3 anchor = 0.5f * (posA + posB);

							rp3d::Vector3 axis = posB - posA;

							layer.getJoints().at(jointIndex) = std::make_shared<SliderJointComponent>(SliderJointComponent(
								layer.getDynamicSystem()->getWorld(),
								glm::vec3(anchor.x, anchor.y, anchor.z),
								a->getBody(),
								b->getBody(),
								axis,
								isLimited,
								minLimit,
								maxLimit,
								canCollide));
						}
						objectA->addComponent(layer.getJoints().at(jointIndex));

						jointIndex++;
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
					std::shared_ptr<UniformBuffer> UBO = layer.getResources()->getUBO(uboName);
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
								if (object2["var"].get<std::string>().compare("CAM_VIEW") == 0) ptr = (void *)&layer.getCamView();
								if (object2["var"].get<std::string>().compare("CAM_PROJ") == 0) ptr = (void *)&layer.getCamProj();
								if (object2["var"].get<std::string>().compare("CAM_VIEWPROJ") == 0) ptr = (void *)&layer.getCamViewProj();
								if (object2["var"].get<std::string>().compare("CAM_POS") == 0) ptr = (void *)&layer.getCamPos();
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
				std::string stages[6] = { "init", "predrawFramebuffer", "predrawLayerTexture", "postdrawFramebuffer", "postdrawLayerTexture", "exit" };
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

							std::shared_ptr<RenderCommand> command;

							bool keepAlive = true; // Adding to shared ptrs to leave deletion to the smart ptr
							// Make the command based on the type
							if (type.compare("ClearDepthColourBuffer") == 0)
							{
								command.reset(RenderCommand::clearDepthColourBufferCommand(keepAlive));
							}

							if (type.compare("ClearColourBuffer") == 0)
							{
								command.reset(RenderCommand::clearColourBufferCommand(keepAlive));
							}

							if (type.compare("SetClearColour") == 0)
							{
								float r = object["r"].get<float>();
								float g = object["g"].get<float>();
								float b = object["b"].get<float>();
								float a = object["a"].get<float>();
								command.reset(RenderCommand::setClearColourCommand(r, g, b, a, keepAlive));
							}

							if (type.compare("SetDepthTestLess") == 0)
							{
								bool enabled = object["enabled"].get<bool>();
								command.reset(RenderCommand::setDepthTestLessCommand(enabled, keepAlive));
							}

							if (type.compare("SetBackfaceCulling") == 0)
							{
								bool enabled = object["enabled"].get<bool>();
								command.reset(RenderCommand::setBackfaceCullingCommand(enabled, keepAlive));
							}

							if (type.compare("SetOneMinusAlphaBlending") == 0)
							{
								bool enabled = object["enabled"].get<bool>();
								command.reset(RenderCommand::setOneMinusAlphaBlendingCommand(enabled, keepAlive));
							}
							// Add it to the vector of commands for that stage
							if (i == 0) layer.getInitCommands().push_back(command);
							if (i == 1) layer.getPredrawFramebufferCommands().push_back(command);
							if (i == 2) layer.getPredrawLayerTextureCommands().push_back(command);
							if (i == 3) layer.getPostdrawFramebufferCommands().push_back(command);
							if (i == 4) layer.getPostdrawLayerTextureCommands().push_back(command);
							if (i == 5) layer.getExitCommands().push_back(command);
						}
					}
				}
			}

			// Run OnGameStart for Lua Functions
			for (int i = 0; i < layer.getLuaScripts().size(); i++)
			{
				layer.getLuaScripts().at(i)->onGameStart();
			}
		}
	};
}