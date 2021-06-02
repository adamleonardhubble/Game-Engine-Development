#pragma once
#include <gtest/gtest.h>
//TESTS FOR GAMEOBJECTS AND COMPONENTS
#include "components/CGO.h"

#include "components/controllerComponent.h"

// Game object class to test
Engine::GameObject object;

// Class to inherit from Component abstract class to provide implementation for pure virtual functions
class ComponentTest : public Engine::Component
{
public:
	virtual void receiveMessage(const Engine::ComponentMessage& msg) {}
};

// Class to inherit from ControllerComponent abstract class to provide implementation for pure virtual functions
class ControllerComponentTest : public Engine::ControllerComponent
{
public:
	virtual void receiveMessage(const Engine::ComponentMessage& msg) {}
};

// Components to use in the tests
std::shared_ptr<Engine::Component> component1;
std::shared_ptr<Engine::Component> component2;
std::shared_ptr<Engine::Component> controllerComponent1;
std::shared_ptr<Engine::Component> controllerComponent2;