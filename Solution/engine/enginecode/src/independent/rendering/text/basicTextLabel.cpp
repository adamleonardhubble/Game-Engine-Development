/** \file basicTextLabel.cpp
*/
#include "engine_pch.h"

#include "rendering/text/basicTextLabel.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
	int BasicTextLabel::s_iIndex = 0;

	BasicTextLabel::BasicTextLabel(const std::string& fontName, unsigned int charSize, const std::string& text, 
		const glm::vec2& position, float orientation, float scale, const glm::vec3& colour) : m_font(fontName),
		m_iSize(charSize), m_text(text), m_position(position), m_fAngle(orientation), m_fScale(scale), m_colour(colour)
	{
		m_iIndex = s_iIndex;
		s_iIndex++;

		makeMaterial();

		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(m_position, 0));
		glm::mat4 rotate = glm::rotate(glm::mat4(1.f), glm::radians(m_fAngle), glm::vec3(0.f, 0.f, 1.f));
		glm::mat4 scaled = glm::scale(glm::mat4(1.f), glm::vec3(glm::vec2(m_fScale), 1));
		m_model = translate * rotate * scaled;
	}

	bool BasicTextLabel::setFont(const std::string& fontName)
	{
		m_font = fontName;

		return false;
	}

	bool BasicTextLabel::setCharSize(unsigned int charSize)
	{
		m_iSize = charSize;

		return false;
	}

	void BasicTextLabel::setText(const std::string& text)
	{
		m_text = text;
	}

	void BasicTextLabel::setPosition(const glm::vec2& position)
	{
		m_position = position;

		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(m_position, 0));
		glm::mat4 rotate = glm::rotate(glm::mat4(1.f), glm::radians(m_fAngle), glm::vec3(0.f, 0.f, 1.f));
		glm::mat4 scaled = glm::scale(glm::mat4(1.f), glm::vec3(glm::vec2(m_fScale), 1));
		m_model = translate * rotate * scaled;
	}

	void BasicTextLabel::setOrientation(float angle)
	{
		m_fAngle = angle;

		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(m_position, 0));
		glm::mat4 rotate = glm::rotate(glm::mat4(1.f), glm::radians(m_fAngle), glm::vec3(0.f, 0.f, 1.f));
		glm::mat4 scaled = glm::scale(glm::mat4(1.f), glm::vec3(glm::vec2(m_fScale), 1));
		m_model = translate * rotate * scaled;
	}

	void BasicTextLabel::setScale(float scale)
	{
		m_fScale = scale;

		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(m_position, 0));
		glm::mat4 rotate = glm::rotate(glm::mat4(1.f), glm::radians(m_fAngle), glm::vec3(0.f, 0.f, 1.f));
		glm::mat4 scaled = glm::scale(glm::mat4(1.f), glm::vec3(glm::vec2(m_fScale), 1));
		m_model = translate * rotate * scaled;
	}

	void BasicTextLabel::setColour(const glm::vec3& colour)
	{
		m_colour = colour;
	}

	void BasicTextLabel::makeMaterial()
	{
		float* vertices; // For the text vertices
		// Create an array the size of the needed vertices
		vertices = new float[4 * 4 * m_text.length()];
		// For character placement
		int previousAdvance = 0;
		int yStart = 0;
		// For each character in the text
		for (int i = 0; i < m_text.length(); i++)
		{
			// Get the character
			std::shared_ptr<Character> ch = ResourceManager::getCharacter(m_font, m_iSize, (unsigned int)m_text.at(i));
			// Set the y start position
			yStart = -(ch->getSize().y - ch->getBearing().y) * m_fScale;
			// Make temporary vertices for just this character
			float vert[4 * 4] = {
				previousAdvance,                                yStart,                                ch->getStartUV().x,  ch->getStartUV().y,
				previousAdvance + (ch->getSize().x * m_fScale), yStart,                                ch->getEndUV().x,    ch->getStartUV().y,
				previousAdvance + (ch->getSize().x * m_fScale), yStart + (ch->getSize().y * m_fScale), ch->getEndUV().x,    ch->getEndUV().y,
				previousAdvance,                                yStart + (ch->getSize().y * m_fScale), ch->getStartUV().x,  ch->getEndUV().y
			};
			previousAdvance += (ch->getAdvance() / 64) * m_fScale; // Increase the advance for placing the next character
			// For each value in the vertices
			for (int j = 0; j < 4 * 4; j++)
			{
				// Add the value to the text vertices
				vertices[(i * 4 * 4) + j] = vert[j];
			}
		}

		unsigned int* indices; // For the text indices
		// Make the indices the correct size
		indices = new unsigned int[4 * m_text.length()];
		// For each value in the indices
		for (int i = 0; i < m_text.length() * 4; i++)
		{
			indices[i] = i; // Set the value
		}
		// Create shader, vertex array, vertex buffer and index buffer with the vertices and indices just made
		std::shared_ptr<Shader> tempSetupShader = ResourceManager::addShader("assets/shaders/text2.glsl");
		std::shared_ptr<VertexArray> tempSetupVAO = ResourceManager::addVAO(m_text + m_font + std::to_string(m_iSize));
		tempSetupVAO->setVertexBuffer(ResourceManager::addVBO(m_text + m_font + std::to_string(m_iSize), vertices, sizeof(float) * 16 * m_text.length(), tempSetupShader->getBufferLayout()));
		tempSetupVAO->setIndexBuffer(ResourceManager::addIndexBuffer(m_text + m_font + std::to_string(m_iSize), indices, m_text.length() * 4));
		// Create the material
		m_material = ResourceManager::changeMaterial("Text" + std::to_string(m_iIndex), tempSetupShader, tempSetupVAO);
	}
}