/** \file basicTextLabel.h
*/
#pragma once

#include "textLabel.h"

namespace Engine
{
	/**
	\class BasicTextLabel
	\brief Simple text label that just draws more than one character
	*/
	class BasicTextLabel : public TextLabel
	{
	private:
		std::string m_font; //!< The font of the text
		unsigned int m_iSize; //!< The size of the text
		std::string m_text; //!< The text
		glm::vec2 m_position; //!< The position of the text
		float m_fAngle; //!< The orientation of the text
		float m_fScale; //!< The scale of the text
		glm::vec3 m_colour; //!< The colour of the text

		glm::mat4 m_model; //!< The text model
		std::shared_ptr<Material> m_material; //!< The text material

		int m_iIndex;
		static int s_iIndex;
	public:
		//! Constructor
		/*!
		\param fontName The name of the font to give the text
		\param charSize The character size
		\param text The text to write
		\param position The position of the text
		\param orientation The orientation of the text
		\param scale The scale to give the text
		\param colour The colour to give the text
		*/
		BasicTextLabel(const std::string& fontName, unsigned int charSize, const std::string& text,
			const glm::vec2& position, float orientation, float scale, const glm::vec3& colour);

		//! Function to set the font of the text
		/*!
		\param fontName The name of the new font
		*/
		bool setFont(const std::string& fontName) override;
		//! Function to set the character size of the text
		/*!
		\param charSize The new size
		*/
		bool setCharSize(unsigned int charSize) override;
		//! Function to set the text being shown
		/*!
		\param text The new text to show
		*/
		void setText(const std::string& text) override;
		//! Function to set the position of the text
		/*!
		\param position The new position
		*/
		void setPosition(const glm::vec2& position) override;
		//! Function to set the orientation of the text
		/*!
		\param angle The new angle
		*/
		void setOrientation(float angle) override;
		//! Function to set the scale of the text
		/*!
		\param scale The new scale of the text
		*/
		void setScale(float scale) override;
		//! Function to set the colour of the text
		/*!
		\param colour The new colour
		*/
		void setColour(const glm::vec3& colour) override;

		//! Function to get the font
		/*!
		\return The font
		*/
		inline const std::string& getFont() const override { return m_font; }
		//! Function to get the character size
		/*!
		\return The charactersize
		*/
		inline unsigned int getCharSize() const override { return m_iSize; }
		//! Function to get the text
		/*!
		\return The text
		*/
		inline const std::string& getText() const override { return m_text; }
		//! Function to get the position
		/*!
		\return The position
		*/
		inline glm::vec2 getPosition() const override { return m_position; }
		//! Function to get the orientation
		/*!
		\return The orientation
		*/
		inline float getOrientation() const override { return m_fAngle; }
		//! Function to get the scale
		/*!
		\return The scale
		*/
		inline float getScale() const override { return m_fScale; }
		//! Function to get the colour
		/*!
		\return The colour
		*/
		inline const glm::vec3& getColour() const override { return m_colour; }
		//! Function to get the model
		/*!
		\return The model
		*/
		inline glm::mat4 getModel() const override { return m_model; }
		//! Function to get the material
		/*!
		\return A pointer to the material
		*/
		inline std::shared_ptr<Material> getMaterial() const override { return m_material; }

		//! Function to make the material for the text
		void makeMaterial() override;
	};
}