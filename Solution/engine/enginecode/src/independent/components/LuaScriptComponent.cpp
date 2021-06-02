// Include Headers
#include "engine_pch.h"
#include "components/LuaScriptComponent.h"

#include "systems/dynamicPhysicsSystem.h"

// Engine Namespace
namespace Engine
{
	// Intialize Static Variables
	//bool LuaScriptComponent::bTransformUpdated = false;
	//LuaScriptComponent<DynamicPhysicsSystem>* LuaScriptComponent<DynamicPhysicsSystem>::me = nullptr;
	LuaScriptComponent<DynamicPhysicsSystem>* LuaScriptComponent<DynamicPhysicsSystem>::ActiveScript = nullptr;
	int LuaScriptComponent<DynamicPhysicsSystem>::KeyCodesArray[65];
}