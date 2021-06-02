#pragma once
#include <gtest/gtest.h>
// TESTING FOR EVENTS
#include "events/event.h"
#include "events/windowEvents.h"
#include "events/mouseEvents.h"
#include "events/keyEvents.h"

// Parameters for window resize
const int width = 1024; 
const int height = 720;
// Events to test
Engine::WindowResizeEvent re(width, height);
Engine::WindowCloseEvent ce;

// Functions to test event dispatching
bool onResizeTrue(Engine::WindowResizeEvent& e)
{
	return true;
}

bool onResizeFalse(Engine::WindowResizeEvent& e)
{
	return false;
}

// Parameters for mouse button press
const int button1 = 0;
const int button2 = 1;
// Parameters for mouse move
const float posX = 400;
const float posY = 200;
// Events to test
Engine::MouseButtonPressedEvent mbpe(button1);
Engine::MouseButtonReleasedEvent mbre(button2);
Engine::MouseMovedEvent mme(posX, posY);

// Parameters for key press
const int keyCode = 68;
// Events to test
Engine::KeyPressedEvent kpe(keyCode, 0);
Engine::KeyPressedEvent krpe(keyCode, 1);