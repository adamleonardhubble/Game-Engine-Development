#include "gameObjectTests.h"

// Test the iterator returns correct components in the correct order
TEST(Gameobject, ComponentIterator)
{
	// Create the components
	component1.reset(new ComponentTest);
	controllerComponent1.reset(new ControllerComponentTest);
	component2.reset(new ComponentTest);
	controllerComponent2.reset(new ControllerComponentTest);
	// Add the components to the object
	object.addComponent(component1);
	object.addComponent(controllerComponent1);
	object.addComponent(component2);
	object.addComponent(controllerComponent2);

	std::shared_ptr<Engine::Component> component[4]; // To get component pointers from the object
	int i = 0;
	for (auto it = object.begin(); it != object.end(); it++)
	{
		component[i] = *it;
		i++;
	}
	// The pointers got should be in this order
	EXPECT_EQ(component[0], component1);
	EXPECT_EQ(component[1], controllerComponent1);
	EXPECT_EQ(component[2], component2);
	EXPECT_EQ(component[3], controllerComponent2);
}

// Test if the removeComponent() function works
TEST(Gameobject, RemoveComponent)
{
	// Iterate through the components
	int i = 0;
	for (auto it = object.begin(); it != object.end(); it++)
	{
		if (i == 1)
		{
			// Remove the 2nd component
			object.removeComponent(it);
			break;
		}
		i++;
	}

	std::shared_ptr<Engine::Component> component[3]; // To get component pointers from the object
	i = 0;
	for (auto it = object.begin(); it != object.end(); it++)
	{
		component[i] = *it;
		i++;
	}
	// The pointers got should be in this order (without the one removed)
	EXPECT_EQ(component[0], component1);
	EXPECT_EQ(component[1], component2);
	EXPECT_EQ(component[2], controllerComponent2);
}