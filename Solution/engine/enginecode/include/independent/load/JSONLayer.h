/** \file JSONLayer.h
*/
#pragma once

#include "core/layer.h"

#include "components/CGO.h"
#include "components/materialComponent.h"
#include "components/positionComponent.h"
#include "components/velocityComponent.h"
#include "components/oscilateComponent.h"
#include "components/rotateComponent.h"
#include "components/textureComponent.h"
#include "components/textureSwitchComponent.h"
#include "components/textComponent.h"
#include "components/playerComponent.h"
#include "components/rigidBodyComponent.h"
#include "components/collisionShapes/boxCollisionShape.h"
#include "components/collisionShapes/sphereCollisionShape.h"
#include "components/collisionShapes/capsuleCollisionShape.h"
#include "components/collisionShapes/convexMeshShape.h"
#include "components/lightComponent.h"
#include "components/cameraComponent.h"
//#include "components/LuaScriptComponent.h"
#include "components/meshComponent.h"
#include "components/joints/jointComponent.h"

#include "systems/dynamicPhysicsSystem.h"

#include "rendering/frameBuffer.h"

#include "rendering/shader.h"

#include "systems/objectUpdateThread.h"

namespace Engine 
{
	template <class G>
	class LuaScriptComponent;

	/**
	\class JSONLayer
	\brief Class that creates a layer from a loaded JSON file
	*/
	class JSONLayer : public Layer
	{
	private:
		static std::shared_ptr<CameraComponent> s_currentCamera; //!< The current camera being used
		static std::shared_ptr<CameraComponent> s_current2DCamera; //!< The current 2D camera being used
	protected:
		std::vector<std::shared_ptr<GameObject>> m_gameObjects;			//!< Game objects which belong to the layer
		std::vector<std::shared_ptr<MaterialComponent>> m_materials;	//!< Material components
		std::vector<std::shared_ptr<MeshComponent>> m_meshes;	        //!< Mesh components
		std::vector<std::shared_ptr<TextComponent>> m_texts;            //!< Text components
		std::vector<std::shared_ptr<PositionComponent>> m_positions;	//!< Position components
		std::vector<std::shared_ptr<VelocityComponent>> m_velocities;	//!< Velocity components
		std::vector<std::shared_ptr<TextureComponent>> m_textures;	    //!< Texture components
		std::shared_ptr<PlayerComponent> m_player = nullptr;            //!< Player component
		std::vector<std::shared_ptr<CollisionComponent>> m_colliders;   //!< Collider components
		std::vector<std::shared_ptr<RigidBodyComponent>> m_rigidbodies; //!< Rigidbody components
		std::vector<std::shared_ptr<ControllerComponent>> m_controllers;//!< Controler components
		std::vector<std::shared_ptr<LightComponent>> m_lights;			//!< Controler components
		std::vector<std::shared_ptr<CameraComponent>> m_cameras;		//!< Controler components
		std::vector<std::shared_ptr<LuaScriptComponent<DynamicPhysicsSystem>>> m_LuaScripts;  //!< Lua Script Components
		std::vector<std::shared_ptr<JointComponent>> m_jointComponents; //!< Joint Components

		std::vector<std::shared_ptr<RenderCommand>> m_initCommands;					//!< Render commands issued on renderer start
		std::vector<std::shared_ptr<RenderCommand>> m_predrawFramebufferCommands;	//!< Render commands issued before materials are drawn
		std::vector<std::shared_ptr<RenderCommand>> m_postdrawFramebufferCommands;	//!< Render commands issued before materials are drawn
		std::vector<std::shared_ptr<RenderCommand>> m_predrawLayerTextureCommands;	//!< Render commands issued after materials are drawn
		std::vector<std::shared_ptr<RenderCommand>> m_postdrawLayerTextureCommands;	//!< Render commands issued after materials are drawn
		std::vector<std::shared_ptr<RenderCommand>> m_exitCommands;					//!< Render commands issued prior to renderer shutdown

		std::list<void*> m_data; //!< Misc data
		std::string m_filepath;	 //!< JSON filepath

		glm::mat4 m_camView;	 //!< The current cameras view
		glm::mat4 m_camProj;	 //!< The current cameras projection
		glm::mat4 m_camViewProj; //!< The current cameras view projecion
		glm::vec3 m_camPos;		 //!< The current cameras position

		std::shared_ptr<DynamicPhysicsSystem> m_physicsSystem; //!< A pointer to the physics system

		std::shared_ptr<ObjectUpdateThread> m_objectUpdateThread; //!< For updating object in separate threads

		#pragma region Frame Buffer
		static std::shared_ptr<Texture> s_pFBNightVisionNoise;	     //!< Frame buffer texture pointer
		static std::shared_ptr<Texture> s_pFBNightVisionMask;	     //!< Frame buffer texture pointer

		static std::shared_ptr<FrameBuffer> s_pFrameBuffer;	         //!< Shared pointer to the frame buffer class
		static std::shared_ptr<PPRenderer> s_pPPRenderer;            //!< Pointer to a post-processing renderer

		static std::shared_ptr<Shader> s_pFBShaderDefault;			 //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderInversed;			 //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderGrayscale;		 //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderSharpen;			 //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderBlur;				 //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderEdgeDetection;	 //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderNightVision;		 //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderMetalPalace;		 //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderDepth;			 //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderShakeConfuseChaos; //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderSimulatedFog;		 //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderCartoon;		     //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderDepthOfField;		 //!< Frame buffer shader pointer

		static std::shared_ptr<Shader> s_pFBShaderShadowDepthMap;    //!< Frame buffer shader pointer
		static std::shared_ptr<Shader> s_pFBShaderShadowLightSpace;  //!< Frame buffer shader pointer
		
		static std::shared_ptr<Shader> s_pFBShaderTest;	             //!< Frame buffer shader pointer

		static std::shared_ptr<LightComponent> s_pLight; //!< Light component pointer

		glm::mat4 m_lightSpaceMatrix; //!< Matrix(4x4), light space matrix used by depth map
		float m_fNearClippingPlane = 0.1f; //!< Depth map near clipping plane distance
		float m_fFarClippingPlane = 100.0f; //!< Depth map far clipping plane distance

		float m_fTimestep = 0.0f; //!< Timestep of the application updating
		
		int m_iShaderToggle = 0; //!< Counter used to determine active shader

		int m_iActiveEdgeDetection = 1; //!< Counter used to determine the active edge detection shader
		int m_iActiveSharpen = 1; //!< Counter used to determine the active sharpen shader
		int m_iActiveBlur = 1; //!< Counter used to determine the active blur shader
		int m_iActiveMetal = 1;	//!< Counter used to determine the active metal palace shader
		float m_fEffectCoverage = 1.0f; //!< Screen coverage of the night vision shader
		float m_fNightVisionTimestep = 0.0f; //!< Timestep used by night vision timestep
		float m_fShakeTimestep = 0.0f; //!< Timestep used by shake-related shaders
		float m_fCartoonRate = 1.0f; //!< Cartoon rate of the cartoon shader
		float m_fFocalDepth = 100.0f; //!< Focal depth of the camera frustum
		bool m_bFarDoF = false;	//!< Determine whether the DoF is set as near or far
		#pragma endregion

		static bool s_bDraw; //!< If needs to draw the texture with all layers on it
	public:
		//! Constructor
		/*!
		\param filepath The location of the JSON file
		\param physics A pointer to the physics system
		\param name The name of the layer
		*/
		JSONLayer(const std::string& filepath, std::shared_ptr<DynamicPhysicsSystem> physics, const std::string& name = "Layer")
			: Layer(name), m_filepath(filepath), m_physicsSystem(physics) {}	

		static void clearStatic()
		{
			s_currentCamera = nullptr;
			s_current2DCamera = nullptr;

			s_pFBNightVisionNoise = nullptr;
			s_pFBNightVisionMask = nullptr;

			s_pFrameBuffer = nullptr;
			s_pPPRenderer = nullptr;

			s_pFBShaderDefault = nullptr;
			s_pFBShaderInversed = nullptr;
			s_pFBShaderGrayscale = nullptr;
			s_pFBShaderSharpen = nullptr;
			s_pFBShaderBlur = nullptr;
			s_pFBShaderEdgeDetection = nullptr;
			s_pFBShaderNightVision = nullptr;
			s_pFBShaderMetalPalace = nullptr;
			s_pFBShaderDepth = nullptr;
			s_pFBShaderShakeConfuseChaos = nullptr;
			s_pFBShaderSimulatedFog = nullptr;
			s_pFBShaderCartoon = nullptr;
			s_pFBShaderDepthOfField = nullptr;

			s_pFBShaderShadowDepthMap = nullptr;
			s_pFBShaderShadowLightSpace = nullptr;

			s_pFBShaderTest = nullptr;

			s_pLight = nullptr;
		}

		//! Function to reset the frame buffer and make the material for the quad to render the frame buffer colour texture
		static void setupFrameBuffer();

		//! Runs when layer is attached
		void onAttach() override; 

		//! Runs when layer is detached
		void onDetach() override;

		//! Runs every frame
		/*!
		\param timestep The time since the previous frame
		*/
		void onUpdate(float timestep) override;

		//! Function to draw the texture that the layer is rendered to
		void drawLayer() override;

		//! Function to draw the objects in each layer
		void drawObjects() override;

		//! Runs when an event occurs
		/*!
		\param event The event that has occurred
		*/
		void onEvent(Event& event) override;

		//! Function to handle key press events
		/*!
		\param e The event
		\return If the event was successful or not
		*/
		bool onKeyPressed(KeyPressedEvent& e);

		//! Function to get the gameobjects in the layer
		/*!
		\return A vector containing shared pointers to all of the gameobjects
		*/
		inline std::vector<std::shared_ptr<GameObject>>& getGameObjects() { return m_gameObjects; }

		//! Function to get the material components in the layer
		/*!
		\return A vector containing shared pointers to all of the material components
		*/
		inline std::vector<std::shared_ptr<MaterialComponent>>& getMaterials() { return m_materials; }

		//! Function to get the mesh components in the layer
		/*!
		\return A vector containing shared pointers to all of the mesh components
		*/
		inline std::vector<std::shared_ptr<MeshComponent>>& getMeshes() { return m_meshes; }

		//! Function to get the text components in the layer
		/*!
		\return A vector containing shared pointers to all of the text components
		*/
		inline std::vector<std::shared_ptr<TextComponent>>& getTexts() { return m_texts; }

		//! Function to get the position components in the layer
		/*!
		\return A vector containing shared pointers to all of the position components
		*/
		inline std::vector<std::shared_ptr<PositionComponent>>& getPositions() { return m_positions; }

		//! Function to get the velocity components in the layer
		/*!
		\return A vector containing shared pointers to all of the velocity components
		*/
		inline std::vector<std::shared_ptr<VelocityComponent>>& getVelocities() { return m_velocities; }

		//! Function to get the texture components in the layer
		/*!
		\return A vector containing shared pointers to all of the texture components
		*/
		inline std::vector<std::shared_ptr<TextureComponent>>& getTextures() { return m_textures; }

		//! Function to get the player component in the layer
		/*!
		\return A shared pointer to the player component (if there is one)
		*/
		inline std::shared_ptr<PlayerComponent>& getPlayer() { return m_player; }

		//! Function to get the collider components in the layer
		/*!
		\return A vector containing shared pointers to all of the collider components
		*/
		inline std::vector<std::shared_ptr<CollisionComponent>>& getColliders() { return m_colliders; }

		//! Function to get the rigidbody components in the layer
		/*!
		\return A vector containing shared pointers to all of the rigidbody components
		*/
		inline std::vector<std::shared_ptr<RigidBodyComponent>>& getRigidbodies() { return m_rigidbodies; }

		//! Function to get the Lua Script components in the layer
		/*!
		\return A vector containing shared pointers to all of the Lua Script Components
		*/
		inline std::vector<std::shared_ptr<LuaScriptComponent<DynamicPhysicsSystem>>>& getLuaScripts() { return m_LuaScripts; }

		//! Function to get the controller components in the layer
		/*!
		\return A vector containing shared pointers to all of the controller components
		*/
		inline std::vector<std::shared_ptr<ControllerComponent>>& getControllers() { return m_controllers; }

		//! Function to get the joint components in the layer
		/*!
		\return A vector containing shared pointers to all of the joint components
		*/
		inline std::vector<std::shared_ptr<JointComponent>>& getJoints() { return m_jointComponents; }

		//! Function to get the light components in the layer
		/*!
		\return A vector containing shared pointers to all of the light components
		*/
		inline std::vector<std::shared_ptr<Engine::LightComponent>>& getLights() { return m_lights; }

		//! Function to get the camera components in the layer
		/*!
		\return A vector containing shared pointers to all of the camera components
		*/
		inline std::vector<std::shared_ptr<Engine::CameraComponent>>& getCameras() { return m_cameras; }

		//! Function to get the framebuffer predraw render commands in the layer
		/*!
		\return A vector containing shared pointers to all of the framebuffer predraw render commands
		*/
		inline std::vector<std::shared_ptr<RenderCommand>>& getPredrawFramebufferCommands() { return m_predrawFramebufferCommands; }

		//! Function to get the framebuffer postdraw render commands in the layer
		/*!
		\return A vector containing shared pointers to all of the framebuffer postdraw render commands
		*/
		inline std::vector<std::shared_ptr<RenderCommand>>& getPostdrawFramebufferCommands() { return m_postdrawFramebufferCommands; }

		//! Function to get the layer texture predraw render commands in the layer
		/*!
		\return A vector containing shared pointers to all of the layer texture predraw render commands
		*/
		inline std::vector<std::shared_ptr<RenderCommand>>& getPredrawLayerTextureCommands() { return m_predrawLayerTextureCommands; }

		//! Function to get the layer texture postdraw render commands in the layer
		/*!
		\return A vector containing shared pointers to all of the layer texture postdraw render commands
		*/
		inline std::vector<std::shared_ptr<RenderCommand>>& getPostdrawLayerTextureCommands() { return m_postdrawLayerTextureCommands; }

		//! Function to get the initialization render commands in the layer
		/*!
		\return A vector containing shared pointers to all of the initialization render commands
		*/
		inline std::vector<std::shared_ptr<RenderCommand>>& getInitCommands() { return m_initCommands; }

		//! Function to get the exit render commands in the layer
		/*!
		\return A vector containing shared pointers to all of the exit render commands
		*/
		inline std::vector<std::shared_ptr<RenderCommand>>& getExitCommands() { return m_exitCommands; }

		//! Function to get the misc data of the layer
		/*!
		\return A list of void pointers to data
		*/
		inline std::list<void*>& getData() { return m_data; }

		//! Function to get the cameras view
		/*!
		\return The current cameras view
		*/
		inline glm::mat4& getCamView() { return m_camView; }

		//! Function to get the cameras projection
		/*!
		\return The current cameras projection
		*/
		inline glm::mat4& getCamProj() { return m_camProj; }

		//! Function to get the cameras view projection
		/*!
		\return The current cameras view projection
		*/
		inline glm::mat4& getCamViewProj() { return m_camViewProj; }

		//! Function to get the cameras position
		/*!
		\return The current cameras position
		*/
		inline glm::vec3& getCamPos() { return m_camPos; }

		//! Function to get the pointer to the dynamic physics system
		/*!
		\return The pointer to the dynamic physics system
		*/
		inline std::shared_ptr<DynamicPhysicsSystem> getDynamicSystem() { return m_physicsSystem; }

		//! Function to set the current 3D camera being used
		/*!
		\param newCamera Pointer to the new camera
		*/
		static void setCurrentCamera(std::shared_ptr<CameraComponent> newCamera) { s_currentCamera = newCamera; }

		//! Function to set the current 2D camera being used
		/*!
		\param newCamera Pointer to the new camera
		*/
		static void setCurrent2DCamera(std::shared_ptr<CameraComponent> newCamera) { s_current2DCamera = newCamera; }
	};
}