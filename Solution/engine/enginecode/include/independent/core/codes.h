/** \file codes.h
\brief Decides which key code file to use based on what operating system is being used
*/
#pragma once

// If the engine is running on a windows computer
#ifdef NG_PLATFORM_WINDOWS
#include "platform/GLFW/GLFWCodes.h" // Include the GLFW codes file
#endif // NG_PLATFORM_WINDOWS