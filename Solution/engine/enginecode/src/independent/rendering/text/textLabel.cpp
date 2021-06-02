/** \file textLabel.cpp
*/
#include "engine_pch.h"

#include "rendering/text/textLabel.h"
#include "rendering/text/basicTextLabel.h"

namespace Engine
{
	TextLabel* TextLabel::create(const std::string& fontName, unsigned int charSize, const std::string& text,
		const glm::vec2& position, float orientation, float scale, const glm::vec3& colour)
	{
		// Create and return a pointer to a text label
		return new BasicTextLabel(fontName, charSize, text, position, orientation, scale, colour);
	}
}