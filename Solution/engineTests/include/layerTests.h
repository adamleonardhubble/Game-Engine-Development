#pragma once
#include <gtest/gtest.h>
#define protected public
//TESTS FOR THE LAYERSTACK AND THE LAYER CLASSES
#include "systems/layerstack.h"
#include "core/layer.h"

#include "cameras/cameraController.h"
#include "rendering/renderer/renderer.h"

Engine::Layerstack stack; // Stack to test

// Class to inherit from Layer abstract class to provide implementation for pure virtual functions
class LayerTest : public Engine::Layer
{
public:
	LayerTest(const std::string& name) : Layer(name) {}

	virtual void onAttach() {}
	virtual void onDetach() {}
	virtual void onUpdate(float timestep) {}
	virtual void drawObjects() {}
	virtual void drawLayer() {}
	virtual void onEvent(Engine::Event& e) {}
};

// Class to inherit from CameraControler abstract class to provide implementation for pure virtual functions
class CameraControllerTest : public Engine::CameraController
{
	virtual void init(float, float, float, float, glm::vec3 = glm::vec3(0), glm::vec2 = glm::vec2(0)) {}
	virtual std::shared_ptr<Engine::Camera> getCamera() { return nullptr; }
	virtual void onUpdate(float timestep) {}
	virtual void onEvent(Engine::Event& e) {}
	virtual bool onResize(Engine::WindowResizeEvent& e) { return false; }
};

// Class to inherit from Renderer abstract class to provide implementation for pure virtual functions
class RendererTest : public Engine::Renderer
{
	virtual void actionCommand(Engine::RenderCommand* command) {}
	//! Function to prepare a scene for submission
	/*!
	\param sceneData A map of pointers to uniform buffers and the data to be set
	*/
	virtual void beginScene(const Engine::SceneData& sceneData) {}
	//! End a scene
	virtual void endScene() {}
	//! Function to submit a material for drawing
	/*!
	\param material A pointer to the material being drawn
	*/
	virtual void submit(const std::shared_ptr<Engine::Material>& material) {}
	virtual void flush() {}
};

// Pointers to layers for testing
std::shared_ptr<Engine::Layer> layer1;
std::shared_ptr<Engine::Layer> layer2;
std::shared_ptr<Engine::Layer> layer3;