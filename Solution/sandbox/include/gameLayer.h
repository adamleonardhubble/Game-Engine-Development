/** \file gameLayer.h
*/
#pragma once

/**
\class GameLayer
\brief Layer for the game world things
*/
class GameLayer : public Engine::Layer
{
private:
	std::vector<std::shared_ptr<Engine::GameObject>> m_gameObjects; //!< Container of pointers to gameobjects
	std::vector<std::shared_ptr<Engine::MaterialComponent>> m_materials; //!< Container of pointers to material components
	std::vector<std::shared_ptr<Engine::PositionComponent>> m_positions; //!< Container of pointers to position components
	std::vector<std::shared_ptr<Engine::VelocityComponent>> m_velocities; //!< Container of pointers to velocity components
	std::vector<std::shared_ptr<Engine::OscilateComponent>> m_oscilate; //!< Container of pointers to oscilate components
	std::vector<std::shared_ptr<Engine::TextureSwitchComponent>> m_textureSwitch; //!< Container of pointers to texture switch components
	std::vector<std::shared_ptr<Engine::TextureComponent>> m_textures; //!< Container of pointers to texture components
	std::vector<std::shared_ptr<Engine::RotateComponent>> m_rotation; //!< Container of pointers to rotation components

	glm::vec3 m_lightColour; //!< Colour of the light in the scene
	glm::vec3 m_lightPosition; //!< Position of the light in the scene
	glm::vec3 m_viewPosition; //!< Where the light in the scene is being viewed from

	std::shared_ptr<Engine::UniformBuffer> m_MatUBO; //!< Matrices uniform buffer
	std::shared_ptr<Engine::UniformBuffer> m_LightUBO; //!< Light uniform buffer
public:
	//! Constructor
	/*!
	\param name The name of the layer
	*/
	GameLayer(const std::string& name = "Layer") : Layer(name) {}

	//! Function called when the layer is added
	void onAttach() override;
	//! Function called when the layer is removed
	void onDetach() override;
	//! Function to update the layer every frame
	/*!
	\param timestep The time since the previous frame
	*/
	void onUpdate(float timestep) override;
	//! Function to handle events
	/*!
	\param event The event
	*/
	void onEvent(Engine::Event& event) override;
};