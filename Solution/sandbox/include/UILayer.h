/** \file UILayer.h
*/
#pragma once

/**
\class UILayer
\brief Layer for 2D things
*/
class UILayer : public Engine::Layer
{
private:
	std::vector<std::shared_ptr<Engine::GameObject>> m_gameObjects; //!< Container of pointers to gameobjects
	std::vector<std::shared_ptr<Engine::MaterialComponent>> m_materials; //!< Container of pointers to material components
	std::vector<std::shared_ptr<Engine::TextComponent>> m_text; //!< Container of pointers to text components
	std::vector<std::shared_ptr<Engine::PositionComponent>> m_positions; //!< Container of pointers to position components
	std::vector<std::shared_ptr<Engine::TextureComponent>> m_textures; //!< Container of pointers to texture components

	std::shared_ptr<Engine::UniformBuffer> m_pUBO; //!< Matrices uniform buffer

	std::shared_ptr<Engine::Material> m_material;
public:
	//! Constructor
	/*!
	\param name The name of the layer
	*/
	UILayer(const std::string& name = "Layer") : Layer(name) {}

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