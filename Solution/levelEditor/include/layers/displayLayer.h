/** \file displayLayer.h
*/
#pragma once

#include "data.h"

/**
\class DisplayLayer
\brief The layer with everything in the game
*/
class DisplayLayer : public Engine::Layer
{
private:
	std::vector<std::shared_ptr<Engine::GameObject>> m_gameObjects;				//!< Game objects
	std::vector<std::shared_ptr<Engine::GameObject>> m_2DGameObjects;			//!< 2D game objects
	std::vector<std::shared_ptr<Engine::MaterialComponent>> m_materials;		//!< Material components
	std::vector<std::shared_ptr<Engine::MeshComponent>> m_meshes;				//!< Mesh components
	std::vector<std::shared_ptr<Engine::TextComponent>> m_texts;				//!< Text components
	std::vector<std::shared_ptr<Engine::PositionComponent>> m_positions;		//!< Position components
	std::vector<std::shared_ptr<Engine::TextureComponent>> m_textures;			//!< Texture components
	std::vector<std::shared_ptr<Engine::LightComponent>> m_lights;				//!< Light components
	std::vector<std::shared_ptr<Engine::CameraComponent>> m_cameras;			//!< Camera components

	std::vector<std::shared_ptr<Engine::RenderCommand>> m_initCommands;			//!< Render commands issued on renderer start
	std::vector<std::shared_ptr<Engine::RenderCommand>> m_predrawCommands;		//!< Render commands issued before materials are drawn
	std::vector<std::shared_ptr<Engine::RenderCommand>> m_predrawUICommands;	//!< Render commands issued before UI is drawn
	std::vector<std::shared_ptr<Engine::RenderCommand>> m_postdrawCommands;		//!< Render commands issued after materials are drawn
	std::vector<std::shared_ptr<Engine::RenderCommand>> m_postdrawUICommands;	//!< Render commands issued after UI is drawn
	std::vector<std::shared_ptr<Engine::RenderCommand>> m_exitCommands;			//!< Render commands issued prior to renderer shutdown

	std::list<void*> m_data; //!< Misc data

	glm::mat4 m_camViewProj; //!< The current cameras view projecion
	glm::mat4 m_camView; //!< The current cameras view
	glm::mat4 m_camProj; //!< The current cameras projecion
	glm::vec3 m_camPos; //!< The current cameras position

	glm::mat4 m_orthoCamViewProj; //!< The current 2D cameras view projection

	unsigned int m_iSceneFrameBuffer; //!< The frame buffer ID for the view from the editor camera
	unsigned int m_iSceneRBO; //!< The render buffer ID for the view from the editor camera
	unsigned int m_iSceneTex; //!< The texture ID for the view from the editor camera

	std::shared_ptr<Engine::Material> m_pSkyBoxMat; //!< The material for the skybox
	std::shared_ptr<Engine::Texture> m_pSkyBoxTex; //!< The texture for the skybox

	bool m_bHoveringOnImage = false; //!< If the mouse is over the rendered image
	bool m_bMousePressed = false; //!< If a mouse button is pressed
public:
	LayerData m_layerData; //!< The object and joint data for the layer
	std::shared_ptr<Engine::FrameBuffer> m_pFrameBuffer; //!< Shared pointer to the frame buffer class
	float m_displayWindowMultiplier; //!< The multiplier for the display window size
	std::shared_ptr<Engine::Shader> m_pFBShaderShadowLightSpace; //!< The shadow depth shader
	glm::mat4 m_lightSpaceMatrix; //!< The light space matrix for rendering shadows

	//! Constructor
	/*!
	\param filepath The filepath of the layer
	\param name The name of the layer
	*/
	DisplayLayer(const std::string& filepath, const std::string& name = "Display Layer") : Layer(name)
	{
		m_layerData.m_filepath = filepath;
		m_layerData.m_name = m_name;
	}

	//! Function called when the layer is added
	void onAttach() override;

	//! Function called when the layer is removed
	void onDetach() override;

	//! Function to update the layer every frame
	/*!
	\param timestep The time since the previous frame
	*/
	void onUpdate(float timestep) override;

	void drawObjects() override {}

	//! Function to draw the texture that the layer is rendered to
	void drawLayer() override {}

	//! Function to handle events
	/*!
	\param event The event
	*/
	void onEvent(Engine::Event& event) override;
	//! Function for if a mouse button is pressed
	/*!
	\param event The event
	\return If the event was handled
	*/
	bool onMouseButtonPress(Engine::MouseButtonPressedEvent& event);
	//! Function for if a mouse button is released
	/*!
	\param event The event
	\return If the event was handled
	*/
	bool onMouseButtonRelease(Engine::MouseButtonReleasedEvent& event);
	//! Function for if the mouse is scrolled
	/*!
	\param event The event
	\return If the event was handled
	*/
	bool onMouseScrolled(Engine::MouseScrolledEvent& event);

	//! Function to get the gameobjects in the layer
	/*!
	\return A vector containing shared pointers to all of the gameobjects
	*/
	inline std::vector<std::shared_ptr<Engine::GameObject>>& getGameObjects() { return m_gameObjects; }
	//! Function to get the 2D gameobjects in the layer
	/*!
	\return A vector containing shared pointers to all of the 2D gameobjects
	*/
	inline std::vector<std::shared_ptr<Engine::GameObject>>& get2DGameObjects() { return m_2DGameObjects; }
	//! Function to get the material components in the layer
	/*!
	\return A vector containing shared pointers to all of the material components
	*/
	inline std::vector<std::shared_ptr<Engine::MaterialComponent>>& getMaterials() { return m_materials; }
	//! Function to get the mesh components in the layer
	/*!
	\return A vector containing shared pointers to all of the mesh components
	*/
	inline std::vector<std::shared_ptr<Engine::MeshComponent>>& getMeshes() { return m_meshes; }
	//! Function to get the text components in the layer
	/*!
	\return A vector containing shared pointers to all of the text components
	*/
	inline std::vector<std::shared_ptr<Engine::TextComponent>>& getTexts() { return m_texts; }
	//! Function to get the position components in the layer
	/*!
	\return A vector containing shared pointers to all of the position components
	*/
	inline std::vector<std::shared_ptr<Engine::PositionComponent>>& getPositions() { return m_positions; }
	//! Function to get the texture components in the layer
	/*!
	\return A vector containing shared pointers to all of the texture components
	*/
	inline std::vector<std::shared_ptr<Engine::TextureComponent>>& getTextures() { return m_textures; }
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

	//! Function to get the predraw render commands in the layer
	/*!
	\return A vector containing shared pointers to all of the predraw render commands
	*/
	inline std::vector<std::shared_ptr<Engine::RenderCommand>>& getPredrawCommands() { return m_predrawCommands; }
	//! Function to get the postdraw render commands in the layer
	/*!
	\return A vector containing shared pointers to all of the postdraw render commands
	*/
	inline std::vector<std::shared_ptr<Engine::RenderCommand>>& getPostdrawCommands() { return m_postdrawCommands; }
	//! Function to get the predraw render commands in the layer
	/*!
	\return A vector containing shared pointers to all of the predraw render commands
	*/
	inline std::vector<std::shared_ptr<Engine::RenderCommand>>& getPredrawUICommands() { return m_predrawUICommands; }
	//! Function to get the postdraw render commands in the layer
	/*!
	\return A vector containing shared pointers to all of the postdraw render commands
	*/
	inline std::vector<std::shared_ptr<Engine::RenderCommand>>& getPostdrawUICommands() { return m_postdrawUICommands; }
	//! Function to get the initialization render commands in the layer
	/*!
	\return A vector containing shared pointers to all of the initialization render commands
	*/
	inline std::vector<std::shared_ptr<Engine::RenderCommand>>& getInitCommands() { return m_initCommands; }
	//! Function to get the exit render commands in the layer
	/*!
	\return A vector containing shared pointers to all of the exit render commands
	*/
	inline std::vector<std::shared_ptr<Engine::RenderCommand>>& getExitCommands() { return m_exitCommands; }

	//! Function to get the misc data of the layer
	/*!
	\return A list of void pointers to data
	*/
	inline std::list<void*>& getData() { return m_data; }

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
	//! Function to get the 2D cameras view projection
	/*!
	\return The current 2D cameras view projection
	*/
	inline glm::mat4& getOrthoCamViewProj() { return m_orthoCamViewProj; }
};