/** \file core.h
*/
#pragma once

#include "systems/log.h"

#ifdef ENGINE_ENABLE_ASSERTS
	#define ENGINE_ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define ENGINE_ASSERT(x, ...) 
#endif // ENGINE_ENABLE_ASSERTS
