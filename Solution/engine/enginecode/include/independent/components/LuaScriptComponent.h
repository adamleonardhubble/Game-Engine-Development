/** \file LuaScriptComponent.h
*/

// Compile Once
#pragma once

// Include Components
#include "components/CGO.h"
#include "core/codes.h"
#include "windows/inputPoller.h"
//#include "components/positionComponent.h"
#include "components/rigidBodyComponent.h"
#include "components/lightComponent.h"
#include "components/materialComponent.h"
#include "components/meshComponent.h"
#include "components/textureComponent.h"
#include "components/collisionComponent.h"
#include "systems/dynamicPhysicsSystem.h"
#include "load/JSONLayer.h"

// Include GLM
#include <glm/glm.hpp>

// Include Lua Stuff
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include <LuaBridge/LuaBridge.h>

//#include <iostream>
namespace Engine
{
	/**
	\class Lua Script Component
	\brief Class for the Lua Script Components
	*/
	template <class G>
	class LuaScriptComponent : public Component
	{
		// Private Members
	private:
		// Private Attributes
		std::string sScriptFilePath = "!NoLuaScriptAssigned!"; //!< Filepath to the Associated Lua Script
		std::string sScriptName = "!NoLuaScriptAssigned!"; //!< Name of the Associated Script File
		bool bLuaComponentActive = false; // Holds wether the Lua component is active
		float deltaTime = 0.0f; //!< Float Containing the Timestep
		int status; //!< Status Integer. Holds the Status of Loading the Script
		lua_State* L; //!< The Lua State
		bool bLuaOpen; //!< Boolean that holds if the Lua State is open
		enum eScriptFunctions {eOnAwake, e_Start, e_Update, e_OnCollision, SCRIPTFUNCTIONS_LENGTH}; //!< Enum Holding the Different Possible Functions the Level Editor can Call
		bool ScriptExists[SCRIPTFUNCTIONS_LENGTH]; //!< Boolean Array That Holds if Each Function Exists
		bool bTransformUpdated;//!< Boolean That Records if the Transform Has Been Updated
		static LuaScriptComponent* ActiveScript; // !< Pointer to the Script Componet of the Object Currently Being Referenced by This Lua Script Compnent

		// Static Input Variables
		enum KeyCodesIndex { a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, num_0, num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9, space, escape, enter, tab, backspace, insert, del, right, left, up, down, left_shift, left_cntrl, left_alt, right_shift, right_cntrl, right_alt, mouse_1, mouse_2, mouse_3, mouse_4, mouse_5, mouse_6, mouse_7, mouse_8, mouse_last, mouse_left, mouse_right, mouse_middle };
		static int KeyCodesArray[65];

		// Public Members
	public:
		// Public Attributes
		LuaScriptComponent* me; //!< Pointer to This Script Component

		// !< Definition of a Data Struct Containing the Objects Variables
		struct LuaScriptVariables {
			glm::vec3* vInputObjectPosition = nullptr; //!< Pointer to the Vector3 Containing the Object's Position
			glm::vec3* vInputObjectRotation = nullptr; //!< Pointer to the Vector3 Containing the Object's Rotation
			glm::vec3* vInputObjectScale = nullptr; //!< Pointer to the Vector3 Containing the Object's Scale
			JSONLayer* MyJSONLayer = nullptr; //!< Pointer to the JSON Layer the Object is in
			std::shared_ptr<RigidBodyComponent> MyRigidBodyComponent = nullptr; //!< Pointer to The Ridgid Body Component
			std::shared_ptr<LightComponent> MyLightComponent = nullptr; //!< Pointer to the Light Component
			std::shared_ptr<MaterialComponent> MyMaterialComponent = nullptr; //!< Pointer to the Material Component
			std::shared_ptr<MeshComponent> MyMeshComponent = nullptr; //!< Pointer to the Mesh Component
			std::shared_ptr<TextureComponent> MyTextureComponent = nullptr; //!< Pointer to the Texture Component
		};

		//!< The Component's Copy of the Objetc's Variables
		LuaScriptVariables ObjectVariables;

		// Data Structs Used by Lua as Objects
		// !< LuaVector Data Struct, Used as a Go-Between for Vectors in C++ and Lua
		struct LuaVectorStruct {
			// Properties
			float fVectorX = 0.0f; // The X Component of the Vector
			float fVectorY = 0.0f; // The Y Component of the Vector
			float fVectorZ = 0.0f; // The Z Component of the Vector

			//static void SetVectorX(float fNewX) { fVectorX = fNewX; } static float GetVectorX() { return fVectorX; }

			// Behaviour
			// Set the Vector's Values
			void SetVector(float fNewX, float fNewY, float fNewZ)
			{
				// Set New Vector
				fVectorX = fNewX; // Set X
				fVectorY = fNewY; // Set Y
				fVectorZ = fNewZ; // Set Z
			}
			// Get the Vector's Magnitude
			float GetMagnitude()
			{
				return glm::sqrt((fVectorX * fVectorX) + (fVectorY * fVectorY) + (fVectorZ * fVectorZ));
			}
			// Normalize the Vector
			void NormalizeVector()
			{
				// Get the Magnitude
				float fMagnitude = GetMagnitude();

				// Normalize Vector Elements
				fVectorX /= fMagnitude;
				fVectorY /= fMagnitude;
				fVectorZ /= fMagnitude;
			}
			// Scale by
			void ScaleVectorBy(float fScaleFactor)
			{
				// Scale Vector
				fVectorX *= fScaleFactor;
				fVectorY *= fScaleFactor;
				fVectorZ *= fScaleFactor;
			}
		};
		// Function to Create a LuaVector From Floats
		static LuaVectorStruct CreateLuaVector(float x, float y, float z)
		{
			// Define Return Vector
			LuaVectorStruct ReturnVector;

			// Set Vector
			ReturnVector.SetVector(x, y, z);

			// Return Vector
			return ReturnVector;
		}
		
		// !< Colour Data Struct, Used as a Go-Between for Colours in C++ and Lua
		struct LuaColourStruct {
			// Properties
			float fVectorR = 0.0f; // The R Component of the Vector
			float fVectorG = 0.0f; // The G Component of the Vector
			float fVectorB = 0.0f; // The B Component of the Vector

			// Behaviour
			void SetColour(float fNewR, float fNewG, float fNewB) // Set the Vector's Values
			{
				// Set New Vector
				fVectorR = fNewR; // Set R
				fVectorG = fNewG; // Set G
				fVectorB = fNewB; // Set G
			}
			void SetColourFromHex(std::string sInputHexCode)
			{
				// Error Check
				if (sInputHexCode.length() == 6 || sInputHexCode.length() == 7) // If String of Suitable Length
				{
					// Remove #?
					if (sInputHexCode.at(0) == '#') // If There is a #
						sInputHexCode = sInputHexCode.substr(1, sInputHexCode.length() - 1); // Remove # From String

					// Capitalise String
					std::locale loc;
					std::string CapHexCode = "";
					for (std::string::size_type i = 0; i < sInputHexCode.length(); ++i)
						CapHexCode += std::toupper(sInputHexCode[i], loc);

					// Define Output
					int iOutput;

					// Get R From Hex
					std::stringstream StringStreamR;
					StringStreamR << std::hex << CapHexCode.substr(0, 2);
					StringStreamR >> iOutput;
					fVectorR = (float)iOutput / 255;

					// Get G From Hex
					std::stringstream StringStreamG;
					StringStreamG << std::hex << CapHexCode.substr(2, 2);
					StringStreamG >> iOutput;
					fVectorG = (float)iOutput / 255;

					// Get B From Hex
					std::stringstream StringStreamB;
					StringStreamB << std::hex << CapHexCode.substr(4, 2);
					StringStreamB >> iOutput;
					fVectorB = (float)iOutput / 255;
				}
				else
					LOG_ERROR("Error seting colour from hex. Incorrect input string.");
			}
		};
		
		// !< ObjectPrefab Data Struct, Used to Control Object Prefabs !All Values Can Be Changed, Current Values Are Default!
		struct ObjectPrefab {
			// Prefab Attributes
			// General
			std::string sObjectName = "New Object"; // Name of the Object
			std::string sObjectTag = ""; // The Object's Tag

			// Transform
			LuaVectorStruct lvPosition; // Define Position Vector
			LuaVectorStruct lvRotation; // Define Rotation Vector
			LuaVectorStruct lvScale = CreateLuaVector(1.0f, 1.0f, 1.0f); // Define Scale Vector

			// Model
			std::string sModelFilepath = ""; // Filepath to the Object's Model
			std::string sShaderFilepath = "assets/shaders/lighting.glsl"; // Filepath to the Object's Shader

			// Material/Texture
			LuaColourStruct lcMaterialDiffuse; // Define Material Diffuse Vector
			LuaColourStruct lcMaterialSpecular; // Define Material Specular Vector
			float fMaterialShininess = 0.0f; // Define Material Shininess
			std::string sTextureFilepath = "assets/textures/None.png"; // Filepath to the Object's Texture

			// RigidBody Component?
			bool bRigidBodyComponentExists = false;
			std::string sRigidBodyType;
			float fRigidBodyBounciness;
			float fRigidBodyFriction;
			float fRigidBodyRollingResistance;
			bool bUsesGravity;

			// Collider Component?
			bool bColliderComponentExists = false;
			float fColliderMass;
			CollisionCategory ColliderCollisionCategory;
			unsigned short ColliderCanCollideWith;
			std::string sColliderType;
			glm::vec3 BoxColliderHalfExtents;
			float fColliderRadius;
			float fColliderHeight;
			std::string sColliderConvexMeshFilepath;

			// Lua Script Component?
			bool bLuaScriptComponentExists = false;
			std::string sLuaScriptComponentFilePath = ""; //!< Filepath to the Associated Lua Script

			// Behaviour
			// Define a Component
			// Define a Dynamic RidgidBodyComponent
			void DefineDynamicRigidBodyComponent(float fInputBodyBounciness, float fInputBodyFriction, float fInputBodyRolRes, bool bInputUsesGravity)
			{
				// Mark Prefab as Having a RidgidBody Component
				bRigidBodyComponentExists = true;

				// Set Body Values
				sRigidBodyType = "dynamic";
				fRigidBodyBounciness = fInputBodyBounciness;
				fRigidBodyFriction = fInputBodyFriction;
				fRigidBodyRollingResistance = fInputBodyRolRes;
				bUsesGravity = bInputUsesGravity;
			}
			// Define a Static RidgidBodyComponent
			void DefineStaticRigidBodyComponent(float fInputBodyBounciness, float fInputBodyFriction, float fInputBodyRolRes)
			{
				// Mark Prefab as Having a RidgidBody Component
				bRigidBodyComponentExists = true;

				// Set Body Values
				sRigidBodyType = "static";
				fRigidBodyBounciness = fInputBodyBounciness;
				fRigidBodyFriction = fInputBodyFriction;
				fRigidBodyRollingResistance = fInputBodyRolRes;
				bUsesGravity = false;
			}
			// Define a Kinematic RidgidBodyComponent
			void DefineKinematicRigidBodyComponent(float fInputBodyBounciness, float fInputBodyFriction, float fInputBodyRolRes)
			{
				// Mark Prefab as Having a RidgidBody Component
				bRigidBodyComponentExists = true;

				// Set Body Values
				sRigidBodyType = "kinematic";
				fRigidBodyBounciness = fInputBodyBounciness;
				fRigidBodyFriction = fInputBodyFriction;
				fRigidBodyRollingResistance = fInputBodyRolRes;
				bUsesGravity = false;
			}

			// Define a Box Collider
			void DefineBoxColliderComponent(LuaVectorStruct lvBoxHalfExtents, float fInputColliderMass, int iCollisionCategory)
			{
				// Mark Prefab as Having a Collider Component
				bColliderComponentExists = true;
				
				// Set Collider Values
				sColliderType = "box";
				BoxColliderHalfExtents = glm::vec3(lvBoxHalfExtents.fVectorX, lvBoxHalfExtents.fVectorY, lvBoxHalfExtents.fVectorZ);
				fColliderMass = fInputColliderMass;

				// Set Collision Category
				ColliderCollisionCategory = (CollisionCategory)iCollisionCategory;
			}
			// Define a Sphere Collider
			void DefineSphereColliderComponent(float fSphereRadius, int iCollisionCategory)
			{
				// Mark Prefab as Having a Collider Component
				bColliderComponentExists = true;

				// Set Collider Values
				sColliderType = "sphere";
				fColliderRadius = fSphereRadius;

				// Set Collision Category
				ColliderCollisionCategory = (CollisionCategory)iCollisionCategory;
			}
			// Define a Capusle Collider
			void DefineCapusleColliderComponent(float fCapRadius, float fCapHeight, int iCollisionCategory)
			{
				// Mark Prefab as Having a Collider Component
				bColliderComponentExists = true;

				// Set Collider Values
				sColliderType = "capsule";
				fColliderRadius = fCapRadius;
				fColliderHeight = fCapHeight;

				// Set Collision Category
				ColliderCollisionCategory = (CollisionCategory)iCollisionCategory;
			}
			// Define a Convex Mesh Collider
			void DefineConvexMeshColliderComponent(std::string sFilepathToMesh, int iCollisionCategory)
			{
				// Mark Prefab as Having a Collider Component
				bColliderComponentExists = true;

				// Set Collider Values
				sColliderType = "convex mesh";
				sColliderConvexMeshFilepath = sFilepathToMesh;

				// Set Collision Category
				ColliderCollisionCategory = (CollisionCategory)iCollisionCategory;
			}
			// Set Collider Collidable Objects
			void DefineColliderCollidables(bool bCanCollideWithOne, bool bCanCollideWithTwo, bool bCanCollideWithThree, bool bCanCollideWithFour)
			{
				// Reset Collider Collidables
				ColliderCanCollideWith = 0;

				// Collide With 1?
				if(bCanCollideWithOne)
					ColliderCanCollideWith |= (unsigned short)1;

				// Collide With 2?
				if (bCanCollideWithTwo)
					ColliderCanCollideWith |= (unsigned short)2;

				// Collide With 3?
				if (bCanCollideWithThree)
					ColliderCanCollideWith |= (unsigned short)4;

				// Collide With 4?
				if (bCanCollideWithFour)
					ColliderCanCollideWith |= (unsigned short)8;

				//LOG_INFO(ColliderCanCollideWith);
			}

			// Define a Lua Script Component
			void DefineLuaScriptComponent(std::string sFilepathToScript)
			{
				// Mark Prefab as Having a Lua Script Component
				bLuaScriptComponentExists = true;

				// Set Script Filepath
				sLuaScriptComponentFilePath = sFilepathToScript;
			}
		};

		// Public Behaviour
		//! Component Constructors
		// Blank Constructor
		LuaScriptComponent()
		{
			// Mark Lua as Not Open
			bLuaOpen = false;

			// Mark Transform as Not Updated
			bTransformUpdated = false;

			// Get me
			me = this;

			// Assign KeyCodes?
			if (KeyCodesArray[KeyCodesIndex::a] != ENGINE_KEY_A)
				AssignKeyCodes();

			// Set Object Variables
			ObjectVariables = LuaScriptVariables();

			// Set Component Type
			m_componentType = ComponentType::LuaScript;

			// Mark Lua Component as NOT Active
			bLuaComponentActive = false;
		}
		// Constructor with Script Filepath and Associated Component Parameters
		LuaScriptComponent(std::string sInputScriptFilepath, LuaScriptVariables InputParameters)
		{
			// Mark Lua as Not Open
			bLuaOpen = false;

			// Mark Transform as Not Updated
			bTransformUpdated = false;

			// Get me
			me = this;

			// Assign KeyCodes?
			if (KeyCodesArray[KeyCodesIndex::a] != ENGINE_KEY_A)
				AssignKeyCodes();

			// Get Input Parameters
			ObjectVariables = InputParameters;

			// Set Component Type
			m_componentType = ComponentType::LuaScript;

			// Assign Script File Path
			sScriptFilePath = sInputScriptFilepath;

			// Get Script Name
			std::size_t EndOfFolderPath = sScriptFilePath.find_last_of("/\\");
			std::size_t NameLength = sScriptFilePath.find_last_of(".") - EndOfFolderPath - 1;
			sScriptName = sScriptFilePath.substr(EndOfFolderPath + 1, NameLength);

			// Mark Lua Component as Active
			bLuaComponentActive = true;
		}
		//! Destructor
		~LuaScriptComponent()
		{
			// Close Lua
			if(bLuaOpen)
				lua_close(L);

			// Delete JSON Layer Pointer

		}
		//! Function called when the component is attached to a gameobject
		/*!
		\param owner Pointer to the gameobject the component is being attached to
		*/
		void onAttach(GameObject* owner) override
		{
			// Set Pointer to the Game Object That Has This Component
			m_pOwner = owner;

			// Get me and set ActiveScript to me
			me = this;
			ActiveScript = me;

			// Assign Object Prefab Filepaths
			/*pCubePrefab.sModelFilepath = sPremadeCubeModelFilepath;
			pCapsulePrefab.sModelFilepath = sPremadeCapsuleModelFilepath;
			pCylinderPrefab.sModelFilepath = sPremadeCylinderModelFilepath;
			pQuadPrefab.sModelFilepath = sPremadeQuadModelFilepath;
			pSpherePrefab.sModelFilepath = sPremadeSphereModelFilepath;*/

			// Initialize Lua
			bLuaOpen = true; // Mark Lua as Open
			L = luaL_newstate(); // Create New Stack
			luaL_openlibs(L); // Load Lua Libraries
			AddObjectData(); // Add C++ Variables and Functions

			// If Lua Component is Active Check for Functions, Add Col Data Class and Run Start
			if (bLuaComponentActive)
			{
				// Check if Script Contains Functions
				ScriptExists[eScriptFunctions::eOnAwake] = ScriptContainsFunction("Awake");
				ScriptExists[eScriptFunctions::e_Start] = ScriptContainsFunction("Start");
				ScriptExists[eScriptFunctions::e_Update] = ScriptContainsFunction("Update");
				ScriptExists[eScriptFunctions::e_OnCollision] = ScriptContainsFunction("OnCollision");

				// If Collision Function Exists Create Collision Data Class
				if (ScriptExists[eScriptFunctions::e_OnCollision])
				{
					// Add Collision Info Class to Lua
					luabridge::getGlobalNamespace(L)

						// Create CollisionDataClass
						.beginClass <CollisionInfo>("CollisionDataClass")
						.addProperty("MyName", &CollisionInfo::thisObjectName)
						.addProperty("MyTag", &CollisionInfo::thisObjectTag)
						.addProperty("OtherName", &CollisionInfo::otherObjectName)
						.addProperty("OtherTag", &CollisionInfo::otherObjectTag)
						.endClass();
				}

				// Run Awake Function?
				if (ScriptExists[eScriptFunctions::eOnAwake])
				{
					// Run Awake Function
					RunAwakeFunction();

					// Update Transform?
					if (bTransformUpdated)
						TransformUpdated();
				}
			}
		}
		void onGameStart()
		{
			// If Lua Component Active
			if (bLuaComponentActive)
			{
				// Run Start Function?
				if (ScriptExists[eScriptFunctions::e_Start])
				{
					// Run Start Function
					RunStartFunction();

					// Update Transform?
					if (bTransformUpdated)
						TransformUpdated();
				}
			}
		}
		//! Send Message Function, Sends Messages to Other Components
		void sendMessage(const ComponentMessage& msg) override
		{
			// Send the message to all components on the object
			for (auto it = m_pOwner->begin(); it != m_pOwner->end(); it++)
			{
				(*it)->receiveMessage(msg);
			}
		}
		//! Function to update the component every frame
		/*!
		\param timestep The time since the previous frame
		*/
		void onUpdate(float timestep) override 
		{
			// If this Component is Active
			if (bLuaComponentActive)
			{
				// Mark Transform as Not Updated
				bTransformUpdated = false;

				// Get me and set ActiveScript to me
				//me = this;
				ActiveScript = me;

				// Get Delta Time
				deltaTime = timestep;

				// Run Update Function?
				if (ScriptExists[eScriptFunctions::e_Update])
				{
					// Run Update Function
					RunUpdateFunction();

					// Update Transform?
					if (bTransformUpdated)
						TransformUpdated();
				}
			}
			else // If Lua Script Component Not Active, There Won't be an Update Function to Run and Check Must Happen b4 Reset
			{
				// Update Transform?
				if (bTransformUpdated)
					TransformUpdated();

				// Mark Transform as Not Updated
				bTransformUpdated = false;
			}
		}
		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override
		{
			// Create message dispatcher
			MessageDispatcher dispatcher(msg);
			// If the message type matches, call the corresponding function
			dispatcher.dispatch(ComponentMessageType::CollisionDetected, std::bind(&LuaScriptComponent::OnCollision, this, std::placeholders::_1));
		}

		//! Function to run Attached Lua Script Awake Function
		void RunAwakeFunction()
		{
			// Attempt to Load Lua Script
			status = luaL_dofile(L, sScriptFilePath.c_str()); // Load Lua Script and Save Status
			if (status) // If Loading Script Failed
				LOG_ERROR("Failed to load " + sScriptName + " Lua Script. Not a valid Lua File."); // Log the Error
			else // If Script Loaded
			{
				// Get the Function to be Run
				status = lua_getglobal(L, "Awake");

				// Error Handle IF the Function Does Not Exist or Is Not on the Stack
				if (!lua_isfunction(L, -1)) // If Function Not Found
					LOG_ERROR("Failed to execute " + sScriptName + " Lua Script Awake Function. Function does not exist."); // Log the Error
				else // If Function Found
				{
					// Call the Function in Protected Mode
					if (lua_pcall(L, 0, 0, 0) == 0) // Output Wether Function Executed Succesfully
					{
						//LOG_INFO(sScriptName + " Awake Function Run"); // Log Awake Function Run
					}
					else // If Error Running Function
					{
						// Get Error Message
						std::string sErrorMessage = lua_tostring(L, -1);
						sErrorMessage = sErrorMessage.substr(sScriptFilePath.length() - 2, sErrorMessage.length());

						// Output Relevant Error Message
						LOG_ERROR("Error Running " + sScriptName + " Lua Script Awake Function. " + sErrorMessage);
						//lua_pop(L, 1);
					}
				}
			}
		}
		//! Function to run Attached Lua Script Start Function
		void RunStartFunction()
		{
			// Attempt to Load Lua Script
			status = luaL_dofile(L, sScriptFilePath.c_str()); // Load Lua Script and Save Status
			if (status) // If Loading Script Failed
				LOG_ERROR("Failed to load " + sScriptName + " Lua Script. Not a valid Lua File."); // Log the Error
			else // If Script Loaded
			{
				// Get the Function to be Run
				status = lua_getglobal(L, "Start");

				// Error Handle IF the Function Does Not Exist or Is Not on the Stack
				if (!lua_isfunction(L, -1)) // If Function Not Found
					LOG_ERROR("Failed to execute " + sScriptName + " Lua Script Start Function. Function does not exist."); // Log the Error
				else // If Function Found
				{
					// Call the Function in Protected Mode
					if (lua_pcall(L, 0, 0, 0) == 0) // Output Wether Function Executed Succesfully
					{
						//LOG_INFO(sScriptName + " Start Function Run"); // Log Start Function Run
					}
					else // If Error Running Function
					{
						// Get Error Message
						std::string sErrorMessage = lua_tostring(L, -1);
						sErrorMessage = sErrorMessage.substr(sScriptFilePath.length() - 2, sErrorMessage.length());

						// Output Relevant Error Message
						LOG_ERROR("Error Running " + sScriptName + " Lua Script Start Function. " + sErrorMessage);
						//lua_pop(L, 1);
					}
				}
			}
		}
		//! Function to run Attached Lua Script Update Function
		void RunUpdateFunction()
		{	
			// Attempt to Load Lua Script
			status = luaL_dofile(L, sScriptFilePath.c_str()); // Load Lua Script and Save Status
			if (status) // If Loading Script Failed
				LOG_ERROR("Failed to load " + sScriptName + " Lua Script. Not a valid Lua File."); // Log the Error
			else // If Script Loaded
			{
				// Get the Function to be Run
				status = lua_getglobal(L, "Update");

				// Error Handle IF the Function Does Not Exist or Is Not on the Stack
				if (!lua_isfunction(L, -1)) // If Function Not Found
					LOG_ERROR("Failed to execute " + sScriptName + " Lua Script Update Function. Function does not exist."); // Log the Error
				else // If Function Found
				{
					// Call the Function in Protected Mode
					if (lua_pcall(L, 0, 0, 0) == 0) // Output Wether Function Executed Succesfully
					{
						//LOG_INFO(sScriptName + " Update Function Run"); // Log Update Function Run
					}
					else // If Error Running Function
					{
						// Get Error Message
						std::string sErrorMessage = lua_tostring(L, -1);
						sErrorMessage = sErrorMessage.substr(sErrorMessage.find_first_of(':') + 1, sErrorMessage.length());

						// Output Relevant Error Message
						LOG_ERROR("Error Running " + sScriptName + " Lua Script Update Function. " + sErrorMessage);
						//lua_pop(L, 1);
					}
				}
			}
		}
		//! Function to run Attached Lua Script On Collision Function
		/*!
		\param data A Void pointer to the CollisionInfo struct that holds the information for this collision
		*/
		void OnCollision(void* data)
		{
			// Check if Script Active
			if (bLuaComponentActive)
			{
				// If there is a Collision Function
				if (ScriptExists[eScriptFunctions::e_OnCollision])
				{
					// Get me and set ActiveScript to me
					me = this;
					ActiveScript = me;

					// Execute On CollisionFunction
					status = luaL_dofile(L, sScriptFilePath.c_str()); // Load Lua Script and Save Status
					if (status) // If Loading Script Failed
						LOG_ERROR("Failed to load " + sScriptName + " Lua Script. Not a valid Lua File."); // Log the Error
					else // If Script Loaded
					{
						// Get the Function to be Run
						status = lua_getglobal(L, "OnCollision");

						// Error Handle IF the Function Does Not Exist or Is Not on the Stack
						if (!lua_isfunction(L, -1)) // If Function Not Found
							LOG_ERROR("Failed to execute " + sScriptName + " Lua Script OnCollision Function. Function does not exist."); // Log the Error
						else // If Function Found
						{
							// Push Collision Info
							CollisionInfo CollisionData = *(CollisionInfo*)data;
							luabridge::push(L, &CollisionData);
							lua_setglobal(L, "CollisionData");

							// Call the Function in Protected Mode
							if (lua_pcall(L, 0, 0, 0) == 0) // Output Wether Function Executed Succesfully
							{
								//LOG_INFO(sScriptName + " OnCollision Function Run"); // Log OnCollision Function Run
							}
							else // If Error Running Function
							{
								// Get Error Message
								std::string sErrorMessage = lua_tostring(L, -1);
								sErrorMessage = sErrorMessage.substr(sScriptFilePath.length() - 2, sErrorMessage.length());

								// Output Relevant Error Message
								LOG_ERROR("Error Running " + sScriptName + " Lua Script OnCollision Function. " + sErrorMessage);
								//lua_pop(L, 1);
							}
						}
					}
				}
			}
		}

		//! Function to Check if the Attached Lua Script Contains a Function With a Given Name
		bool ScriptContainsFunction(std::string sFunctionName)
		{
			// Check For On Collision Function
			status = luaL_dofile(L, sScriptFilePath.c_str()); // Load Lua Script and Save Status
			if (status) // If Loading Script Failed
				return false; // Return False to Avoid Searching Non-Existent or Broken Script For Function
			else // If Script Loaded
			{
				// Attempt to Get Function
				status = lua_getglobal(L, sFunctionName.c_str());

				// Check If Function Exists
				if (!lua_isfunction(L, -1)) // If Function Not Found
					return false; // Return False as Function Not Found
				else // If Function Found
					return true; // Retun True as Function Found
			}
		}

		//! Function to Update the Script's Filepath
		void SetScriptFilePath(std::string sInputScriptFilepath)
		{
			// Assign Script File Path
			sScriptFilePath = sInputScriptFilepath;

			// Get Script Name
			std::size_t EndOfFolderPath = sScriptFilePath.find_last_of("/\\");
			std::size_t NameLength = sScriptFilePath.find_last_of(".") - EndOfFolderPath - 1;
			sScriptName = sScriptFilePath.substr(EndOfFolderPath + 1, NameLength);
		}
		//! Function to Update the Script's Name
		void SetScriptName(std::string sInputScriptName)
		{
			// Set Script Name
			sScriptName = sInputScriptName;

			// Remove Current Script Name From Filepath
			std::size_t EndOfFolderPath = sScriptFilePath.find_last_of("/\\");
			sScriptFilePath = sScriptFilePath.substr(0, EndOfFolderPath) + sScriptName + ".lua";
		}

		// Lua Vector Functions
#pragma region RestOfLuaVectorFunctions
		// Function to Add Two Vectors
		static LuaVectorStruct LuaVectorAddition(LuaVectorStruct lvVectorOne, LuaVectorStruct lvVectorTwo)
		{
			// Define Return Vector
			LuaVectorStruct lvReturnVector;

			// Set Vector
			lvReturnVector.fVectorX = lvVectorOne.fVectorX + lvVectorTwo.fVectorX;
			lvReturnVector.fVectorY = lvVectorOne.fVectorY + lvVectorTwo.fVectorY;
			lvReturnVector.fVectorZ = lvVectorOne.fVectorZ + lvVectorTwo.fVectorZ;

			// Return Vector
			return lvReturnVector;
		}
		// Function to Add Two Vectors
		static LuaVectorStruct LuaVectorSubtraction(LuaVectorStruct lvVectorOne, LuaVectorStruct lvVectorTwo)
		{
			// Define Return Vector
			LuaVectorStruct lvReturnVector;

			// Set Vector
			lvReturnVector.fVectorX = lvVectorOne.fVectorX - lvVectorTwo.fVectorX;
			lvReturnVector.fVectorY = lvVectorOne.fVectorY - lvVectorTwo.fVectorY;
			lvReturnVector.fVectorZ = lvVectorOne.fVectorZ - lvVectorTwo.fVectorZ;

			// Return Vector
			return lvReturnVector;
		}
		// Function to Multiply Two Vectors !COMPONENT WISE MULTIPLICATON!
		static LuaVectorStruct LuaVectorComponentWiseMultiplication(LuaVectorStruct lvVectorOne, LuaVectorStruct lvVectorTwo)
		{
			// Define Return Vector
			LuaVectorStruct lvReturnVector;

			// Set Vector
			lvReturnVector.fVectorX = lvVectorOne.fVectorX * lvVectorTwo.fVectorX;
			lvReturnVector.fVectorY = lvVectorOne.fVectorY * lvVectorTwo.fVectorY;
			lvReturnVector.fVectorZ = lvVectorOne.fVectorZ * lvVectorTwo.fVectorZ;

			// Return Vector
			return lvReturnVector;
		}
		// Function to get a the Dot Product of Two Input LuaVectors
		static float LuaVectorDotProduct(LuaVectorStruct lvVectorOne, LuaVectorStruct lvVectorTwo)
		{
			// Return Dot Product
			return (lvVectorOne.fVectorX * lvVectorTwo.fVectorX) + (lvVectorOne.fVectorY * lvVectorTwo.fVectorY) + (lvVectorOne.fVectorZ * lvVectorTwo.fVectorZ);
		}
		// Angle Between Two Vectors
		static float AngleBetweenTwoVectors(LuaVectorStruct lvVectorOne, LuaVectorStruct lvVectorTwo)
		{
			// Return Angle
			return acos(LuaVectorDotProduct(lvVectorOne, lvVectorTwo) / (lvVectorOne.GetMagnitude() * lvVectorTwo.GetMagnitude()));
		}
#pragma endregion

		// LuaColour Functions
#pragma region LuaColour
		// Create Colour Vector From Floats
		static LuaColourStruct CreateLuaColourVectorFromFloats(float r, float g, float b)
		{
			// Define Return Vector
			LuaColourStruct ReturnVector;
			ReturnVector.SetColour(r, g, b);

			// Return Vector
			return ReturnVector;
		}
		// Create Colour Vector From Hex String
		static LuaColourStruct CreateLuaColourVectorFromHex(std::string sInputHexString)
		{
			// Define Return Vector
			LuaColourStruct ReturnVector;
			ReturnVector.SetColourFromHex(sInputHexString);

			// Return Vector
			return ReturnVector;
		}
#pragma endregion

		// Logger Functions
#pragma region LoggerFunctions
		static void LoggerInfo(const std::string sStringInput) { LOG_INFO(sStringInput); }
		static void LoggerError(const std::string sStringInput) { LOG_ERROR(sStringInput); }
		static void LoggerCritical(const std::string sStringInput) { LOG_CRITICAL(sStringInput); }
		static void LoggerTrace(const std::string sStringInput) { LOG_TRACE(sStringInput); }
		static void LoggerWarning(const std::string sStringInput) { LOG_WARN(sStringInput); }
#pragma endregion

		// Active Script Functions
#pragma region ActiveScriptData
		// Set Active Script to Me
		static void SetActiveScriptToMe()
		{
			// Set to Me
			ActiveScript = me;
		}
		// Set Active Script
		static void SetActiveScript(int iObjectIndex)
		{
			// Set Active Script to Lua Component Attached to Given Object
		}
#pragma endregion

		// Object Transform Functions
#pragma region TransformFunctions
		// Object Transform Getters and Setters
		static float GetPosX() { return ActiveScript->ObjectVariables.vInputObjectPosition->x; }
		static void SetPosX(float input) { ActiveScript->bTransformUpdated = true; ActiveScript->ObjectVariables.vInputObjectPosition->x = input; }
		static float GetPosY() { return ActiveScript->ObjectVariables.vInputObjectPosition->y; }
		static void SetPosY(float input) { ActiveScript->bTransformUpdated = true; ActiveScript->ObjectVariables.vInputObjectPosition->y = input; }
		static float GetPosZ() { return ActiveScript->ObjectVariables.vInputObjectPosition->z; }
		static void SetPosZ(float input) { ActiveScript->bTransformUpdated = true; ActiveScript->ObjectVariables.vInputObjectPosition->z = input; }
		static void SetPos(float xInput, float yInput, float zInput) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectPosition = glm::vec3(xInput, yInput, zInput);  }
		static void SetPosFromLuaVector(LuaVectorStruct lvInput) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectPosition = glm::vec3(lvInput.fVectorX, lvInput.fVectorY, lvInput.fVectorZ); }
		static LuaVectorStruct GetPos()
		{
			// Define Return Vector
			LuaVectorStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorX = ActiveScript->ObjectVariables.vInputObjectPosition->x;
			vReturnVector.fVectorY = ActiveScript->ObjectVariables.vInputObjectPosition->y;
			vReturnVector.fVectorZ = ActiveScript->ObjectVariables.vInputObjectPosition->z;

			// Return Vector
			return vReturnVector;
		}
		static void TranslateByFloats(float fXMove, float fYMove, float fZMove) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectPosition += glm::vec3(fXMove, fYMove, fZMove) ;}
		static void TranslateByLuaVec(LuaVectorStruct lvInput) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectPosition += glm::vec3(lvInput.fVectorX, lvInput.fVectorY, lvInput.fVectorZ); }
		static float GetRotX() { return ActiveScript->ObjectVariables.vInputObjectRotation->x; }
		static void SetRotX(float input) { ActiveScript->bTransformUpdated = true; ActiveScript->ObjectVariables.vInputObjectRotation->x = input; }
		static float GetRotY() { return ActiveScript->ObjectVariables.vInputObjectRotation->y; }
		static void SetRotY(float input) { ActiveScript->bTransformUpdated = true; ActiveScript->ObjectVariables.vInputObjectRotation->y = input; }
		static float GetRotZ() { return ActiveScript->ObjectVariables.vInputObjectRotation->z; }
		static void SetRotZ(float input) { ActiveScript->bTransformUpdated = true; ActiveScript->ObjectVariables.vInputObjectRotation->z = input; }
		static void SetRot(float xInput, float yInput, float zInput) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectRotation = glm::vec3(xInput, yInput, zInput); }
		static void SetRotFromLuaVector(LuaVectorStruct lvInput) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectRotation = glm::vec3(lvInput.fVectorX, lvInput.fVectorY, lvInput.fVectorZ); }
		static LuaVectorStruct GetRot()
		{
			// Define Return Vector
			LuaVectorStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorX = ActiveScript->ObjectVariables.vInputObjectRotation->x;
			vReturnVector.fVectorY = ActiveScript->ObjectVariables.vInputObjectRotation->y;
			vReturnVector.fVectorZ = ActiveScript->ObjectVariables.vInputObjectRotation->z;

			// Return Vector
			return vReturnVector;
		}
		static void RotateByFloats(float fXRot, float fYRot, float fZRot) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectRotation += glm::vec3(fXRot, fYRot, fZRot); }
		static void RotateByLuaVec(LuaVectorStruct lvInput) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectRotation += glm::vec3(lvInput.fVectorX, lvInput.fVectorY, lvInput.fVectorZ); }
		static float GetScaX() { return ActiveScript->ObjectVariables.vInputObjectScale->x; }
		static void SetScaX(float input) { ActiveScript->bTransformUpdated = true; ActiveScript->ObjectVariables.vInputObjectScale->x = input; }
		static float GetScaY() { return ActiveScript->ObjectVariables.vInputObjectScale->y; }
		static void SetScaY(float input) { ActiveScript->bTransformUpdated = true; ActiveScript->ObjectVariables.vInputObjectScale->y = input; }
		static float GetScaZ() { return ActiveScript->ObjectVariables.vInputObjectScale->z; }
		static void SetScaZ(float input) { ActiveScript->bTransformUpdated = true; ActiveScript->ObjectVariables.vInputObjectScale->z = input; }
		static void SetSca(float xInput, float yInput, float zInput) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectScale = glm::vec3(xInput, yInput, zInput); }
		static void SetScaFromLuaVector(LuaVectorStruct lvInput) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectScale = glm::vec3(lvInput.fVectorX, lvInput.fVectorY, lvInput.fVectorZ); }
		static LuaVectorStruct GetSca()
		{
			// Define Return Vector
			LuaVectorStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorX = ActiveScript->ObjectVariables.vInputObjectScale->x;
			vReturnVector.fVectorY = ActiveScript->ObjectVariables.vInputObjectScale->y;
			vReturnVector.fVectorZ = ActiveScript->ObjectVariables.vInputObjectScale->z;

			// Return Vector
			return vReturnVector;
		}
		static void IncreaseScaleByFloats(float fXScaInc, float fYScaInc, float fZScaInc) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectScale += glm::vec3(fXScaInc, fYScaInc, fZScaInc); }
		static void IncreaseScaleByLuaVec(LuaVectorStruct lvInput) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectScale += glm::vec3(lvInput.fVectorX, lvInput.fVectorY, lvInput.fVectorZ); }
		static void ScaleByFloatFactor(float fXScaFactor, float fYScaFactor, float fZScaFactor) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectScale *= glm::vec3(fXScaFactor, fYScaFactor, fZScaFactor); }
		static void ScaleByLuaVecFactor(LuaVectorStruct lvInput) { ActiveScript->bTransformUpdated = true; *ActiveScript->ObjectVariables.vInputObjectScale *= glm::vec3(lvInput.fVectorX, lvInput.fVectorY, lvInput.fVectorZ); }

		//! Position Changed Function, MUST BE CALLED AFTER THE OBJECTS TRANSFORM IS CHANGED TO UPDATE THE MODEL
		static void TransformUpdated()
		{
			// Update Position
			glm::vec3 PosData(*ActiveScript->ObjectVariables.vInputObjectPosition);
			ActiveScript->sendMessage(ComponentMessage(ComponentMessageType::PositionSet, (void*)&PosData));

			// Update Rotation
			glm::vec3 RotData(*ActiveScript->ObjectVariables.vInputObjectRotation);
			ActiveScript->sendMessage(ComponentMessage(ComponentMessageType::RotationSet, (void*)&RotData));

			// Update Scale
			glm::vec3 ScaData(*ActiveScript->ObjectVariables.vInputObjectScale);
			ActiveScript->sendMessage(ComponentMessage(ComponentMessageType::ScaleSet, (void*)&ScaData));
		}
#pragma endregion

		// RigidBody Component Functions
#pragma region RigidBodyFunction
		static void RigidBodyApplyForce(LuaVectorStruct lvForce) { ActiveScript->ObjectVariables.MyRigidBodyComponent->applyForce(glm::vec3(lvForce.fVectorX, lvForce.fVectorY, lvForce.fVectorZ)); }
		static void RigidBodyApplyForceToPoint(LuaVectorStruct lvForce, LuaVectorStruct lvPoint) { ActiveScript->ObjectVariables.MyRigidBodyComponent->applyForce(glm::vec3(lvForce.fVectorX, lvForce.fVectorY, lvForce.fVectorZ), glm::vec3(lvPoint.fVectorX, lvPoint.fVectorY, lvPoint.fVectorZ)); }
		static void RigidBodyApplyTorque(LuaVectorStruct lvTorque) { ActiveScript->ObjectVariables.MyRigidBodyComponent->applyTorque(glm::vec3(lvTorque.fVectorX, lvTorque.fVectorY, lvTorque.fVectorZ)); }
		static void RigidBodySetVelocity(LuaVectorStruct lvInputVelocity) { ActiveScript->ObjectVariables.MyRigidBodyComponent->setLinearVelocity(glm::vec3(lvInputVelocity.fVectorX, lvInputVelocity.fVectorY, lvInputVelocity.fVectorZ)); }
		static LuaVectorStruct GetVelocity()
		{
			// Define Return Vector
			LuaVectorStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorX = ActiveScript->ObjectVariables.MyRigidBodyComponent->getLinearVelocityX();
			vReturnVector.fVectorY = ActiveScript->ObjectVariables.MyRigidBodyComponent->getLinearVelocityY();
			vReturnVector.fVectorZ = ActiveScript->ObjectVariables.MyRigidBodyComponent->getLinearVelocityZ();

			// Return Vector
			return vReturnVector;
		}
		static float RigidBodyGetVelocityX() { return ActiveScript->ObjectVariables.MyRigidBodyComponent->getLinearVelocityX(); } static void RigidBodySetVelocityX(float x) { ActiveScript->ObjectVariables.MyRigidBodyComponent->setLinearVelocityX(x); }
		static float RigidBodyGetVelocityY() { return ActiveScript->ObjectVariables.MyRigidBodyComponent->getLinearVelocityY(); } static void RigidBodySetVelocityY(float y) { ActiveScript->ObjectVariables.MyRigidBodyComponent->setLinearVelocityY(y); }
		static float RigidBodyGetVelocityZ() { return ActiveScript->ObjectVariables.MyRigidBodyComponent->getLinearVelocityZ(); } static void RigidBodySetVelocityZ(float z) { ActiveScript->ObjectVariables.MyRigidBodyComponent->setLinearVelocityZ(z); }
		static void RigidBodySetAngularVelocity(LuaVectorStruct lvAngVel) { ActiveScript->ObjectVariables.MyRigidBodyComponent->setAngularVelocity(glm::vec3(lvAngVel.fVectorX, lvAngVel.fVectorY, lvAngVel.fVectorZ)); }
		static LuaVectorStruct GetAngularVelocity()
		{
			// Define Return Vector
			LuaVectorStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorX = ActiveScript->ObjectVariables.MyRigidBodyComponent->getAngularVelocityX();
			vReturnVector.fVectorY = ActiveScript->ObjectVariables.MyRigidBodyComponent->getAngularVelocityY();
			vReturnVector.fVectorZ = ActiveScript->ObjectVariables.MyRigidBodyComponent->getAngularVelocityZ();

			// Return Vector
			return vReturnVector;
		}
		static float RigidBodyGetAngularVelocityX() { return ActiveScript->ObjectVariables.MyRigidBodyComponent->getAngularVelocityX(); } static void RigidBodySetAngularVelocityX(float x) { ActiveScript->ObjectVariables.MyRigidBodyComponent->setAngularVelocityX(x); }
		static float RigidBodyGetAngularVelocityY() { return ActiveScript->ObjectVariables.MyRigidBodyComponent->getAngularVelocityY(); } static void RigidBodySetAngularVelocityY(float y) { ActiveScript->ObjectVariables.MyRigidBodyComponent->setAngularVelocityY(y); }
		static float RigidBodyGetAngularVelocityZ() { return ActiveScript->ObjectVariables.MyRigidBodyComponent->getAngularVelocityZ(); } static void RigidBodySetAngularVelocityZ(float z) { ActiveScript->ObjectVariables.MyRigidBodyComponent->setAngularVelocityZ(z); }
#pragma endregion

		// Light Component Functions
#pragma region LightComponentFunctions
		static float GetLightAmbientX() { return ActiveScript->ObjectVariables.MyLightComponent->getAmbientX(); }
		static float GetLightAmbientY() { return ActiveScript->ObjectVariables.MyLightComponent->getAmbientY(); }
		static float GetLightAmbientZ() { return ActiveScript->ObjectVariables.MyLightComponent->getAmbientZ(); }
		static LuaColourStruct GetLightAmbient()
		{
			// Define Return Vector
			LuaColourStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorR = ActiveScript->ObjectVariables.MyLightComponent->getAmbientX();
			vReturnVector.fVectorG = ActiveScript->ObjectVariables.MyLightComponent->getAmbientY();
			vReturnVector.fVectorB = ActiveScript->ObjectVariables.MyLightComponent->getAmbientZ();

			// Return Vector
			return vReturnVector;
		}
		static void SetLightAmbientX(float input) { ActiveScript->ObjectVariables.MyLightComponent->setAmbientX(input); }
		static void SetLightAmbientY(float input) { ActiveScript->ObjectVariables.MyLightComponent->setAmbientY(input); }
		static void SetLightAmbientZ(float input) { ActiveScript->ObjectVariables.MyLightComponent->setAmbientZ(input); }
		static void SetLightAmbient(float xInput, float yInput, float zInput) { ActiveScript->ObjectVariables.MyLightComponent->setAmbient(glm::vec3(xInput, yInput, zInput)); }
		static void SetLightAmbientFromLuaCol(LuaColourStruct lvInput) { ActiveScript->ObjectVariables.MyLightComponent->setAmbient(glm::vec3(lvInput.fVectorR, lvInput.fVectorG, lvInput.fVectorB)); }
		static float GetLightPointDiffuseX() { return ActiveScript->ObjectVariables.MyLightComponent->getPointDiffuseX(); }
		static float GetLightPointDiffuseY() { return ActiveScript->ObjectVariables.MyLightComponent->getPointDiffuseY(); }
		static float GetLightPointDiffuseZ() { return ActiveScript->ObjectVariables.MyLightComponent->getPointDiffuseZ(); }
		static LuaColourStruct GetLightPointDiffuse()
		{
			// Define Return Vector
			LuaColourStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorR = ActiveScript->ObjectVariables.MyLightComponent->getPointDiffuseX();
			vReturnVector.fVectorG = ActiveScript->ObjectVariables.MyLightComponent->getPointDiffuseY();
			vReturnVector.fVectorB = ActiveScript->ObjectVariables.MyLightComponent->getPointDiffuseZ();

			// Return Vector
			return vReturnVector;
		}
		static void SetLightPointDiffuseX(float input) { ActiveScript->ObjectVariables.MyLightComponent->setPointDiffuseX(input); }
		static void SetLightPointDiffuseY(float input) { ActiveScript->ObjectVariables.MyLightComponent->setPointDiffuseY(input); }
		static void SetLightPointDiffuseZ(float input) { ActiveScript->ObjectVariables.MyLightComponent->setPointDiffuseZ(input); }
		static void SetLightPointDiffuse(float xInput, float yInput, float zInput) { ActiveScript->ObjectVariables.MyLightComponent->setPointDiffuse(glm::vec3(xInput, yInput, zInput)); }
		static void SetLightPointDiffuseFromLuaCol(LuaColourStruct lvInput) { ActiveScript->ObjectVariables.MyLightComponent->setPointDiffuse(glm::vec3(lvInput.fVectorR, lvInput.fVectorG, lvInput.fVectorB)); }
		static float GetLightPointSpecularX() { return ActiveScript->ObjectVariables.MyLightComponent->getPointSpecularX(); }
		static float GetLightPointSpecularY() { return ActiveScript->ObjectVariables.MyLightComponent->getPointSpecularY(); }
		static float GetLightPointSpecularZ() { return ActiveScript->ObjectVariables.MyLightComponent->getPointSpecularZ(); }
		static LuaColourStruct GetLightPointSpecular()
		{
			// Define Return Vector
			LuaColourStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorR = ActiveScript->ObjectVariables.MyLightComponent->getPointSpecularX();
			vReturnVector.fVectorG = ActiveScript->ObjectVariables.MyLightComponent->getPointSpecularY();
			vReturnVector.fVectorB = ActiveScript->ObjectVariables.MyLightComponent->getPointSpecularZ();

			// Return Vector
			return vReturnVector;
		}
		static void SetLightPointSpecularX(float input) { ActiveScript->ObjectVariables.MyLightComponent->setPointSpecularX(input); }
		static void SetLightPointSpecularY(float input) { ActiveScript->ObjectVariables.MyLightComponent->setPointSpecularY(input); }
		static void SetLightPointSpecularZ(float input) { ActiveScript->ObjectVariables.MyLightComponent->setPointSpecularZ(input); }
		static void SetLightPointSpecular(float xInput, float yInput, float zInput) { ActiveScript->ObjectVariables.MyLightComponent->setPointSpecular(glm::vec3(xInput, yInput, zInput)); }
		static void SetLightPointSpecularFromLuaCol(LuaColourStruct lvInput) { ActiveScript->ObjectVariables.MyLightComponent->setPointSpecular(glm::vec3(lvInput.fVectorR, lvInput.fVectorG, lvInput.fVectorB)); }
		static float GetLightDirectionalDiffuseX() { return ActiveScript->ObjectVariables.MyLightComponent->getDirectionalDiffuseX(); }
		static float GetLightDirectionalDiffuseY() { return ActiveScript->ObjectVariables.MyLightComponent->getDirectionalDiffuseY(); }
		static float GetLightDirectionalDiffuseZ() { return ActiveScript->ObjectVariables.MyLightComponent->getDirectionalDiffuseZ(); }
		static LuaColourStruct GetLightDirectionalDiffuse()
		{
			// Define Return Vector
			LuaColourStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorR = ActiveScript->ObjectVariables.MyLightComponent->getDirectionalDiffuseX();
			vReturnVector.fVectorG = ActiveScript->ObjectVariables.MyLightComponent->getDirectionalDiffuseY();
			vReturnVector.fVectorB = ActiveScript->ObjectVariables.MyLightComponent->getDirectionalDiffuseZ();

			// Return Vector
			return vReturnVector;
		}
		static void SetLightDirectionalDiffuseX(float input) { ActiveScript->ObjectVariables.MyLightComponent->setDirectionalDiffuseX(input); }
		static void SetLightDirectionalDiffuseY(float input) { ActiveScript->ObjectVariables.MyLightComponent->setDirectionalDiffuseY(input); }
		static void SetLightDirectionalDiffuseZ(float input) { ActiveScript->ObjectVariables.MyLightComponent->setDirectionalDiffuseZ(input); }
		static void SetLightDirectionalDiffuse(float xInput, float yInput, float zInput) { ActiveScript->ObjectVariables.MyLightComponent->setDirectionalDiffuse(glm::vec3(xInput, yInput, zInput)); }
		static void SetLightDirectionalDiffuseFromLuaCol(LuaColourStruct lvInput) { ActiveScript->ObjectVariables.MyLightComponent->setDirectionalDiffuse(glm::vec3(lvInput.fVectorR, lvInput.fVectorG, lvInput.fVectorB)); }
		static float GetLightDirectionalSpecularX() { return ActiveScript->ObjectVariables.MyLightComponent->getDirectionalSpecularX(); }
		static float GetLightDirectionalSpecularY() { return ActiveScript->ObjectVariables.MyLightComponent->getDirectionalSpecularY(); }
		static float GetLightDirectionalSpecularZ() { return ActiveScript->ObjectVariables.MyLightComponent->getDirectionalSpecularZ(); }
		static LuaColourStruct GetLightDirectionalSpecular()
		{
			// Define Return Vector
			LuaColourStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorR = ActiveScript->ObjectVariables.MyLightComponent->getDirectionalSpecularX();
			vReturnVector.fVectorG = ActiveScript->ObjectVariables.MyLightComponent->getDirectionalSpecularY();
			vReturnVector.fVectorB = ActiveScript->ObjectVariables.MyLightComponent->getDirectionalSpecularZ();

			// Return Vector
			return vReturnVector;
		}
		static void SetLightDirectionalSpecularX(float input) { ActiveScript->ObjectVariables.MyLightComponent->setDirectionalSpecularX(input); }
		static void SetLightDirectionalSpecularY(float input) { ActiveScript->ObjectVariables.MyLightComponent->setDirectionalSpecularY(input); }
		static void SetLightDirectionalSpecularZ(float input) { ActiveScript->ObjectVariables.MyLightComponent->setDirectionalSpecularZ(input); }
		static void SetLightDirectionalSpecular(float xInput, float yInput, float zInput) { ActiveScript->ObjectVariables.MyLightComponent->setDirectionalSpecular(glm::vec3(xInput, yInput, zInput)); }
		static void SetLightDirectionalSpecularFromLuaCol(LuaColourStruct lvInput) { ActiveScript->ObjectVariables.MyLightComponent->setDirectionalSpecular(glm::vec3(lvInput.fVectorR, lvInput.fVectorG, lvInput.fVectorB)); }
		static float GetLightAttenuation() { return ActiveScript->ObjectVariables.MyLightComponent->getAttenuation(); }
		static void SetLightAttenuation(float input) { ActiveScript->ObjectVariables.MyLightComponent->setAttenuation(input); }
#pragma endregion

		// Material Component Functions
#pragma region MaterialComponentFunctions
		static float GetMaterialDiffuseX() { return ActiveScript->ObjectVariables.MyMaterialComponent->getDiffuse().x; }
		static float GetMaterialDiffuseY() { return ActiveScript->ObjectVariables.MyMaterialComponent->getDiffuse().y; }
		static float GetMaterialDiffuseZ() { return ActiveScript->ObjectVariables.MyMaterialComponent->getDiffuse().z; }
		static LuaColourStruct GetMaterialDiffuse()
		{
			// Define Return Vector
			LuaColourStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorR = ActiveScript->ObjectVariables.MyMaterialComponent->getDiffuse().x;
			vReturnVector.fVectorG = ActiveScript->ObjectVariables.MyMaterialComponent->getDiffuse().y;
			vReturnVector.fVectorB = ActiveScript->ObjectVariables.MyMaterialComponent->getDiffuse().z;

			// Return Vector
			return vReturnVector;
		}
		static void SetMaterialDiffuseX(float input) { ActiveScript->ObjectVariables.MyMaterialComponent->setDiffuse(glm::vec3(input, GetMaterialDiffuseY(), GetMaterialDiffuseZ())); }
		static void SetMaterialDiffuseY(float input) { ActiveScript->ObjectVariables.MyMaterialComponent->setDiffuse(glm::vec3(GetMaterialDiffuseX(), input, GetMaterialDiffuseZ())); }
		static void SetMaterialDiffuseZ(float input) { ActiveScript->ObjectVariables.MyMaterialComponent->setDiffuse(glm::vec3(GetMaterialDiffuseX(), GetMaterialDiffuseY(), input)); }
		static void SetMaterialDiffuse(float x, float y, float z) { ActiveScript->ObjectVariables.MyMaterialComponent->setDiffuse(glm::vec3(x, y, z)); }
		static void SetMaterialDiffuseFromLuaCol(LuaColourStruct lvInput) { ActiveScript->ObjectVariables.MyMaterialComponent->setDiffuse(glm::vec3(lvInput.fVectorR, lvInput.fVectorG, lvInput.fVectorB)); }
		static float GetMaterialSpecularX() { return ActiveScript->ObjectVariables.MyMaterialComponent->getSpecular().x; }
		static float GetMaterialSpecularY() { return ActiveScript->ObjectVariables.MyMaterialComponent->getSpecular().y; }
		static float GetMaterialSpecularZ() { return ActiveScript->ObjectVariables.MyMaterialComponent->getSpecular().z; }
		static LuaColourStruct GetMaterialSpecular()
		{
			// Define Return Vector
			LuaColourStruct vReturnVector;

			// Set Vector Values
			vReturnVector.fVectorR = ActiveScript->ObjectVariables.MyMaterialComponent->getSpecular().x;
			vReturnVector.fVectorG = ActiveScript->ObjectVariables.MyMaterialComponent->getSpecular().y;
			vReturnVector.fVectorB = ActiveScript->ObjectVariables.MyMaterialComponent->getSpecular().z;

			// Return Vector
			return vReturnVector;
		}
		static void SetMaterialSpecularX(float input) { ActiveScript->ObjectVariables.MyMaterialComponent->setSpecular(glm::vec3(input, GetMaterialSpecularY(), GetMaterialSpecularZ())); }
		static void SetMaterialSpecularY(float input) { ActiveScript->ObjectVariables.MyMaterialComponent->setSpecular(glm::vec3(GetMaterialSpecularX(), input, GetMaterialSpecularZ())); }
		static void SetMaterialSpecularZ(float input) { ActiveScript->ObjectVariables.MyMaterialComponent->setSpecular(glm::vec3(GetMaterialSpecularX(), GetMaterialSpecularY(), input)); }
		static void SetMaterialSpecular(float x, float y, float z) { ActiveScript->ObjectVariables.MyMaterialComponent->setSpecular(glm::vec3(x, y, z)); }
		static void SetMaterialSpecularFromLuaCol(LuaColourStruct lvInput) { ActiveScript->ObjectVariables.MyMaterialComponent->setSpecular(glm::vec3(lvInput.fVectorR, lvInput.fVectorG, lvInput.fVectorB)); }
		static float GetMaterialShininess() { return ActiveScript->ObjectVariables.MyMaterialComponent->getShininess(); }
		static void SetMaterialShininess(float input) { ActiveScript->ObjectVariables.MyMaterialComponent->setShininess(input); }
#pragma endregion

		// Raycasting Functions
#pragma region RaycastingFunctions
		// Create Ray and get Index From Object
		static int CreateRayFromObject(LuaVectorStruct lvOffset, LuaVectorStruct lvDistance)
		{
			// Create New Ray Object and Return Index
			return G::addRay(ActiveScript->m_pOwner, glm::vec3(lvOffset.fVectorX, lvOffset.fVectorY, lvOffset.fVectorZ), glm::vec3(lvDistance.fVectorX, lvDistance.fVectorY, lvDistance.fVectorZ));
		}
		// Create Ray and get Index
		static int CreateRay(LuaVectorStruct lvStart, LuaVectorStruct lvDistance)
		{
			// Create New Ray Object and Return Index
			return G::addRay(glm::vec3(lvStart.fVectorX, lvStart.fVectorY, lvStart.fVectorZ), glm::vec3(lvDistance.fVectorX, lvDistance.fVectorY, lvDistance.fVectorZ));
		}
		// Create Ray and get Index From Object
		static RayHitInfo RayCastFromObject(LuaVectorStruct lvOffset, LuaVectorStruct lvDistance)
		{
			// Cast ray and Return RayHitInfo
			return G::castRay(ActiveScript->m_pOwner, glm::vec3(lvOffset.fVectorX, lvOffset.fVectorY, lvOffset.fVectorZ), glm::vec3(lvDistance.fVectorX, lvDistance.fVectorY, lvDistance.fVectorZ));
		}
		// Create Ray and get Index
		static RayHitInfo RayCastFromPoint(LuaVectorStruct lvStart, LuaVectorStruct lvDistance)
		{
			// Cast ray and Return RayHitInfo
			return G::castRay(glm::vec3(lvStart.fVectorX, lvStart.fVectorY, lvStart.fVectorZ), glm::vec3(lvDistance.fVectorX, lvDistance.fVectorY, lvDistance.fVectorZ));
		}
		// Set Ray Start Point
		static void ChangeRayStartPoint(int iRayIndex, LuaVectorStruct lvStart)
		{
			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Set Ray Start Point
			rpRay->changeStartPoint(glm::vec3(lvStart.fVectorX, lvStart.fVectorY, lvStart.fVectorZ));
		}
		// Get Ray Start Point
		static LuaVectorStruct GetRayStartPoint(int iRayIndex)
		{
			// Define Return Vector
			LuaVectorStruct vReturnVector;

			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Set Vector Values
			vReturnVector.fVectorX = rpRay->getStartX();
			vReturnVector.fVectorY = rpRay->getStartY();
			vReturnVector.fVectorZ = rpRay->getStartZ();

			// Return Vector
			return vReturnVector;
		}
		// Set Ray Direction/Distance
		static void ChangeRayDistance(int iRayIndex, LuaVectorStruct lvDistance)
		{
			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Set Ray Start Point
			rpRay->changeDistance(glm::vec3(lvDistance.fVectorX, lvDistance.fVectorY, lvDistance.fVectorZ));
		}
		// Set Ray Direction/Distancefrom LuaVector
		static void SetRayDistanceFromLuaVec(int iRayIndex, LuaVectorStruct lvInputDistance)
		{
			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Set Ray Start Point
			rpRay->changeDistance(glm::vec3(lvInputDistance.fVectorX, lvInputDistance.fVectorY, lvInputDistance.fVectorZ));
		}
		// Get Ray Direction/Distance
		static LuaVectorStruct GetRayDistance(int iRayIndex)
		{
			// Define Return Vector
			LuaVectorStruct vReturnVector;

			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Set Vector Values
			vReturnVector.fVectorX = rpRay->getDistance().x;
			vReturnVector.fVectorY = rpRay->getDistance().y;
			vReturnVector.fVectorZ = rpRay->getDistance().z;

			// Return Vector
			return vReturnVector;
		}
		// Get Ray X Distance
		static float GetRayDistanceX(int iRayIndex)
		{
			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Return Ray Distance X
			return rpRay->getDistanceX();
		}
		// Get Ray Y Distance
		static float GetRayDistanceY(int iRayIndex)
		{
			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Return Ray Distance Y
			return rpRay->getDistanceY();
		}
		// Get Ray Z Distance
		static float GetRayDistanceZ(int iRayIndex)
		{
			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Return Ray Distance Z
			return rpRay->getDistanceZ();
		}
		// Get Ray Length
		static float GetRayLength(int iRayIndex)
		{
			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Return Ray Length
			return rpRay->getLength();
		}
		// Enable Ray
		static void EnableRay(int iRayIndex)
		{
			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Enable Ray
			rpRay->enabled(true);
		}
		// Disable Ray
		static void DisableRay(int iRayIndex)
		{
			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Disable Ray
			rpRay->enabled(false);
		}
		// Get RayHitInfo
		static RayHitInfo GetRayHitInfo(int iRayIndex)
		{
			// Get Pointer to Ray
			Ray* rpRay = G::getRay(iRayIndex);

			// Return RayHitInfo
			return rpRay->getHitInfo();
		}
#pragma endregion

		// Model/Mesh Functions
#pragma region Mesh/ModelFunctions
		// Component Functions
		static void SetObjectModel(std::string sInput) { ActiveScript->ObjectVariables.MyMeshComponent->setModel(sInput); }

		// Constant Filepaths to Premade Models
#pragma region PremadeFilepaths
		std::string sPremadeCubeModelFilepath = "assets/models/cube.obj";
		std::string sPremadeCapsuleModelFilepath = "assets/models/Capsule.obj";
		std::string sPremadeCylinderModelFilepath = "assets/models/Cylinder.obj";
		std::string sPremadeQuadModelFilepath = "assets/models/Quad.obj";
		std::string sPremadeSphereModelFilepath = "assets/models/Sphere.obj";
#pragma endregion
#pragma endregion

		// Prefab Functions
#pragma region CreateObjectFunctions
#pragma region PrefabFunctions
		// Define New Prefab
		static ObjectPrefab CreateNewPrefab()
		{
			// Create Instance of ObjectPrefab
			ObjectPrefab pBlankPrefab;

			// Return New Prefab
			return pBlankPrefab;
		}
		// Define Cube Prefab
		static ObjectPrefab CreateCubePrefab()
		{
			// Create Instance of ObjectPrefab
			ObjectPrefab pNewCubePrefab;

			// Set Model Filepath to Cube
			pNewCubePrefab.sModelFilepath = ActiveScript->sPremadeCubeModelFilepath;

			// Return New Prefab
			return pNewCubePrefab;
		}
		// Define Capsule Prefab
		static ObjectPrefab CreateCapsulePrefab()
		{
			// Create Instance of ObjectPrefab
			ObjectPrefab pNewPrefab;

			// Set Model Filepath to Cube
			pNewPrefab.sModelFilepath = ActiveScript->sPremadeCapsuleModelFilepath;

			// Return New Prefab
			return pNewPrefab;
		}
		// Define Cylinder Prefab
		static ObjectPrefab CreateCylinderPrefab()
		{
			// Create Instance of ObjectPrefab
			ObjectPrefab pNewPrefab;

			// Set Model Filepath to Cube
			pNewPrefab.sModelFilepath = ActiveScript->sPremadeCylinderModelFilepath;

			// Return New Prefab
			return pNewPrefab;
		}
		// Define Quad Prefab
		static ObjectPrefab CreateQuadPrefab()
		{
			// Create Instance of ObjectPrefab
			ObjectPrefab pNewPrefab;

			// Set Model Filepath to Cube
			pNewPrefab.sModelFilepath = ActiveScript->sPremadeQuadModelFilepath;

			// Return New Prefab
			return pNewPrefab;
		}
		// Define Sphere Prefab
		static ObjectPrefab CreateSpherePrefab()
		{
			// Create Instance of ObjectPrefab
			ObjectPrefab pNewPrefab;

			// Set Model Filepath to Cube
			pNewPrefab.sModelFilepath = ActiveScript->sPremadeSphereModelFilepath;

			// Return New Prefab
			return pNewPrefab;
		}
#pragma endregion
		// Create Object From Prefab
		static void CreateObjectFromPrefab(ObjectPrefab pNewObjectPrefab)
		{
			// Save JSON Layer Pointer
			JSONLayer* JSONLayerPointer = ActiveScript->ObjectVariables.MyJSONLayer;

			// Add Space to Layer Vectors for New Cube
			JSONLayerPointer->getGameObjects().push_back(nullptr);
			JSONLayerPointer->getMaterials().push_back(nullptr);
			JSONLayerPointer->getMeshes().push_back(nullptr);
			JSONLayerPointer->getPositions().push_back(nullptr);
			JSONLayerPointer->getTextures().push_back(nullptr);

			// Create New gameobject in the layer
			std::string name = pNewObjectPrefab.sObjectName; // Set Name
			std::string tag = pNewObjectPrefab.sObjectTag; // Set Tag
			JSONLayerPointer->getGameObjects().at(JSONLayerPointer->getGameObjects().size() - 1) = std::make_shared<GameObject>(GameObject(name, tag, (int)JSONLayerPointer->getGameObjects().size() - 1)); // Create
			std::shared_ptr<GameObject> NewObject = JSONLayerPointer->getGameObjects().at(JSONLayerPointer->getGameObjects().size() - 1); // get Object

			// Create New Mesh Component
			std::shared_ptr<Engine::Model> model = nullptr; // Define Pointer to Model
			if (pNewObjectPrefab.sModelFilepath != "") // If Prefab Has A Model Assigned
				model = JSONLayerPointer->getResources()->addModel(pNewObjectPrefab.sModelFilepath); // Register Model With Resources and Set Pointer
			JSONLayerPointer->getMeshes().at(JSONLayerPointer->getMeshes().size() - 1) = std::make_shared<Engine::MeshComponent>(Engine::MeshComponent(model)); // Save Model Pointer
			NewObject->addComponent(JSONLayerPointer->getMeshes().at(JSONLayerPointer->getMeshes().size() - 1)); // Add Component to GameObject

			// Add Shader to Resource Manager
			std::shared_ptr<Shader> shader = JSONLayerPointer->getResources()->addShader(pNewObjectPrefab.sShaderFilepath);

			// Define the New Object's Material
			std::shared_ptr<VertexArray> VAO = JSONLayerPointer->getResources()->addVAO(pNewObjectPrefab.sModelFilepath); // Get Vertex Array From Object
			VAO->setVertexBuffer(JSONLayerPointer->getResources()->addVBO(pNewObjectPrefab.sModelFilepath, model->vertices, sizeof(float) * model->verticesSize, shader->getBufferLayout())); // Set Vertex Buffer
			VAO->setIndexBuffer(JSONLayerPointer->getResources()->addIndexBuffer(pNewObjectPrefab.sModelFilepath, model->indices, model->indicesSize)); // Set Index Buffer
			std::shared_ptr<Material> mat = JSONLayerPointer->getResources()->addMaterial(name + std::to_string(JSONLayerPointer->getGameObjects().size() - 1), shader, VAO); // Create Pointer to Material

			// Define a Material Component With the Above Material
			JSONLayerPointer->getMaterials().at(JSONLayerPointer->getMaterials().size() - 1) = std::make_shared<MaterialComponent>(MaterialComponent(mat)); // Set Pointer
			JSONLayerPointer->getMaterials().at(JSONLayerPointer->getMaterials().size() - 1)->setDiffuse(glm::vec3(pNewObjectPrefab.lcMaterialDiffuse.fVectorR, pNewObjectPrefab.lcMaterialDiffuse.fVectorG, pNewObjectPrefab.lcMaterialDiffuse.fVectorB)); // Set Diffuse to Prefab Values
			JSONLayerPointer->getMaterials().at(JSONLayerPointer->getMaterials().size() - 1)->setSpecular(glm::vec3(pNewObjectPrefab.lcMaterialSpecular.fVectorR, pNewObjectPrefab.lcMaterialSpecular.fVectorG, pNewObjectPrefab.lcMaterialSpecular.fVectorB)); // Set Specular to Prefab Values
			JSONLayerPointer->getMaterials().at(JSONLayerPointer->getMaterials().size() - 1)->setShininess(pNewObjectPrefab.fMaterialShininess); // Set Shininesss to Default
			NewObject->addComponent(JSONLayerPointer->getMaterials().at(JSONLayerPointer->getMaterials().size() - 1)); // Add Material Component to Object

			// Define New Position Component
			glm::vec3 vNewObjectPosition = glm::vec3(pNewObjectPrefab.lvPosition.fVectorX, pNewObjectPrefab.lvPosition.fVectorY, pNewObjectPrefab.lvPosition.fVectorZ);
			glm::vec3 vNewObjectRotation = glm::vec3(pNewObjectPrefab.lvRotation.fVectorX, pNewObjectPrefab.lvRotation.fVectorY, pNewObjectPrefab.lvRotation.fVectorZ);
			glm::vec3 vNewObjectScale = glm::vec3(pNewObjectPrefab.lvScale.fVectorX, pNewObjectPrefab.lvScale.fVectorY, pNewObjectPrefab.lvScale.fVectorZ);
			JSONLayerPointer->getPositions().at(JSONLayerPointer->getPositions().size() - 1) = std::make_shared<PositionComponent>(PositionComponent(vNewObjectPosition, vNewObjectRotation, vNewObjectScale)); // Create Component
			NewObject->addComponent(JSONLayerPointer->getPositions().at(JSONLayerPointer->getPositions().size() - 1)); // Add Component to Object

			// Texture
			std::shared_ptr<Texture> tex = JSONLayerPointer->getResources()->addTexture(pNewObjectPrefab.sTextureFilepath); // Define Texture
			JSONLayerPointer->getTextures().at(JSONLayerPointer->getTextures().size() - 1) = std::make_shared<Engine::TextureComponent>(Engine::TextureComponent(tex, tex)); // Make Texture Component
			NewObject->addComponent(JSONLayerPointer->getTextures().at(JSONLayerPointer->getTextures().size() - 1)); // Add Texture Component to Object

			// Add Collider Component?
			if (pNewObjectPrefab.bColliderComponentExists)
			{
				// Create Relevant Collider
				if (pNewObjectPrefab.sColliderType == "box")
				{
					// Create Box Collider
					JSONLayerPointer->getColliders().push_back(std::make_shared<BoxCollisionComponent>(BoxCollisionComponent(pNewObjectPrefab.BoxColliderHalfExtents, pNewObjectPrefab.fColliderMass, pNewObjectPrefab.ColliderCollisionCategory, pNewObjectPrefab.ColliderCanCollideWith)));
					//LOG_INFO(JSONLayerPointer->getColliders().size());
				}
				else if (pNewObjectPrefab.sColliderType == "sphere")
				{
					// Create Sphere Collider
					JSONLayerPointer->getColliders().push_back(std::make_shared<SphereCollisionComponent>(SphereCollisionComponent(pNewObjectPrefab.fColliderRadius, pNewObjectPrefab.fColliderMass, pNewObjectPrefab.ColliderCollisionCategory, pNewObjectPrefab.ColliderCanCollideWith)));
				}
				else if (pNewObjectPrefab.sColliderType == "capsule")
				{
					// Create Capsule Collider
					JSONLayerPointer->getColliders().push_back(std::make_shared<CapsuleCollisionComponent>(CapsuleCollisionComponent(pNewObjectPrefab.fColliderRadius, pNewObjectPrefab.fColliderHeight, pNewObjectPrefab.fColliderMass, pNewObjectPrefab.ColliderCollisionCategory, pNewObjectPrefab.ColliderCanCollideWith)));
				}
				else if (pNewObjectPrefab.sColliderType == "convex mesh")
				{
					// Add Model and Get Pointer
					std::shared_ptr<Model> model = JSONLayerPointer->getResources()->addModel(pNewObjectPrefab.sColliderConvexMeshFilepath);

					// Create Convex Mesh Collider
					JSONLayerPointer->getColliders().push_back(std::make_shared<ConvexMeshCollisionComponent>(ConvexMeshCollisionComponent(pNewObjectPrefab.fColliderMass, model, JSONLayerPointer->getPositions().at(JSONLayerPointer->getPositions().size() - 1)->getScale(), pNewObjectPrefab.ColliderCollisionCategory, pNewObjectPrefab.ColliderCanCollideWith)));
				}

				// Add the component to the object
				NewObject->addComponent(JSONLayerPointer->getColliders().at(JSONLayerPointer->getColliders().size() - 1));
				//LOG_INFO(pNewObjectPrefab.ColliderCanCollideWith);
			}

			// Add RigidBody Component?
			if (pNewObjectPrefab.bRigidBodyComponentExists) // If Component Exists
			{
				// Define Body Type
				rp3d::BodyType type;

				// Preset Uses Gravity to False
				bool usesGravity = false;

				// Set the RidigidBodies Type
				if (pNewObjectPrefab.sRigidBodyType == "dynamic")
				{
					// Set Body Type as Dynamic
					type = rp3d::BodyType::DYNAMIC;

					// Check if it Uses Gravity
					usesGravity = pNewObjectPrefab.bUsesGravity;
				}
				else if (pNewObjectPrefab.sRigidBodyType == "static")
					type = rp3d::BodyType::STATIC; // Set Body Type as Static
				else if (pNewObjectPrefab.sRigidBodyType == "kinematic")
					type = rp3d::BodyType::KINEMATIC; // Set Body Type as Kinematic

				// Define the component
				JSONLayerPointer->getRigidbodies().push_back(std::make_shared<RigidBodyComponent>(RigidBodyComponent(
					JSONLayerPointer->getDynamicSystem()->getWorld(),
					JSONLayerPointer->getPositions().at(JSONLayerPointer->getPositions().size() - 1)->getPosition(),
					JSONLayerPointer->getPositions().at(JSONLayerPointer->getPositions().size() - 1)->getRotation(),
					JSONLayerPointer->getDynamicSystem()->getTimeAccumulator(), type, usesGravity,
					JSONLayerPointer->getDynamicSystem()->getTimestep())));
				
				// Set the the values in material of the body
				rp3d::Material* mat = JSONLayerPointer->getRigidbodies().at(JSONLayerPointer->getRigidbodies().size() - 1)->getPhysicsMaterial();
				mat->setBounciness(rp3d::decimal(pNewObjectPrefab.fRigidBodyBounciness));
				mat->setFrictionCoefficient(rp3d::decimal(pNewObjectPrefab.fRigidBodyFriction));
				mat->setRollingResistance(rp3d::decimal(pNewObjectPrefab.fRigidBodyRollingResistance));
				
				// Add the component to the object
				NewObject->addComponent(JSONLayerPointer->getRigidbodies().at(JSONLayerPointer->getRigidbodies().size() - 1));
			}			

			// Add Lua Script Component?
			if (pNewObjectPrefab.bLuaScriptComponentExists)
			{
				// Define Input Lua Script Variables Struct
				LuaScriptComponent<DynamicPhysicsSystem>::LuaScriptVariables InputParameters;

				// Add Pointers to Position, Rotation and Scale
				InputParameters.vInputObjectPosition = &JSONLayerPointer->getPositions().at(JSONLayerPointer->getPositions().size() - 1)->getPosition();
				InputParameters.vInputObjectRotation = &JSONLayerPointer->getPositions().at(JSONLayerPointer->getPositions().size() - 1)->getRotationDeg();
				InputParameters.vInputObjectScale = &JSONLayerPointer->getPositions().at(JSONLayerPointer->getPositions().size() - 1)->getScale();
				
				// Add Pointer to JSON Layer
				InputParameters.MyJSONLayer = JSONLayerPointer;

				// Get RidgidBody Component?
				if (pNewObjectPrefab.bRigidBodyComponentExists)
					InputParameters.MyRigidBodyComponent = JSONLayerPointer->getRigidbodies().at(JSONLayerPointer->getRigidbodies().size() - 1);

				// Get Material Component?
				InputParameters.MyMaterialComponent = JSONLayerPointer->getMaterials().at(JSONLayerPointer->getMaterials().size() - 1);

				// Get Mesh Component?
				InputParameters.MyMeshComponent = JSONLayerPointer->getMeshes().at(JSONLayerPointer->getMeshes().size() - 1);	

				// Get Texture Component?
				InputParameters.MyTextureComponent = JSONLayerPointer->getTextures().at(JSONLayerPointer->getTextures().size() - 1);

				// Make Component
				JSONLayerPointer->getLuaScripts().push_back(std::make_shared<LuaScriptComponent<DynamicPhysicsSystem>>(LuaScriptComponent<DynamicPhysicsSystem>(pNewObjectPrefab.sLuaScriptComponentFilePath, InputParameters)));
				
				// Add Component to Object
				NewObject->addComponent(JSONLayerPointer->getLuaScripts().at(JSONLayerPointer->getLuaScripts().size() - 1));
			}

			//LoggerInfo("Prefab Created at (" + vNewObjectPosition.x + ", " + vNewObjectPosition.y + ", " + vNewObjectPosition.z + ")");
		}
		// Create Object From Prefab At Defined Location
		static void CreateObjectFromPrefabAtLocation(ObjectPrefab pNewObjectPrefab, LuaVectorStruct lvInputObjectPosition)
		{
			// Set Object Prefab Position
			pNewObjectPrefab.lvPosition = lvInputObjectPosition;

			// Create Object
			CreateObjectFromPrefab(pNewObjectPrefab);
		}
#pragma endregion

		// Texture Component Functions
		static float getTextureHeightScale() { return ActiveScript->ObjectVariables.MyTextureComponent->getHeightScale(); }
		static void setTextureHeightScale(float input) { ActiveScript->ObjectVariables.MyTextureComponent->setHeightScale(input); }

		// Input Polling Functions
#pragma region InputPolling
		void AssignKeyCodes()
		{
			// Assign Key Codes
			KeyCodesArray[KeyCodesIndex::a]				= ENGINE_KEY_A;
			KeyCodesArray[KeyCodesIndex::b]				= ENGINE_KEY_B;
			KeyCodesArray[KeyCodesIndex::c]				= ENGINE_KEY_C;
			KeyCodesArray[KeyCodesIndex::d]				= ENGINE_KEY_D;
			KeyCodesArray[KeyCodesIndex::e]				= ENGINE_KEY_E;
			KeyCodesArray[KeyCodesIndex::f]				= ENGINE_KEY_F;
			KeyCodesArray[KeyCodesIndex::g]				= ENGINE_KEY_G;
			KeyCodesArray[KeyCodesIndex::h]				= ENGINE_KEY_H;
			KeyCodesArray[KeyCodesIndex::i]				= ENGINE_KEY_I;
			KeyCodesArray[KeyCodesIndex::j]				= ENGINE_KEY_J;
			KeyCodesArray[KeyCodesIndex::k]				= ENGINE_KEY_K;
			KeyCodesArray[KeyCodesIndex::l]				= ENGINE_KEY_L;
			KeyCodesArray[KeyCodesIndex::m]				= ENGINE_KEY_M;
			KeyCodesArray[KeyCodesIndex::n]				= ENGINE_KEY_N;
			KeyCodesArray[KeyCodesIndex::o]				= ENGINE_KEY_O;
			KeyCodesArray[KeyCodesIndex::p]				= ENGINE_KEY_P;
			KeyCodesArray[KeyCodesIndex::q]				= ENGINE_KEY_Q;
			KeyCodesArray[KeyCodesIndex::r]				= ENGINE_KEY_R;
			KeyCodesArray[KeyCodesIndex::s]				= ENGINE_KEY_S;
			KeyCodesArray[KeyCodesIndex::t]				= ENGINE_KEY_T;
			KeyCodesArray[KeyCodesIndex::u]				= ENGINE_KEY_U;
			KeyCodesArray[KeyCodesIndex::v]				= ENGINE_KEY_V;
			KeyCodesArray[KeyCodesIndex::w]				= ENGINE_KEY_W;
			KeyCodesArray[KeyCodesIndex::x]				= ENGINE_KEY_X;
			KeyCodesArray[KeyCodesIndex::y]				= ENGINE_KEY_Y;
			KeyCodesArray[KeyCodesIndex::z]				= ENGINE_KEY_Z;
			KeyCodesArray[KeyCodesIndex::num_0]			= ENGINE_KEY_0;
			KeyCodesArray[KeyCodesIndex::num_1]			= ENGINE_KEY_1;
			KeyCodesArray[KeyCodesIndex::num_2]			= ENGINE_KEY_2;
			KeyCodesArray[KeyCodesIndex::num_3]			= ENGINE_KEY_3;
			KeyCodesArray[KeyCodesIndex::num_4]			= ENGINE_KEY_4;
			KeyCodesArray[KeyCodesIndex::num_5]			= ENGINE_KEY_5;
			KeyCodesArray[KeyCodesIndex::num_6]			= ENGINE_KEY_6;
			KeyCodesArray[KeyCodesIndex::num_7]			= ENGINE_KEY_7;
			KeyCodesArray[KeyCodesIndex::num_8]			= ENGINE_KEY_8;
			KeyCodesArray[KeyCodesIndex::num_9]			= ENGINE_KEY_9;
			KeyCodesArray[KeyCodesIndex::space]			= ENGINE_KEY_SPACE;
			KeyCodesArray[KeyCodesIndex::escape]		= ENGINE_KEY_ESCAPE;
			KeyCodesArray[KeyCodesIndex::enter]			= ENGINE_KEY_ENTER;
			KeyCodesArray[KeyCodesIndex::tab]			= ENGINE_KEY_TAB;
			KeyCodesArray[KeyCodesIndex::backspace]		= ENGINE_KEY_BACKSPACE;
			KeyCodesArray[KeyCodesIndex::insert]		= ENGINE_KEY_INSERT;
			KeyCodesArray[KeyCodesIndex::del]			= ENGINE_KEY_DELETE;
			KeyCodesArray[KeyCodesIndex::right]			= ENGINE_KEY_RIGHT;
			KeyCodesArray[KeyCodesIndex::left]			= ENGINE_KEY_LEFT;
			KeyCodesArray[KeyCodesIndex::up]			= ENGINE_KEY_UP;
			KeyCodesArray[KeyCodesIndex::down]			= ENGINE_KEY_DOWN;
			KeyCodesArray[KeyCodesIndex::left_shift]	= ENGINE_KEY_LEFT_SHIFT;
			KeyCodesArray[KeyCodesIndex::left_cntrl]	= ENGINE_KEY_LEFT_CONTROL;
			KeyCodesArray[KeyCodesIndex::left_alt]		= ENGINE_KEY_LEFT_ALT;
			KeyCodesArray[KeyCodesIndex::right_cntrl]	= ENGINE_KEY_RIGHT_CONTROL;
			KeyCodesArray[KeyCodesIndex::right_shift]	= ENGINE_KEY_RIGHT_SHIFT;
			KeyCodesArray[KeyCodesIndex::right_alt]		= ENGINE_KEY_RIGHT_ALT;
			KeyCodesArray[KeyCodesIndex::mouse_1]		= ENGINE_MOUSE_BUTTON_1;
			KeyCodesArray[KeyCodesIndex::mouse_2]		= ENGINE_MOUSE_BUTTON_2;
			KeyCodesArray[KeyCodesIndex::mouse_3]		= ENGINE_MOUSE_BUTTON_3;
			KeyCodesArray[KeyCodesIndex::mouse_4]		= ENGINE_MOUSE_BUTTON_4;
			KeyCodesArray[KeyCodesIndex::mouse_5]		= ENGINE_MOUSE_BUTTON_5;
			KeyCodesArray[KeyCodesIndex::mouse_6]		= ENGINE_MOUSE_BUTTON_6;
			KeyCodesArray[KeyCodesIndex::mouse_7]		= ENGINE_MOUSE_BUTTON_7;
			KeyCodesArray[KeyCodesIndex::mouse_8]		= ENGINE_MOUSE_BUTTON_8;
			KeyCodesArray[KeyCodesIndex::mouse_last]	= ENGINE_MOUSE_BUTTON_LAST;
			KeyCodesArray[KeyCodesIndex::mouse_middle]	= ENGINE_MOUSE_BUTTON_MIDDLE;
			KeyCodesArray[KeyCodesIndex::mouse_right]	= ENGINE_MOUSE_BUTTON_RIGHT;
			KeyCodesArray[KeyCodesIndex::mouse_left]	= ENGINE_MOUSE_BUTTON_LEFT;
		}
#pragma region KeyPressed
		/*
#pragma region Letters
		static bool InputAPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_A); }
		static bool InputBPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_B); }
		static bool InputCPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_C); }
		static bool InputDPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_D); }
		static bool InputEPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_E); }
		static bool InputFPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_F); }
		static bool InputGPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_G); }
		static bool InputHPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_H); }
		static bool InputIPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_I); }
		static bool InputJPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_J); }
		static bool InputKPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_K); }
		static bool InputLPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_L); }
		static bool InputMPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_M); }
		static bool InputNPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_N); }
		static bool InputOPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_O); }
		static bool InputPPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_P); }
		static bool InputQPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_Q); }
		static bool InputRPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_R); }
		static bool InputSPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_S); }
		static bool InputTPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_T); }
		static bool InputUPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_U); }
		static bool InputVPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_V); }
		static bool InputWPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_W); }
		static bool InputXPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_X); }
		static bool InputYPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_Y); }
		static bool InputZPressed() { return InputPoller::isKeyPressed(ENGINE_KEY_Z); }
#pragma endregion
		*/
		static bool InputKeyPressed(int iKeyCode) { return InputPoller::isKeyPressed(iKeyCode); }
		static bool InputMouseButtonPressed(int iKeyCode) { return InputPoller::isMouseButtonPressed(iKeyCode); }
		static float GetMouseX() { return InputPoller::getMouseX(); }
		static float GetMouseY() { return InputPoller::getMouseY(); }
#pragma endregion
#pragma endregion

		// Add Object Data
		//! Add General Object Data Function, Adds Object Data to Lua Scripts
		void AddObjectData()
		{
			// Add Object Data
			// Open Global Namespace
			luabridge::getGlobalNamespace(L)

				// Add LuaVector Namespace
#pragma region AddLuaVectors
				.beginNamespace("LuaVector") // Begin Lua Vector Namespace
				.addFunction("Create", CreateLuaVector) // Add Create LuaVector Function
				.addFunction("Addition", LuaVectorAddition) // Add Lua Vector Addition Function
				.addFunction("Subtraction", LuaVectorSubtraction) // Add LuaVector Subtraction Function
				.addFunction("Multiplication", LuaVectorComponentWiseMultiplication) // Add Lua Vector Component Wise Multiplication Function
				.addFunction("DotProduct", LuaVectorDotProduct) // Add LuaVector Dot Product Function
				.addFunction("AngleBetween", AngleBetweenTwoVectors) // Add Lua Vector Angle Between Function
				.endNamespace() // End Lua Vector Namespace
#pragma endregion

				// Add LuaColour Namespace
#pragma region LuaColour
				.beginNamespace("LuaColour") // Begin Lua Vector Namespace
				.addFunction("CreateFromRGB", CreateLuaColourVectorFromFloats) // Add Create LuaColour From Floats Function
				.addFunction("CreateFromHex", CreateLuaColourVectorFromHex) // Add Create LuaColour From Hex Function
				.endNamespace() // End Lua Vector Namespace
#pragma endregion

				// Add LuaVector Class
#pragma region AddLuaVectorClass
				.beginClass <LuaVectorStruct>("LuaVectorObject") // Define Class
				.addProperty("x", &LuaVectorStruct::fVectorX) // Add X Component
				.addProperty("y", &LuaVectorStruct::fVectorY) // Add Y Component
				.addProperty("z", &LuaVectorStruct::fVectorZ) // Add Z Component
				.addFunction("Set", &LuaVectorStruct::SetVector) // Set Values
				.addFunction("Magnitude", &LuaVectorStruct::GetMagnitude) // Get the Magnitude
				.addFunction("Normalize", &LuaVectorStruct::NormalizeVector) // Get the Magnitude
				.addFunction("ScaleBy", &LuaVectorStruct::ScaleVectorBy) // Scale the Vector by an Input Factor
				.endClass() // Stop Defining Class
#pragma endregion

				// Add LuaColour Class
#pragma region AddLuaColourClass
				.beginClass <LuaColourStruct>("LuaColourObject") // Define Class
				.addProperty("r", &LuaColourStruct::fVectorR) // Add R Component
				.addProperty("g", &LuaColourStruct::fVectorG) // Add G Component
				.addProperty("b", &LuaColourStruct::fVectorB) // Add B Component
				.addFunction("SetFromRGB", &LuaColourStruct::SetColour) // Set Values From RGB
				.addFunction("SetFromHex", &LuaColourStruct::SetColourFromHex) // Set Values From Hex
				.endClass() // Stop Defining Class
#pragma endregion
				
				// Add Transform Functionality
#pragma region AddObjectTransform
				.beginNamespace("Transform") // Create transform Namespace
				.beginNamespace("Position") // Create position Subnamespace
				.addProperty("x", GetPosX, SetPosX) // Add X Position Variable
				.addProperty("y", GetPosY, SetPosY) // Add Y Position Variable
				.addProperty("z", GetPosZ, SetPosZ) // Add Z Position Variable
				.addFunction("Get", GetPos) // Get Pos as LuaVector
				.addFunction("Set", SetPosFromLuaVector) // Set Pos From LuaVector
				.addFunction("Translate", TranslateByLuaVec) // Translate Object By LuaVector
				.endNamespace() // End position Subnamespace
				.beginNamespace("Rotation") // Create rotation Subnamespace
				.addProperty("x", GetRotX, SetRotX) // Add X Rotation Variable
				.addProperty("y", GetRotY, SetRotY) // Add X Rotation Variable
				.addProperty("z", GetRotZ, SetRotZ) // Add X Rotation Variable
				.addFunction("Get", GetRot) // Get Rot as LuaVector
				.addFunction("Set", SetRotFromLuaVector) // Set Pos From LuaVector
				.addFunction("Rotate", RotateByLuaVec) // Rotate Object By LuaVector
				.endNamespace() // End rotation Subnamespace
				.beginNamespace("Scale") // Create scale Subnamespace
				.addProperty("x", GetScaX, SetScaX) // Add X Scale Variable
				.addProperty("y", GetScaY, SetScaY) // Add Y Scale Variable
				.addProperty("z", GetScaZ, SetScaZ) // Add Z Scale Variable
				.addFunction("Get", GetSca) // Get Sca as LuaVector
				.addFunction("Set", SetScaFromLuaVector) // Set Pos From LuaVector
				.addFunction("IncrementScale", IncreaseScaleByLuaVec) // Increment Object Scale By LuaVector
				.addFunction("ScaleByFactor", ScaleByLuaVecFactor) // Translate Object By LuaVector
				.endNamespace() // End scale Subnamespace
				.endNamespace() // End Parent transform Namespace
#pragma endregion

				// Add Time Functionality
#pragma region AddTimestep
				.beginNamespace("Time") // Create Time Namespace
				.addProperty("Timestep", &deltaTime) // Add Timestep
				.endNamespace() // End time Namespace
#pragma endregion

				// Add Logger Functionality
#pragma region AddLoggerFunctions
				.beginNamespace("Logger") // Create logger Namespace
				.addFunction("Info", LoggerInfo) // Add Logger Info Function
				.addFunction("Trace", LoggerTrace) // Add Logger Info Function
				.addFunction("Error", LoggerError) // Add Logger Info Function
				.addFunction("Warn", LoggerWarning) // Add Logger Info Function
				.addFunction("Critical", LoggerCritical) // Add Logger Info Function
				.endNamespace() // End logger Namespace
#pragma endregion

				// Add Input Polling Functionality
#pragma region AddInputPolling
				// Create Input Namespace
				.beginNamespace("Input")

				// Add Keys
#pragma region KeyboardKeyCodes
				.beginNamespace("Keys")
				.addProperty("a", &KeyCodesArray[KeyCodesIndex::a], false)
				.addProperty("b", &KeyCodesArray[KeyCodesIndex::b], false)
				.addProperty("c", &KeyCodesArray[KeyCodesIndex::c], false)
				.addProperty("d", &KeyCodesArray[KeyCodesIndex::d], false)
				.addProperty("e", &KeyCodesArray[KeyCodesIndex::e], false)
				.addProperty("f", &KeyCodesArray[KeyCodesIndex::f], false)
				.addProperty("g", &KeyCodesArray[KeyCodesIndex::g], false)
				.addProperty("h", &KeyCodesArray[KeyCodesIndex::h], false)
				.addProperty("i", &KeyCodesArray[KeyCodesIndex::i], false)
				.addProperty("j", &KeyCodesArray[KeyCodesIndex::j], false)
				.addProperty("k", &KeyCodesArray[KeyCodesIndex::k], false)
				.addProperty("l", &KeyCodesArray[KeyCodesIndex::l], false)
				.addProperty("m", &KeyCodesArray[KeyCodesIndex::m], false)
				.addProperty("n", &KeyCodesArray[KeyCodesIndex::n], false)
				.addProperty("o", &KeyCodesArray[KeyCodesIndex::o], false)
				.addProperty("p", &KeyCodesArray[KeyCodesIndex::p], false)
				.addProperty("q", &KeyCodesArray[KeyCodesIndex::q], false)
				.addProperty("r", &KeyCodesArray[KeyCodesIndex::r], false)
				.addProperty("s", &KeyCodesArray[KeyCodesIndex::s], false)
				.addProperty("t", &KeyCodesArray[KeyCodesIndex::t], false)
				.addProperty("u", &KeyCodesArray[KeyCodesIndex::u], false)
				.addProperty("v", &KeyCodesArray[KeyCodesIndex::v], false)
				.addProperty("w", &KeyCodesArray[KeyCodesIndex::w], false)
				.addProperty("x", &KeyCodesArray[KeyCodesIndex::x], false)
				.addProperty("y", &KeyCodesArray[KeyCodesIndex::y], false)
				.addProperty("z", &KeyCodesArray[KeyCodesIndex::z], false)
				.addProperty("num_0", &KeyCodesArray[KeyCodesIndex::num_0], false)
				.addProperty("num_1", &KeyCodesArray[KeyCodesIndex::num_1], false)
				.addProperty("num_2", &KeyCodesArray[KeyCodesIndex::num_2], false)
				.addProperty("num_3", &KeyCodesArray[KeyCodesIndex::num_3], false)
				.addProperty("num_4", &KeyCodesArray[KeyCodesIndex::num_4], false)
				.addProperty("num_5", &KeyCodesArray[KeyCodesIndex::num_5], false)
				.addProperty("num_6", &KeyCodesArray[KeyCodesIndex::num_6], false)
				.addProperty("num_7", &KeyCodesArray[KeyCodesIndex::num_7], false)
				.addProperty("num_8", &KeyCodesArray[KeyCodesIndex::num_8], false)
				.addProperty("num_9", &KeyCodesArray[KeyCodesIndex::num_9], false)
				.addProperty("space", &KeyCodesArray[KeyCodesIndex::space], false)
				.addProperty("escape", &KeyCodesArray[KeyCodesIndex::escape], false)
				.addProperty("enter", &KeyCodesArray[KeyCodesIndex::enter], false)
				.addProperty("tab", &KeyCodesArray[KeyCodesIndex::tab], false)
				.addProperty("backspace", &KeyCodesArray[KeyCodesIndex::backspace], false)
				.addProperty("insert", &KeyCodesArray[KeyCodesIndex::insert], false)
				.addProperty("delete", &KeyCodesArray[KeyCodesIndex::del], false)
				.addProperty("right", &KeyCodesArray[KeyCodesIndex::right], false)
				.addProperty("left", &KeyCodesArray[KeyCodesIndex::left], false)
				.addProperty("up", &KeyCodesArray[KeyCodesIndex::up], false)
				.addProperty("down", &KeyCodesArray[KeyCodesIndex::down], false)
				.addProperty("left_shift", &KeyCodesArray[KeyCodesIndex::left_shift], false)
				.addProperty("left_ctrl", &KeyCodesArray[KeyCodesIndex::left_cntrl], false)
				.addProperty("left_alt", &KeyCodesArray[KeyCodesIndex::left_alt], false)
				.addProperty("right_shift", &KeyCodesArray[KeyCodesIndex::right_shift], false)
				.addProperty("right_ctrl", &KeyCodesArray[KeyCodesIndex::right_cntrl], false)
				.addProperty("right_alt", &KeyCodesArray[KeyCodesIndex::right_alt], false)
				.endNamespace() // End keys Namespace
#pragma endregion

				// Add Mouse Buttons
#pragma region MouseButtonCodes
				.beginNamespace("MouseButtons") // Create mouseButtons Namespace
				.addProperty("Mouse_1", &KeyCodesArray[KeyCodesIndex::mouse_1], false)
				.addProperty("Mouse_2", &KeyCodesArray[KeyCodesIndex::mouse_2], false)
				.addProperty("Mouse_3", &KeyCodesArray[KeyCodesIndex::mouse_3], false)
				.addProperty("Mouse_4", &KeyCodesArray[KeyCodesIndex::mouse_4], false)
				.addProperty("Mouse_5", &KeyCodesArray[KeyCodesIndex::mouse_5], false)
				.addProperty("Mouse_6", &KeyCodesArray[KeyCodesIndex::mouse_6], false)
				.addProperty("Mouse_7", &KeyCodesArray[KeyCodesIndex::mouse_7], false)
				.addProperty("Mouse_8", &KeyCodesArray[KeyCodesIndex::mouse_8], false)
				.addProperty("Mouse_Left", &KeyCodesArray[KeyCodesIndex::mouse_left], false)
				.addProperty("Mouse_Last", &KeyCodesArray[KeyCodesIndex::mouse_last], false)
				.addProperty("Mouse_Middle", &KeyCodesArray[KeyCodesIndex::mouse_middle], false)
				.addProperty("Mouse_Right", &KeyCodesArray[KeyCodesIndex::mouse_right], false)
				//.addProperty("Mouse_x", GetMouseX, false)
				//.addProperty("Mouse_y", GetMouseY, false)
				.endNamespace() // End mouseButtons Namespace
#pragma endregion

				// Add Input Polling Functions
#pragma region InputPollingFunctions
				.addFunction("KeyPressed", InputKeyPressed) // Add Key Pressed Function
				.addFunction("MousePressed", InputMouseButtonPressed) // Add Mouse Button Pressed Function
#pragma endregion

				// End input Namespace
				.endNamespace()
#pragma endregion

				// Add Raycasting
#pragma region Raycasting
				// Create RayHitInfo Class
#pragma region RayHitInfoClass
				.beginClass <RayHitInfo>("RaycastHit")
				.addProperty("DidHit", &RayHitInfo::hit)
				.addProperty("HitDistance", &RayHitInfo::hitDistance)
				.addProperty("ObjectName", &RayHitInfo::objectNameHit)
				.addProperty("ObjectTag", &RayHitInfo::tagHit)
				.endClass()
#pragma endregion

				// Add Raycasting Functionality
#pragma region RaycastingFunctions
				.beginNamespace("Raycasting") // Create raycasting Namespace
				.addFunction("CreateRayFromMe", CreateRayFromObject) // Function to Create a Ray From the Object
				.addFunction("CreateRayFromPoint", CreateRay) // Function to Create a Ray From a Given Point
				.addFunction("SetRayStartPoint", ChangeRayStartPoint) // Function to set the Start Point of the Ray
				.addFunction("GetRayStartPoint", GetRayStartPoint) // Function to get the Start Point of the Ray
				.addFunction("SetRayMovementVectorFromFloats", ChangeRayDistance) // Function to set the Direction/Distance of the Ray (uses a vector so has direction and distance)
				.addFunction("SetRayMovementVector", SetRayDistanceFromLuaVec) // Above function using LuaVec
				.addFunction("GetRayMovementVector", GetRayDistance) // Function to get the Ray's Movement as a Lua Vector
				.addFunction("GetRayLength", GetRayLength) // Function to set the Length of the Ray
				.addFunction("DisableRay", DisableRay) // Function to Disable a Ray
				.addFunction("EnableRay", EnableRay) // Enable Ray
				.addFunction("GetRayHitInfo", GetRayHitInfo) // Get the Hit Info for a Ray
				.addFunction("CastRayFromPoint", RayCastFromPoint) // Cast a ray From a Given Point
				.addFunction("CastRayFromMe", RayCastFromObject) // Cast a ray From an the Object
				.endNamespace() // End raycasting Namespace
#pragma endregion
#pragma endregion

				// Add Mesh/Model Functions
#pragma region ModelFunctions
				.beginNamespace("Model") // Create Model Namespace
				.addFunction("Set", SetObjectModel) // Add Set Model Function
				.addProperty("Cube", &sPremadeCubeModelFilepath) // Add Cube Model Filepath
				.addProperty("Capsule", &sPremadeCapsuleModelFilepath) // Add Capsule Model Filepath
				.addProperty("Cylinder", &sPremadeCylinderModelFilepath) // Add Cylinder Model Filepath
				.addProperty("Quad", &sPremadeQuadModelFilepath) // Add Quad Model Filepath
				.addProperty("Sphere", &sPremadeSphereModelFilepath) // Add Sphere Model Filepath
				.endNamespace() // End Model Namespace
#pragma endregion

				// Add Prefab Class
#pragma region AddPrefabClass
				.beginClass <ObjectPrefab>("ObjectPrefabTable")
				.addProperty("ObjectName", &ObjectPrefab::sObjectName)
				.addProperty("ObjectTag", &ObjectPrefab::sObjectTag)
				.addProperty("Model", &ObjectPrefab::sModelFilepath)
				.addProperty("Position", &ObjectPrefab::lvPosition)
				.addProperty("Rotation", &ObjectPrefab::lvRotation)
				.addProperty("Scale", &ObjectPrefab::lvScale)
				.addProperty("MaterialDiffuse", &ObjectPrefab::lcMaterialDiffuse)
				.addProperty("MaterialSpecular", &ObjectPrefab::lcMaterialSpecular)
				.addProperty("ShaderFilepath", &ObjectPrefab::sShaderFilepath)
				.addProperty("MatShininess", &ObjectPrefab::fMaterialShininess)
				.addProperty("TextureFilepath", &ObjectPrefab::sTextureFilepath)
				.addFunction("AddDynamicRigidBody", &ObjectPrefab::DefineDynamicRigidBodyComponent)
				.addFunction("AddStaticRigidBody", &ObjectPrefab::DefineStaticRigidBodyComponent)
				.addFunction("AddKinematicRigidBody", &ObjectPrefab::DefineKinematicRigidBodyComponent)
				.addFunction("AddBoxCollider", &ObjectPrefab::DefineBoxColliderComponent)
				.addFunction("AddSphereCollider", &ObjectPrefab::DefineBoxColliderComponent)
				.addFunction("AddCapsuleCollider", &ObjectPrefab::DefineBoxColliderComponent)
				.addFunction("AddConvexMeshCollider", &ObjectPrefab::DefineBoxColliderComponent)
				.addFunction("SetCollidables", &ObjectPrefab::DefineColliderCollidables)
				.addFunction("AddLuaScript", &ObjectPrefab::DefineLuaScriptComponent)
				.endClass()
#pragma endregion

				// Add Prefab Namespace
#pragma region PrefabNamespace
				.beginNamespace("Prefab") // Create Prefab Namespace
				.addFunction("Instantiate", CreateObjectFromPrefab) // Create From Prefab Function
				.addFunction("InstantiateAt", CreateObjectFromPrefabAtLocation) // Create Object From Prefab At a Given Location
				.addFunction("Blank", CreateNewPrefab) // Create a Blank Prefab
				.addFunction("Cube", CreateCubePrefab) // Create a Cube Prefab
				.addFunction("Capsule", CreateCapsulePrefab) // Create a Capsule Prefab
				.addFunction("Cylinder", CreateCylinderPrefab) // Create a Cylinder Prefab
				.addFunction("Quad", CreateQuadPrefab) // Create a Quad Prefab
				.addFunction("Sphere", CreateSpherePrefab) // Create a Sphere Prefab
				.endNamespace() // End Prefab Namespace
#pragma endregion

				// Add RigidBody Data?
#pragma region AddRigidBodyData
					// Add RidgidBody Functions
					.beginNamespace("RigidBody") // Create RigidBody Namespace
					.addFunction("ApplyForce", RigidBodyApplyForce) // Add Apply Force Function
					.addFunction("ApplyForceToPoint", RigidBodyApplyForceToPoint) // Add Apply Force Function
					.addFunction("ApplyTorque", RigidBodyApplyTorque) // Add Apply Force Function
					// Add Velocity Data
					.beginNamespace("Velocity") // Create velocity Namespace
					.addFunction("Set", RigidBodySetVelocity) // Add SetByFloats Velocity Function
					.addFunction("Get", GetVelocity) // Add Get Velocity Function
					.addProperty("x", RigidBodyGetVelocityX, RigidBodySetVelocityX) // Add Velocity X
					.addProperty("y", RigidBodyGetVelocityY, RigidBodySetVelocityY) // Add Velocity Y
					.addProperty("z", RigidBodyGetVelocityZ, RigidBodySetVelocityZ) // Add Velocity Z
					.beginNamespace("Angular")
					.addFunction("Set", RigidBodySetAngularVelocity) // Add Set Velocity Function
					.addFunction("Get", GetAngularVelocity) // Add Get Velocity Function
					.addProperty("x", RigidBodyGetAngularVelocityX, RigidBodySetAngularVelocityX) // Add Angular Velocity X
					.addProperty("y", RigidBodyGetAngularVelocityY, RigidBodySetAngularVelocityY) // Add Angular Velocity Y
					.addProperty("z", RigidBodyGetAngularVelocityZ, RigidBodySetAngularVelocityZ) // Add Angular Velocity Z
					.endNamespace() // End Angular namespace
					.endNamespace() // End velocity Namespace
#pragma endregion

				// Add Light Component Data?
#pragma region AddLightComponentData
				// Add Light Variables
				.beginNamespace("Light") // Create ridgidBody Namespace
				.beginNamespace("Ambient")
				.addProperty("r", GetLightAmbientX, SetLightAmbientX)
				.addProperty("g", GetLightAmbientY, SetLightAmbientY)
				.addProperty("b", GetLightAmbientZ, SetLightAmbientZ)
				.addFunction("Set", SetLightAmbientFromLuaCol)
				.addFunction("Get", GetLightAmbient)
				.endNamespace()
				.beginNamespace("PointDiffuse")
				.addProperty("r", GetLightPointDiffuseX, SetLightPointDiffuseX)
				.addProperty("g", GetLightPointDiffuseY, SetLightPointDiffuseY)
				.addProperty("b", GetLightPointDiffuseZ, SetLightPointDiffuseZ)
				.addFunction("Set", SetLightPointDiffuseFromLuaCol)
				.addFunction("Get", GetLightPointDiffuse)
				.endNamespace()
				.beginNamespace("PointSpecular")
				.addProperty("r", GetLightPointSpecularX, SetLightPointSpecularX)
				.addProperty("g", GetLightPointSpecularY, SetLightPointSpecularY)
				.addProperty("b", GetLightPointSpecularZ, SetLightPointSpecularZ)
				.addFunction("Set", SetLightPointSpecularFromLuaCol)
				.addFunction("Get", GetLightPointSpecular)
				.endNamespace()
				.beginNamespace("DirectionalDiffuse")
				.addProperty("r", GetLightDirectionalDiffuseX, SetLightDirectionalDiffuseX)
				.addProperty("g", GetLightDirectionalDiffuseY, SetLightDirectionalDiffuseY)
				.addProperty("b", GetLightDirectionalDiffuseZ, SetLightDirectionalDiffuseZ)
				.addFunction("Set", SetLightDirectionalDiffuseFromLuaCol)
				.addFunction("Get", GetLightDirectionalDiffuse)
				.endNamespace()
				.beginNamespace("DirectionalSpecular")
				.addProperty("r", GetLightDirectionalSpecularX, SetLightDirectionalSpecularX)
				.addProperty("g", GetLightDirectionalSpecularY, SetLightDirectionalSpecularY)
				.addProperty("b", GetLightDirectionalSpecularZ, SetLightDirectionalSpecularZ)
				.addFunction("Set", SetLightDirectionalSpecularFromLuaCol)
				.addFunction("Get", GetLightDirectionalSpecular)
				.endNamespace()
				.addProperty("Attenuation", GetLightAttenuation, SetLightAttenuation)
				.endNamespace()
#pragma endregion

				// Add Material Component Data?
#pragma region AddMaterialComponentData
				// Add RidgidBody Functions
				.beginNamespace("Material") // Create ridgidBody Namespace
				.beginNamespace("Diffuse")
				.addProperty("r", GetMaterialDiffuseX, SetMaterialDiffuseX)
				.addProperty("g", GetMaterialDiffuseY, SetMaterialDiffuseY)
				.addProperty("b", GetMaterialDiffuseZ, SetMaterialDiffuseZ)
				.addFunction("Set", SetMaterialDiffuseFromLuaCol)
				.addFunction("Get", GetMaterialDiffuse)
				.endNamespace()
				.beginNamespace("Specular")
				.addProperty("r", GetMaterialSpecularX, SetMaterialSpecularX)
				.addProperty("g", GetMaterialSpecularY, SetMaterialSpecularY)
				.addProperty("b", GetMaterialSpecularZ, SetMaterialSpecularZ)
				.addFunction("Set", SetMaterialSpecularFromLuaCol)
				.addFunction("Get", GetMaterialSpecular)
				.endNamespace()
				.addProperty("Shininess", GetMaterialShininess, SetMaterialShininess)
				.endNamespace()
#pragma endregion

				// Add Texture Component Data
#pragma region AddTextureComponentData
				// Add RidgidBody Functions
				.beginNamespace("Texture") // Create ridgidBody Namespace
				.addProperty("HeightScale", getTextureHeightScale, setTextureHeightScale)
				.endNamespace()
#pragma endregion

				// End Global Namespace
				;
		}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::LuaScript; }
	};
}