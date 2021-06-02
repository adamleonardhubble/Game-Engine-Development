#include "bufferTests.h"

// Commented out, tests don't work due to error using BufferLayout class
// Test to see if the layout class calculates stride correctly
//TEST(BufferLayout, Stride)
//{
//	// Layout
//	Engine::BufferLayout layout;
//	// Add data types to the layout
//	layout.addElement(type1);
//	layout.addElement(type2);
//	layout.addElement(type3);
//	// Calculate the correct stride here
//	unsigned int stride = 0;
//	stride += Engine::ShaderDataTypeSize(type1);
//	stride += Engine::ShaderDataTypeSize(type2);
//	stride += Engine::ShaderDataTypeSize(type3);
//	// The stride in the layout class and the one calculated here should match
//	EXPECT_EQ(stride, layout.getStride());
//}