/** \file textureSwitchComponent.cpp
*/
#include "engine_pch.h"

#include "components/textureSwitchComponent.h"
#include "components/oscilateComponent.h"

namespace Engine
{
	void TextureSwitchComponent::onAttach(GameObject* owner)
	{
		// Set the owner
		m_pOwner = owner;
		// Set the texture slot
		m_iTexSlot = m_tex1->getSlot();
		// Send the texture slot with the uniform name to upload
		std::pair<std::string, void*> data("u_diffuseTex", (void*)&m_iTexSlot);
		ComponentMessage msg(ComponentMessageType::UniformSet, (void*)&data);
		sendMessage(msg);

		std::pair<std::string, void*> data2("u_specTex", (void*)&m_iTexSlot);
		ComponentMessage msg2(ComponentMessageType::UniformSet, (void*)&data2);
		sendMessage(msg2);

		bool hasNormal = false;
		std::pair<std::string, void*> data3("u_hasNormalMap", (void*)&hasNormal);
		ComponentMessage msg3(ComponentMessageType::UniformSet, (void*)&data3);
		sendMessage(msg3);

		bool hasDepth = false;
		std::pair<std::string, void*> data4("u_hasDepthMap", (void*)&hasDepth);
		ComponentMessage msg4(ComponentMessageType::UniformSet, (void*)&data4);
		sendMessage(msg4);
	}

	void TextureSwitchComponent::onUpdate(float timestep)
	{
		// Send the texture slot with the uniform name to upload
		std::pair<std::string, void*> data("u_diffuseTex", (void*)&m_iTexSlot);
		ComponentMessage msg(ComponentMessageType::UniformSet, (void*)&data);
		sendMessage(msg);

		std::pair<std::string, void*> data2("u_specTex", (void*)&m_iTexSlot);
		ComponentMessage msg2(ComponentMessageType::UniformSet, (void*)&data2);
		sendMessage(msg2);

		bool hasNormal = false;
		std::pair<std::string, void*> data3("u_hasNormalMap", (void*)&hasNormal);
		ComponentMessage msg3(ComponentMessageType::UniformSet, (void*)&data3);
		sendMessage(msg3);

		bool hasDepth = false;
		std::pair<std::string, void*> data4("u_hasDepthMap", (void*)&hasDepth);
		ComponentMessage msg4(ComponentMessageType::UniformSet, (void*)&data4);
		sendMessage(msg4);
	}

	void TextureSwitchComponent::receiveMessage(const ComponentMessage& msg)
	{
		// Create message dispatcher
		MessageDispatcher dispatcher(msg);
		// If the message type matches, call the corresponding function
		dispatcher.dispatch(ComponentMessageType::TextureSet, std::bind(&TextureSwitchComponent::textureSet, this, std::placeholders::_1));
	}

	void TextureSwitchComponent::textureSet(void* data)
	{
		OscilateComponent::state tex = *(OscilateComponent::state*)data; // Get the data from the message

			// If the state is up
		if (tex == OscilateComponent::state::UP)
		{
			// Make the current texture slot the one from the first texture
			m_iTexSlot = m_tex1->getSlot();
		}
		else if (tex == OscilateComponent::state::DOWN) // If the state is down
		{
			// Make the current texture slot the one from the second texture
			m_iTexSlot = m_tex2->getSlot();
		}
	}
}