/** \file changeObjects.h
*/
#pragma once

#include "layers/displayLayer.h"
//#include <Windows.h>
#include "shellapi.h"

/**
\class ChangeObjects
\brief Class with static functions that change objects when the user does things
*/
class ChangeObjects
{
private:
	static std::shared_ptr<DisplayLayer> s_displayLayer; //!< A pointer to the display layer
	static std::string s_sProjectName; //!< The name of the project
	static bool bShowLuaHelpWindow;
public:
	//! Function to set the pointer to the display layer
	/*!
	\param displayLayer The pointer to set it to
	*/
	static void setDisplayLayer(std::shared_ptr<DisplayLayer> displayLayer) { s_displayLayer = displayLayer; }
	//! Function to set the project name
	/*!
	\param newName The name of the project
	*/
	static void setProjectName(std::string newName) { s_sProjectName = newName; }

	//! Make a New Game Object
	/*!
	\param name The name of the new GameObject
	\param object A pointer to the new GameObject
	\param UI If the object is 2D
	*/
	static void makeGameObject(const std::string& name, std::shared_ptr<Engine::GameObject>& object, bool UI);

	//! Add a Position Component
	/*!
	\param object Pointer to the GameObject that's having a position component added
	\param UI If the object is a UI object
	*/
	static void addPosition(std::shared_ptr<Engine::GameObject>& object, bool UI);

	//! Add a mesh component
	/*!
	\param modelFilepath The filepath of the model to give the object
	\param object A reference to the object
	*/
	static void addMesh(const std::string& modelFilepath, std::shared_ptr<Engine::GameObject>& object);
	// Add a Material Component
	/*!
	\param name The name of the object
	\param object A pointer to the the object having a material component added
	\param UI If the object is a UI object
	*/
	static void addMaterial(const std::string& name, std::shared_ptr<Engine::GameObject>& object, bool UI);
	//! Add a Collider Component
	/*!
	\param object A Pointer to the Object Having the Component Attached
	*/
	static void addCollider(Object& object);
	//! Add a RidgidBody Component
	/*!
	\param object A Pointer to the Object Having the Component Attached
	*/
	static void addRigidbody(Object& object);
	//! Add a texture component to an object
	/*!
	\param object A reference to the object
	\param UI If the object is a UI object
	*/
	static void addTexture(std::shared_ptr<Engine::GameObject>& object, bool UI);

	//! Add a Lua Script Component
	/*!
	\param object A Pointer to the Object Having the Component Attached
	*/
	static void AddLuaScriptComponent(Object& object);
	//! Function to make a joint
	/*!
	\param type The type of joint to make
	*/
	static void makeJoint(const std::string& type);

	//! Remove a Lua Script Component
	/*!
	\param object A Pointer to the Object Having the Component Removed
	*/
	static void RemoveLuaScriptComponent(Object& object);
	//! Function to change an objects name
	/*!
	\param i The vector value of the object
	\param UI If the object is a UI object
	*/
	static void changeName(int i, bool UI);
	//! Function to change the layer an object is in
	/*!
	\param i The vector value of the object
	\param isObject If it is an object or a joint
	\param UI If the object is a UI object
	*/
	static void changeLayer(int i, bool isObject, bool UI);
	//! Function to change the tag on an object
	/*!
	\param i The vector value of the object
	\param UI If the object is a UI object
	*/
	static void changeTag(int i, bool UI);

	//! Change Object Position GUI
	/*!
	\param i The index of the object in the m_objects vector
	\param UI If the object is a UI object
	*/
	static void changePosition(int i, bool UI);
	//! Chnage Object Rotation GUI
	/*!
	\param i The index of the object in the m_objects vector
	\param UI If the object is a UI object
	*/
	static void changeRotation(int i, bool UI);
	//! Change Object Scale GUI
	/*!
	\param i The index of the object in the m_objects vector
	\param UI If the object is a UI object
	*/
	static void changeScale(int i, bool UI);
	//! Lua Script Component GUI
	/*!
	\param i The index of the object in the m_objects vector
	\param UI If the object is a UI object
	*/
	static void LuaScriptComponentGUI(int i, bool UI);
	//! Change a mesh of an object
	/*!
	\param i The index of the object in the m_objects vector
	*/
	static void changeMesh(int i);

	//! Change Object Material GUI
	/*!
	\param i The index of the object in the m_objects vector
	*/
	static void changeMaterial(int i);
	//! Change textures
	/*!
	\param i The index of the object in the m_objects vector
	*/
	static void changeTextures(int i);

	//! Change Light
	/*!
	\param i The index of the object in the m_objects vector
	*/
	static void changeLight(int i);
	//! Change camera
	/*!
	\param i The index of the object in the m_objects vector
	*/
	static void changeCamera(int i);
	//! Change collider
	/*!
	\param i The index of the object in the m_objects vector
	*/
	static void changeCollider(int i);
	//! Change rigidbody
	/*!
	\param i The index of the object in the m_objects vector
	*/
	static void changeRigidbody(int i);
	//! Change joint
	/*!
	\param i The index of the object in the m_objects vector
	*/
	static void changeJoint(int i);
	//! Change text component
	/*!
	\param i The index of the object in the m_2DObjects vector
	*/
	static void changeText(int i);

	//! Function to get the name from a filepath
	/*!
	\param str The filepath
	\return The name
	*/
	static std::string parseFilePath(const std::string& str);
};