#include "layerTests.h"

// Test the iterator returns correct layers in the correct order
TEST(LayerStack, Iterator)
{
	// Create layers on pointers
	layer1.reset(new LayerTest("Layer 1"));
	layer2.reset(new LayerTest("Layer 2"));
	// Add layers to back of layerstack vector
	stack.push(layer1);
	stack.push(layer2);

	std::string layerNames[2]; // To get layer names from stack iterator
	int i = 0;
	for (auto it = stack.begin(); it != stack.end(); it++)
	{
		layerNames[i] = (*it)->getName();
		i++;
	}
	// The names got should be in this order
	EXPECT_EQ(layerNames[0], "Layer 1");
	EXPECT_EQ(layerNames[1], "Layer 2");
}

// Test the reverse iterator returns correct layers in the correct order
TEST(LayerStack, ReverseIterator)
{
	// Create layer on pointer
	layer3.reset(new LayerTest("Layer 3"));
	// Add layer to back of layerstack vector
	stack.push(layer3);

	std::string layerNames[3]; // To get layer names from stack reverse iterator
	int i = 0;
	for (auto it = stack.rbegin(); it != stack.rend(); ++it)
	{
		layerNames[i] = (*it)->getName();
		i++;
	}
	// The names got should be in this order
	EXPECT_EQ(layerNames[0], "Layer 3");
	EXPECT_EQ(layerNames[1], "Layer 2");
	EXPECT_EQ(layerNames[2], "Layer 1");
}

// Test if using getCamera() to set the camera in a layer will work
TEST(LayerSetThings, Camera)
{
	// Make a test camera controller
	std::shared_ptr<Engine::CameraController> camera;
	camera.reset(new CameraControllerTest());
	// Use getCamera() to set the camera controller
	layer1->getCamera() = camera;
	// The layer camrea controller pointer should be the same as the one made here
	EXPECT_EQ(layer1->m_pCamera, camera);
}

// Test if using getRenderer() to set the renderer in a layer will work
TEST(LayerSetThings, Renderer)
{
	// Make a test renderer
	std::shared_ptr<Engine::Renderer> renderer;
	renderer.reset(new RendererTest());
	// Use getRenderer() to set the renderer
	layer1->getRenderer() = renderer;
	// The layer renderer pointer should be the same as the one made here
	EXPECT_EQ(layer1->m_pRenderer, renderer);
}