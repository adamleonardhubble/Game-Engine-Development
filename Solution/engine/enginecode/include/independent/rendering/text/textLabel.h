/** \file textLabel.h
*/
#pragma once

#include "rendering/material.h"
#include "systems/resourceManager.h"
#include "rendering/model.h"

#include <glm/glm.hpp>

namespace Engine
{
	/**
	\class TextLabel
	\brief Base class for allowing to render more than one character at once
	*/
	class TextLabel
	{
	public:
		//! Function to set the font of the text
		/*!
		\param fontName The name of the new font
		*/
		virtual bool setFont(const std::string& fontName) = 0;
		//! Function to set the character size of the text
		/*!
		\param charSize The new size
		*/
		virtual bool setCharSize(unsigned int charSize) = 0;
		//! Function to set the text being shown
		/*!
		\param text The new text to show
		*/
		virtual void setText(const std::string& text) = 0;
		//! Function to set the position of the text
		/*!
		\param position The new position
		*/
		virtual void setPosition(const glm::vec2& position) = 0;
		//! Function to set the orientation of the text
		/*!
		\param angle The new angle
		*/
		virtual void setOrientation(float angle) = 0;
		//! Function to set the scale of the text
		/*!
		\param scale The new scale of the text
		*/
		virtual void setScale(float scale) = 0;
		//! Function to set the colour of the text
		/*!
		\param colour The new colour
		*/
		virtual void setColour(const glm::vec3& colour) = 0;
		//! Function to get the font
		/*!
		\return The font
		*/
		virtual const std::string& getFont() const = 0;
		//! Function to get the character size
		/*!
		\return The charactersize
		*/
		virtual unsigned int getCharSize() const = 0;
		//! Function to get the text
		/*!
		\return The text
		*/
		virtual const std::string& getText() const = 0;
		//! Function to get the position
		/*!
		\return The position
		*/
		virtual glm::vec2 getPosition() const = 0;
		//! Function to get the orientation
		/*!
		\return The orientation
		*/
		virtual float getOrientation() const = 0;
		//! Function to get the scale
		/*!
		\return The scale
		*/
		virtual float getScale() const = 0;
		//! Function to get the colour
		/*!
		\return The colour
		*/
		virtual const glm::vec3& getColour() const = 0;
		//! Function to get the model
		/*!
		\return The model
		*/
		virtual glm::mat4 getModel() const = 0;
		//! Function to get the material
		/*!
		\return A pointer to the material
		*/
		virtual std::shared_ptr<Material> getMaterial() const = 0;

		//! Function to make the material for the text
		virtual void makeMaterial() = 0;

		//! Function to create a text label
		/*!
		\param fontName The name of the font to give the text
		\param charSize The character size
		\param text The text to write
		\param position The position of the text
		\param orientation The orientation of the text
		\param scale The scale to give the text
		\param colour The colour to give the text
		\return A pointer to the label class
		*/
		static TextLabel* create(const std::string& fontName, unsigned int charSize, const std::string& text,
			const glm::vec2& position, float orientation, float scale, const glm::vec3& colour);
	};
}