/** \file data.h
*/
#pragma once

/*extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}*/

/**
\class ColliderData
\brief Values used to change the collider on an object
*/
class ColliderData
{
public:
	std::string m_shape = "box"; //!< The shape of the collider

	bool m_useScaleForSize = true; //!< If the collider size is the same as the object scale

	float m_radius = 0; //!< The radius if the collider is round
	float m_height = 0; //!< The height if the collider is a capsule
	glm::vec3 m_halfExtents = glm::vec3(0.f); //!< The half extents if the collider is a box

	std::string m_modelFilepath = ""; //!< The model filepath for if the collider uses a model

	unsigned int m_category = 1; //!< The collision category that the collider is part of
	bool m_collideCat[4] = { true, true, true, true }; //!< A bool for if the collider can collide with each category

	float m_mass = 1; //!< The mass for the collider
};

/**
\class RigidBodyData
\brief Values used to change the rigidbody on an object
*/
class RigidBodyData
{
public:
	std::string m_type = "dynamic"; //!< The type of the rigidbody

	bool m_gravity = true; //!< If the body uses gravity
	float m_bounce = 0; //!< The bouce value
	float m_friction = 0; //!< The friction value
	float m_rollResist = 0; //!< The roll resistance value
};

class LuaScriptData
{
	// Public Members
public:
	// Public Attributes
	std::string m_sScriptFilePath = "";

	bool m_makingNewScript = false;
	char m_newName[200];
	//std::string m_sFunctionName;

	// Public Functions
};

/**
\class Joint
\brief Values used to change a joint
*/
class Joint
{
public:
	int m_jointVectorValue; //!< Vector index value for the joint vector in LayerData

	std::string m_type = "Ball and Socket"; //!< The type of joint

	glm::vec3 m_position = glm::vec3(0); //!< The position of the joint

	std::string m_bodyAName = ""; //!< The name of one of the objects it's connecting
	std::string m_bodyBName = ""; //!< The name of the other object it's connecting

	std::string m_layer = "Default3D"; //!< The layer it's in
	bool m_in3DLayer = true; //!< If it's in a 3D layer

	bool m_bBodiesCanCollide = true; //!< If the bodies on the joint can collide with each other

	bool m_bIsLimited = false; //!< If the joint movement is limited
	float m_fMaxLimit = 0; //!< The max value for the limit
	float m_fMinLimit = 0; //!< The min value for the limit

	glm::vec3 m_axis = glm::vec3(0); //!< The axis for joints that only move on one axis
};

/**
\class TempTextData
\brief Temporary data when changing a text object
*/
class TempTextData
{
public:
	int m_size;
	char m_text[100];
	std::string m_font;
};

/**
\class Object
\brief Class containing data for all of the objects in the level editor
*/
class Object
{
public:
	int m_objectVectorValue; //!< Vector index value for the object vector in LayerData and the object in the display layer

	bool m_hasText = false;
	int m_textComponentVectorValue = 0;
	std::string m_textFont = "";
	int m_textCharSize = 0;
	TempTextData m_tempTexData;

	int m_positionComponentVectorValue = 0; //!< Vector index value for the position component in the display layer

	bool m_hasMat = false; //!< If the object has a material
	int m_materialComponentVectorValue = 0; //!< Vector index value for the material component in the display layer
	std::string m_shaderFilepath = ""; //!< The filepath of the shader used
	std::string m_tempShaderFilepath = ""; //!< A temporary filepath for when running the game

	bool m_hasMesh = false; //!< If the objects has a mesh
	int m_meshComponentVectorValue = 0; //!< Vector index value for the mesh component in the display layer
	std::string m_modelFilepath = ""; //!< The filepath of the model used

	bool m_hasLight = false; //!< If the object has a light
	int m_lightComponentVectorValue = 0; //!< Vector index value for the light component in the display layer

	bool m_hasCamera = false; //!< If the object has a camera
	int m_cameraComponentVectorValue = 0; //!< Vector index value for the camera component in the display layer
	std::string m_cameraType = ""; //!< The type of the camera

	bool m_hasCollider = false; //!< If the object has a collider
	ColliderData m_collider; //!< The data used for the collider
	bool m_hasRigidbody = false; //!< If the object has a rigidbody
	RigidBodyData m_rigidBody; //!< The data used for the rigidbody

	bool m_hasTexture = false; //!< If the object has a texture
	int m_textureComponentVectorValue = 0; //!< Vector index value for the texture component in the display layer
	std::string m_diffuseTextureFilepath = "assets/textures/None.png"; //!< The filepath of the diffuse texture used
	std::string m_specularTextureFilepath = "assets/textures/None.png"; //!< The filepath of the specular texture used
	bool m_hasNormalMap = false; //!< If the object has a normal map
	std::string m_normalMapFilepath = "assets/textures/None.png"; //!< The filepath of the normal map used
	bool m_hasDepthMap = false; //!< If the object has a depth map
	std::string m_depthMapFilepath = "assets/textures/None.png"; //!< The filepath of the depth map used

	bool bHasLuaScript = false; //!< If the object has a lua script
	LuaScriptData m_LuaScript; //!< The data used for the lua script

	std::string m_layer = "Default3D"; //!< The layer the object is in
	bool m_in3DLayer = true; //!< If the object is in a 3D layer
	std::string m_tag = "Default"; //!< The tag of the thing
	
	//! Default constructor
	Object() {}
	//! Constructor
	/*!
	\param vectorValue The object vector value
	*/
	Object(int vectorValue) : m_objectVectorValue(vectorValue) {}
};

/**
\class LayerData
\brief Data for the layer showing (everything in the display layer), the level
*/
class LayerData
{
public:
	std::string m_name = "Default Layer"; //!< Name of the layer
	std::string m_filepath = "assets/savedJSON/Default/Default3DLayer.json"; //!< The filepath of the layer

	std::vector<Object> m_objects; //!< The objects in the layer
	std::vector<Object> m_2DObjects;
	std::vector<Joint> m_joints; //!< The joints in the layer
	int m_numColliders = 0; //!< The number of colliders in the layer
	int m_numRigidbodies = 0; //!< The number of rigidbodies in the layer

	int m_numDifferentModels = 0; //!< The number of models in the layer

	int m_INumOfLuaScripts = 0; //!< The number of lua scripts in the layer

	std::map<std::string, std::vector<int>> m_fontsAndSizes;
	std::unordered_multimap<std::string, unsigned int> m_fonts;

	std::vector<std::string> m_3DLayers; //!< The 3D layers in the level
	std::vector<std::string> m_2DLayers; //!< The 2D layers in the level
	std::vector<std::string> m_tags; //!< Tags for in the level
	std::string m_levelName = "Default"; //!< The name of the level
};