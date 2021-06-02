#include "eventTests.h"

TEST(Events, ResizeConstructor)
{
	// Get window width and height from the event
	int gotWidth = re.getWidth();
	int gotHeight = re.getHeight();

	// The width and height should be equal to the ones used in the constructor
	EXPECT_EQ(gotWidth, width);
	EXPECT_EQ(gotHeight, height);
}

TEST(Events, DispatchTrue)
{
	// Make dispatcher object for a window resize event
	Engine::EventDispatcher dispatcher(re);
	// Dispatch the window resize event, should return true and call onResizeTrue which will return true
	bool result = dispatcher.dispatch<Engine::WindowResizeEvent>(std::bind(onResizeTrue, std::placeholders::_1));
	EXPECT_TRUE(result);
}

TEST(Events, DispatchFalse)
{
	// Make dispatcher object for a window close event
	Engine::EventDispatcher dispatcher(ce);
	// Dispatch the window close event, should return false and do nothing
	bool result = dispatcher.dispatch<Engine::WindowResizeEvent>(std::bind(onResizeTrue, std::placeholders::_1));
	EXPECT_FALSE(result);
}

TEST(Events, DispatchHandleTrue)
{
	// Make dispatcher object for a window resize event
	Engine::EventDispatcher dispatcher(re);
	// Dispatch the window resize event, should return true and call onResizeTrue which will return true
	dispatcher.dispatch<Engine::WindowResizeEvent>(std::bind(onResizeTrue, std::placeholders::_1));
	bool result = re.handled(); // Check if the event has been handled
	EXPECT_TRUE(result); // It should have been handled
}

TEST(Events, DispatchHandleFalse)
{
	// Make dispatcher object for a window resize event
	Engine::EventDispatcher dispatcher(re);
	// Dispatch the window resize event, should return true and call onResizeFalse which will return false
	dispatcher.dispatch<Engine::WindowResizeEvent>(std::bind(onResizeFalse, std::placeholders::_1));
	bool result = re.handled(); // Check if the event has been handled
	EXPECT_FALSE(result); // It should not have been handled
}

TEST(Events, MousePressConstructor)
{
	// Get button codes for the button pressed and button released events
	int gotButtonPressed = mbpe.getMouseButton();
	int gotButtonReleased = mbre.getMouseButton();

	// They should be equal to the ones used in the constructor
	EXPECT_EQ(gotButtonPressed, button1);
	EXPECT_EQ(gotButtonReleased, button2);
}

TEST(Events, MouseMoveConstructor)
{
	// Get x and y positions from the event
	float gotX = mme.getX();
	float gotY = mme.getY();

	// They should be equal to the ones used in the constructor
	EXPECT_EQ(gotX, posX);
	EXPECT_EQ(gotY, posY);
}

TEST(Events, EventCategory)
{
	// Get event category flags from each event
	int catMoved = mme.getCategoryFlags();
	int catButtonPress = mbpe.getCategoryFlags();
	int catButtonReleased = mbre.getCategoryFlags();

	// Mouse moved and mouse button pressed should have different flags 
	// (button pressed has EventCategoryMouseButton as well as the ones that moved has)
	EXPECT_NE(catMoved, catButtonPress);
	// Mouse button pressed and mouse button released should have the same category flags
	EXPECT_EQ(catButtonPress, catButtonReleased);
}

TEST(Events, KeyPressConstructor)
{
	// Get key code and if repeated from the events
	int gotKey = kpe.getKeyCode();
	bool repeat1 = kpe.getRepeated();
	bool repeat2 = krpe.getRepeated();

	// The key code should be equal to the one in the constructor
	EXPECT_EQ(gotKey, keyCode);
	EXPECT_FALSE(repeat1); // kpe had 0 for the repeat count in the constructor so it is not a repeat key press
	EXPECT_TRUE(repeat2); // krpe had 1 for the repeat count in the constructor so it is a repeat key press
}

TEST(Events, KeyEventTypes)
{
	// Get event type for the two events constructed and a key event not constructed
	Engine::EventType gotPressedType = kpe.getEventType();
	Engine::EventType gotRepeatType = krpe.getEventType();
	Engine::EventType gotTypedType = Engine::KeyTypedEvent::getStaticType(); // getStaticType as the event has not been constructed

	EXPECT_EQ(gotPressedType, gotRepeatType); // Pressed and repeat should have the same type
	EXPECT_NE(gotPressedType, gotTypedType); // Pressed and typed should have different types
}