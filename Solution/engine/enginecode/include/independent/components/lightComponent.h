/** \file lightComponent.h
*/
#pragma once

#include "CGO.h"

#include "rendering/uniformBuffer.h"
#include "rendering/renderer/renderer.h"

#include <glm/glm.hpp>

namespace Engine
{
	/**
	\class LightComponent
	\brief Component to give the scene a light
	*/
	class LightComponent : public Component
	{
	private:
		glm::vec3* m_ambient; //!< The ambient portion of the light

		glm::vec3* m_pointDiffuse; //!< The diffuse portion of the point light
		glm::vec3* m_pointSpecular; //!< The specular portion of the point light

		glm::vec3* m_dirDiffuse; //!< The diffuse portion of the directional light
		glm::vec3* m_dirSpecular; //!< The specular portion of the directional light

		glm::vec3* m_attenuation; //!< The attenuation of the light
	public:
		//! Constructor
		/*!
		\param ambient Pointer to the ambient light values to change
		\param pointDiffuse Pointer to the diffuse point light values to change
		\param pointSpecular Pointer to the specular point light values to change
		\param directionalDiffuse Pointer to the diffuse directional light values to change
		\param directionalSpecular Pointer to the specular directional light values to change
		\param attenuation Pointer to the attenuation light values to change
		*/
		LightComponent(glm::vec3* ambient, glm::vec3* pointDiffuse, glm::vec3* pointSpecular, glm::vec3* directionalDiffuse, glm::vec3* directionalSpecular,
			glm::vec3* attenuation) : m_ambient(ambient), m_pointDiffuse(pointDiffuse), m_pointSpecular(pointSpecular), m_dirDiffuse(directionalDiffuse),
			m_dirSpecular(directionalSpecular), m_attenuation(attenuation) {}

		//! Function to get the ambient light values for changing
		/*!
		\return The values of the light
		*/
		inline glm::vec3 getAmbient() { return *m_ambient; }
		// Function to get the ambient light's x value
		/*!
		\return the ambient x value
		*/
		inline float getAmbientX() { return m_ambient->x; }
		// Function to get the ambient light's y value
		/*!
		\return the ambient y value
		*/
		inline float getAmbientY() { return m_ambient->y; }
		// Function to get the ambient light's x value
		/*!
		\return the ambient x value
		*/
		inline float getAmbientZ() { return m_ambient->z; }
		//! Function to get the diffuse point light values for changing
		/*!
		\return The values of the light
		*/
		inline glm::vec3 getPointDiffuse() { return *m_pointDiffuse; }
		// Function to get the light's point diffuse x value
		/*!
		\return the point diffuse x value
		*/
		inline float getPointDiffuseX() { return m_pointDiffuse->x; }
		// Function to get the light's point diffuse y value
		/*!
		\return the point diffuse y value
		*/
		inline float getPointDiffuseY() { return m_pointDiffuse->y; }
		// Function to get the light's point diffuse z value
		/*!
		\return the point diffuse z value
		*/
		inline float getPointDiffuseZ() { return m_pointDiffuse->z; }
		//! Function to get the specular point light values for changing
		/*!
		\return The values of the light
		*/
		inline glm::vec3 getPointSpecular() { return *m_pointSpecular; }
		// Function to get the light's point specular x value
		/*!
		\return the point specular x value
		*/
		inline float getPointSpecularX() { return m_pointSpecular->x; }
		// Function to get the light's point specular y value
		/*!
		\return the point specular y value
		*/
		inline float getPointSpecularY() { return m_pointSpecular->y; }
		// Function to get the light's point specularzx value
		/*!
		\return the point specular z value
		*/
		inline float getPointSpecularZ() { return m_pointSpecular->z; }
		//! Function to get the diffuse directional light values for changing
		/*!
		\return The values of the light
		*/
		inline glm::vec3 getDirectionalDiffuse() { return *m_dirDiffuse; }
		// Function to get the light's directional diffuse x value
		/*!
		\return the directional diffuse x value
		*/
		inline float getDirectionalDiffuseX() { return m_dirDiffuse->x; }
		// Function to get the light's directional diffuse y value
		/*!
		\return the directional diffuse y value
		*/
		inline float getDirectionalDiffuseY() { return m_dirDiffuse->y; }
		// Function to get the light's directional diffuse z value
		/*!
		\return the directional diffuse z value
		*/
		inline float getDirectionalDiffuseZ() { return m_dirDiffuse->z; }
		//! Function to get the specular directional light values for changing
		/*!
		\return The values of the light
		*/
		inline glm::vec3 getDirectionalSpecular() { return *m_dirSpecular; }
		// Function to get the light's directional specular x value
		/*!
		\return the directional specular x value
		*/
		inline float getDirectionalSpecularX() { return m_dirSpecular->x; }
		// Function to get the light's directional specular y value
		/*!
		\return the directional specular y value
		*/
		inline float getDirectionalSpecularY() { return m_dirSpecular->y; }
		// Function to get the light's directional specular z value
		/*!
		\return the directional specular z value
		*/
		inline float getDirectionalSpecularZ() { return m_dirSpecular->z; }
		//! Function to get the attenuation value for changing
		/*!
		\return The linear atteuation value
		*/
		inline float getAttenuation() { return (*m_attenuation).y; }

		//! Function to set the ambient light values
		/*!
		\param newAmbient The new values
		*/
		inline void setAmbient(glm::vec3 newAmbient) { *m_ambient = newAmbient; }
		//! Function to set the ambient light's x value
		/*!
		\param newX the new x value
		*/
		inline void setAmbientX(float newX) { m_ambient->x = newX; }
		//! Function to set the ambient light's y value
		/*!
		\param newY the new y value
		*/
		inline void setAmbientY(float newY) { m_ambient->y = newY; }
		//! Function to set the ambient light's z value
		/*!
		\param newX the new z value
		*/
		inline void setAmbientZ(float newZ) { m_ambient->z = newZ; }
		//! Function to set the diffuse point light values
		/*!
		\param newDiffuse The new values
		*/
		inline void setPointDiffuse(glm::vec3 newDiffuse) { *m_pointDiffuse = newDiffuse; }
		//! Function to set the light's point diffuse x value
		/*!
		\param newX the new x value
		*/
		inline void setPointDiffuseX(float newX) { m_pointDiffuse->x = newX; }
		//! Function to set the light's point diffuse y value
		/*!
		\param newY the new y value
		*/
		inline void setPointDiffuseY(float newY) { m_pointDiffuse->y = newY; }
		//! Function to set the light's point diffuse z value
		/*!
		\param newX the new z value
		*/
		inline void setPointDiffuseZ(float newZ) { m_pointDiffuse->z = newZ; }
		//! Function to set the specular point light values
		/*!
		\param newSpecular The new values
		*/
		inline void setPointSpecular(glm::vec3 newSpecular) { *m_pointSpecular = newSpecular; }
		//! Function to set the light's point specular x value
		/*!
		\param newX the new x value
		*/
		inline void setPointSpecularX(float newX) { m_pointSpecular->x = newX; }
		//! Function to set the light's point specular y value
		/*!
		\param newY the new y value
		*/
		inline void setPointSpecularY(float newY) { m_pointSpecular->y = newY; }
		//! Function to set the light's point specular z value
		/*!
		\param newX the new z value
		*/
		inline void setPointSpecularZ(float newZ) { m_pointSpecular->z = newZ; }
		//! Function to set the diffuse directional light values
		/*!
		\param newDiffuse The new values
		*/
		inline void setDirectionalDiffuse(glm::vec3 newDiffuse) { *m_dirDiffuse = newDiffuse; }
		//! Function to set the light's directional diffuse x value
		/*!
		\param newX the new x value
		*/
		inline void setDirectionalDiffuseX(float newX) { m_dirDiffuse->x = newX; }
		//! Function to set the light's directional diffuse y value
		/*!
		\param newY the new y value
		*/
		inline void setDirectionalDiffuseY(float newY) { m_dirDiffuse->y = newY; }
		//! Function to set the light's directional diffuse z value
		/*!
		\param newX the new z value
		*/
		inline void setDirectionalDiffuseZ(float newZ) { m_dirDiffuse->z = newZ; }
		//! Function to set the specular directional light values
		/*!
		\param newSpecular The new values
		*/
		inline void setDirectionalSpecular(glm::vec3 newSpecular) { *m_dirSpecular = newSpecular; }
		//! Function to set the light's directional diffuse x value
		/*!
		\param newX the new x value
		*/
		inline void setDirectionalSpecularX(float newX) { m_dirSpecular->x = newX; }
		//! Function to set the light's directional diffuse y value
		/*!
		\param newY the new y value
		*/
		inline void setDirectionalSpecularY(float newY) { m_dirSpecular->y = newY; }
		//! Function to set the light's directional diffuse z value
		/*!
		\param newX the new z value
		*/
		inline void setDirectionalSpecularZ(float newZ) { m_dirSpecular->z = newZ; }
		//! Function to set the attenuation values
		/*!
		\param newAttenuation The new linear attenustion value
		*/
		void setAttenuation(float newAttenuation) { *m_attenuation = glm::vec3(1.0, newAttenuation, (newAttenuation / 100.f) * 3.5f); }

		//! Function to receive a message from another component on the gameobject
		/*!
		\param msg The message being received
		*/
		void receiveMessage(const ComponentMessage& msg) override {}

		//! Function to get the component type for comparison
		/*!
		\return The type that the component is
		*/
		static ComponentType getStaticComponentType() { return ComponentType::Light; }
	};
}