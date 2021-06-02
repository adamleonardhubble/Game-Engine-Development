/** \file textureComponent.cpp
*/
#include "engine_pch.h"

#include "components/textureComponent.h"

namespace Engine
{
	void TextureComponent::setDiffuse(std::shared_ptr<Texture> diffuseTex)
	{ 
		m_diffuseTex = diffuseTex; 
		m_iDiffuseTexSlot = m_diffuseTex->getSlot();

		// Send the diffuse texture slot with the uniform name to upload
		std::pair<std::string, void*> data("u_diffuseTex", (void*)&m_iDiffuseTexSlot);
		ComponentMessage msg(ComponentMessageType::UniformSet, (void*)&data);
		sendMessage(msg);
	}

	void TextureComponent::setSpecular(std::shared_ptr<Texture> specularTex)
	{ 
		m_specularTex = specularTex; 
		m_iSpecularTexSlot = m_specularTex->getSlot(); 

		// Send the specular texture slot with the uniform name to upload
		std::pair<std::string, void*> data("u_specTex", (void*)&m_iSpecularTexSlot);
		ComponentMessage msg(ComponentMessageType::UniformSet, (void*)&data);
		sendMessage(msg);
	}

	void TextureComponent::setNormalMap(bool hasNormalMap, std::shared_ptr<Texture> normalMap)
	{
		m_bHasNormalMap = hasNormalMap;

		// Send if there is a normal map with the uniform name to upload
		std::pair<std::string, void*> data("u_hasNormalMap", (void*)&m_bHasNormalMap);
		ComponentMessage msg(ComponentMessageType::UniformSet, (void*)&data);
		sendMessage(msg);

		if (m_bHasNormalMap)
		{
			m_normalMap = normalMap;
			m_iNormalMapSlot = m_normalMap->getSlot();

			// Send the normal map slot with the uniform name to upload
			std::pair<std::string, void*> data2("u_normalMap", (void*)&m_iNormalMapSlot);
			ComponentMessage msg2(ComponentMessageType::UniformSet, (void*)&data2);
			sendMessage(msg2);
		}
	}

	void TextureComponent::setDepthMap(bool hasDepthMap, std::shared_ptr<Texture> depthMap)
	{
		m_bHasDepthMap = hasDepthMap;

		// Send if there is a depth map with the uniform name to upload
		std::pair<std::string, void*> data("u_hasDepthMap", (void*)&m_bHasDepthMap);
		ComponentMessage msg(ComponentMessageType::UniformSet, (void*)&data);
		sendMessage(msg);

		if (m_bHasDepthMap)
		{
			m_depthMap = depthMap;
			m_iDepthMapSlot = m_depthMap->getSlot();

			// Send the depth map slot with the uniform name to upload
			std::pair<std::string, void*> data2("u_depthMap", (void*)&m_iDepthMapSlot);
			ComponentMessage msg2(ComponentMessageType::UniformSet, (void*)&data2);
			sendMessage(msg2);
		}
	}

	void TextureComponent::setHeightScale(float heightScale)
	{
		m_fDepthMapHeightScale = heightScale;

		// Send the height scale with the uniform name to upload
		std::pair<std::string, void*> data("u_heightScale", (void*)&m_fDepthMapHeightScale);
		ComponentMessage msg(ComponentMessageType::UniformSet, (void*)&data);
		sendMessage(msg);
	}

	void TextureComponent::onAttach(GameObject* owner)
	{
		// Set the owner
		m_pOwner = owner;
		// Set the texture slot
		m_iDiffuseTexSlot = m_diffuseTex->getSlot();
		m_iSpecularTexSlot = m_specularTex->getSlot();
		// Send the diffuse texture slot with the uniform name to upload
		std::pair<std::string, void*> data("u_diffuseTex", (void*)&m_iDiffuseTexSlot);
		ComponentMessage msg(ComponentMessageType::UniformSet, (void*)&data);
		sendMessage(msg);

		// Send the specular texture slot with the uniform name to upload
		std::pair<std::string, void*> data2("u_specTex", (void*)&m_iSpecularTexSlot);
		ComponentMessage msg2(ComponentMessageType::UniformSet, (void*)&data2);
		sendMessage(msg2);

		// Send if there is a normal map with the uniform name to upload
		std::pair<std::string, void*> data3("u_hasNormalMap", (void*)&m_bHasNormalMap);
		ComponentMessage msg3(ComponentMessageType::UniformSet, (void*)&data3);
		sendMessage(msg3);

		if (m_bHasNormalMap)
		{
			m_iNormalMapSlot = m_normalMap->getSlot();
			// Send the normal map slot with the uniform name to upload
			std::pair<std::string, void*> data4("u_normalMap", (void*)&m_iNormalMapSlot);
			ComponentMessage msg4(ComponentMessageType::UniformSet, (void*)&data4);
			sendMessage(msg4);
		}

		// Send if there is a depth map with the uniform name to upload
		std::pair<std::string, void*> data5("u_hasDepthMap", (void*)&m_bHasDepthMap);
		ComponentMessage msg5(ComponentMessageType::UniformSet, (void*)&data5);
		sendMessage(msg5);

		if (m_bHasDepthMap)
		{
			m_iDepthMapSlot = m_depthMap->getSlot();
			// Send the depth map slot with the uniform name to upload
			std::pair<std::string, void*> data6("u_depthMap", (void*)&m_iDepthMapSlot);
			ComponentMessage msg6(ComponentMessageType::UniformSet, (void*)&data6);
			sendMessage(msg6);

			// Send the height scale with the uniform name to upload
			std::pair<std::string, void*> data7("u_heightScale", (void*)&m_fDepthMapHeightScale);
			ComponentMessage msg7(ComponentMessageType::UniformSet, (void*)&data7);
			sendMessage(msg7);
		}
	}
	
	void TextureComponent::onUpdate(float timestep)
	{
		// Send the diffuse texture slot with the uniform name to upload
		std::pair<std::string, void*> data("u_diffuseTex", (void*)&m_iDiffuseTexSlot);
		ComponentMessage msg(ComponentMessageType::UniformSet, (void*)&data);
		sendMessage(msg);

		// Send the specular texture slot with the uniform name to upload
		std::pair<std::string, void*> data2("u_specTex", (void*)&m_iSpecularTexSlot);
		ComponentMessage msg2(ComponentMessageType::UniformSet, (void*)&data2);
		sendMessage(msg2);

		// Send if there is a normal map with the uniform name to upload
		std::pair<std::string, void*> data3("u_hasNormalMap", (void*)&m_bHasNormalMap);
		ComponentMessage msg3(ComponentMessageType::UniformSet, (void*)&data3);
		sendMessage(msg3);

		if (m_bHasNormalMap)
		{
			// Send the normal map slot with the uniform name to upload
			std::pair<std::string, void*> data4("u_normalMap", (void*)&m_iNormalMapSlot);
			ComponentMessage msg4(ComponentMessageType::UniformSet, (void*)&data4);
			sendMessage(msg4);
		}

		// Send if there is a depth map with the uniform name to upload
		std::pair<std::string, void*> data5("u_hasDepthMap", (void*)&m_bHasDepthMap);
		ComponentMessage msg5(ComponentMessageType::UniformSet, (void*)&data5);
		sendMessage(msg5);

		if (m_bHasDepthMap)
		{
			// Send the depth map slot with the uniform name to upload
			std::pair<std::string, void*> data6("u_depthMap", (void*)&m_iDepthMapSlot);
			ComponentMessage msg6(ComponentMessageType::UniformSet, (void*)&data6);
			sendMessage(msg6);

			// Send the height scale with the uniform name to upload
			std::pair<std::string, void*> data7("u_heightScale", (void*)&m_fDepthMapHeightScale);
			ComponentMessage msg7(ComponentMessageType::UniformSet, (void*)&data7);
			sendMessage(msg7);
		}
	}

	void TextureComponent::sendMessage(const ComponentMessage& msg)
	{
		// Send the message to all components on the object
		for (auto it = m_pOwner->begin(); it != m_pOwner->end(); it++)
		{
			(*it)->receiveMessage(msg);
		}
	}
}