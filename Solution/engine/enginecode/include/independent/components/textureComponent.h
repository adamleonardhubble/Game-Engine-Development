/** \file textureComponent.h
*/
#pragma once

#include "CGO.h"

#include "rendering/texture.h"

namespace Engine
{
	/**
	\class TextureComponent
	\brief Component to give a gameobject a texture
	*/
	class TextureComponent : public Component
	{
	private:
		std::shared_ptr<Texture> m_diffuseTex; //!< Pointer to the diffuse texture
		std::shared_ptr<Texture> m_specularTex; //!< Pointer to the specular texture
		std::shared_ptr<Texture> m_normalMap; //!< Pointer to the normal map
		std::shared_ptr<Texture> m_depthMap; //!< Pointer to the depth map

		unsigned int m_iDiffuseTexSlot; //!< The diffuse texture slot
		unsigned int m_iSpecularTexSlot; //!< The specular texture slot
		unsigned int m_iNormalMapSlot; //!< The normal map texture slot
		unsigned int m_iDepthMapSlot; //!< The depth map texture slot

		bool m_bHasNormalMap; //!< If there is a normal map or not
		bool m_bHasDepthMap; //!< If there is a depth map or not
		float m_fDepthMapHeightScale; //!< The height scale for calculating parallax mapping

		//! Function to send a message to another component on the gameobject
		/*!
		\param msg The message being sent
		*/
		void sendMessage(const ComponentMessage& msg) override;
	public:
		//! Constructor
		/*!
		\param diffuseTex Pointer to the diffuse texture
		\param specularTex Pointer to the specular texture
		*/
		TextureComponent(std::shared_ptr<Texture> diffuseTex, std::shared_ptr<Texture> specularTex) : m_diffuseTex(diffuseTex), m_specularTex(specularTex) 
		{
			m_bHasNormalMap = false;
			m_bHasDepthMap = false;
			m_fDepthMapHeightScale = 0;

			m_componentType = ComponentType::Texture; // Set the component type
		}
		//! Constructor
		/*!
		\param diffuseTex Pointer to the diffuse texture
		\param specularTex Pointer to the specular texture
		\param normalMap Pointer to the normal map
		*/
		TextureComponent(std::shared_ptr<Texture> diffuseTex, std::shared_ptr<Texture> specularTex, std::shared_ptr<Texture> normalMap) : 
			m_diffuseTex(diffuseTex), m_specularTex(specularTex), m_normalMap(normalMap)
		{
			m_bHasNormalMap = true;
			m_bHasDepthMap = false;
			m_fDepthMapHeightScale = 0;

			m_componentType = ComponentType::Texture; // Set the component type
		}
		//! Constructor
		/*!
		\param diffuseTex Pointer to the diffuse texture
		\param specularTex Pointer to the specular texture
		\param depthMap Pointer to the normal map
		*/
		TextureComponent(std::shared_ptr<Texture> diffuseTex, std::shared_ptr<Texture> specularTex, std::shared_ptr<Texture> depthMap, float heightScale) :
			m_diffuseTex(diffuseTex), m_specularTex(specularTex), m_depthMap(depthMap), m_fDepthMapHeightScale(heightScale)
		{
			m_bHasNormalMap = false;
			m_bHasDepthMap = true;

			m_componentType = ComponentType::Texture; // Set the component type
		}
		//! Constructor
		/*!
		\param diffuseTex Pointer to the diffuse texture
		\param specularTex Pointer to the specular texture
		\param normalMap Pointer to the normal map
		\param depthMap Pointer to the depth map
		\param heightScale The height scale value
		*/
		TextureComponent(std::shared_ptr<Texture> diffuseTex, std::shared_ptr<Texture> specularTex, std::shared_ptr<Texture> normalMap, 
			std::shared_ptr<Texture> depthMap, float heightScale) :
			m_diffuseTex(diffuseTex), m_specularTex(specularTex), m_normalMap(normalMap), m_depthMap(depthMap), m_fDepthMapHeightScale(heightScale)
		{
			m_bHasNormalMap = true;
			m_bHasDepthMap = true;

			m_componentType = ComponentType::Texture; // Set the component type
		}

		//! Function to set the diffuse texture of an object
		/*!
		\param diffuseTex The new texture
		*/
		void setDiffuse(std::shared_ptr<Texture> diffuseTex);
		//! Function to get the diffuse texture of an object
		/*!
		\return The diffuse texture
		*/
		
		std::shared_ptr<Texture> getTexture() { return m_diffuseTex; }
		//! Function to set the specular texture of an object
		/*!
		\param specularTex The new texture
		*/
		void setSpecular(std::shared_ptr<Texture> specularTex);
		//! Function to set the normal map of an object
		/*!
		\param hasNormalMap If the object is being given a normal map
		\param normalMap The new texture
		*/
		void setNormalMap(bool hasNormalMap, std::shared_ptr<Texture> normalMap);
		//! Function to set the depth map of an object
		/*!
		\param hasDepthMap If the object is being given a depth map
		\param depthMap The new texture
		*/
		void setDepthMap(bool hasDepthMap, std::shared_ptr<Texture> depthMap);
		//! Function to get the height scale for the depth map
		/*!
		\return The height scale
		*/
		float getHeightScale() { return m_fDepthMapHeightScale; }
		//! Function to set the height scale for the depth map
		/*!
		\param heightScale The new value
		*/
		void setHeightScale(float heightScale);

		//! Function called when the component is attached to a gameobject
		/*!
		\param owner Pointer to the gameobject the component is being attached to
		*/
		void onAttach(GameObject* owner) override;
		//! Function to update the component every frame
		/*!
		\param timestep The time since the previous frame
		*/
		void onUpdate(float timestep) override;
		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override {}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::Texture; }
	};
}