/** \file character.h
*/
#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	/**
	\class Character
	\brief Stores all of a characters meta-data
	*/
	class Character
	{
	private:
		glm::vec2 m_startUV; //!< The characters start UV coordinates
		glm::vec2 m_endUV; //!< The characters end UV coordinates
		glm::ivec2 m_size; //!< The characters size
		glm::ivec2 m_bearing; //!< The bearing
		int m_iAdvance; //!< The advance
	public:
		//! Constructor
		/*!
		\param size The characters size
		\param bearing The bearing
		\param advance The advance
		\param startUV The characters start UV coordinates
		\param endUV The characters end UV coordinates
		*/
		Character(glm::ivec2 size, glm::ivec2 bearing, int advance, glm::vec2 startUV = glm::vec2(0.f, 0.f), glm::vec2 endUV = glm::vec2(0.f, 0.f)) :
			m_startUV(startUV), m_endUV(endUV), m_size(size), m_bearing(bearing), m_iAdvance(advance) {}

		//! Function to set the UVs
		/*!
		\param startUV The characters start UV coordinates
		\param endUV The characters end UV coordinates
		*/
		void setUVs(glm::vec2 startUV, glm::vec2 endUV) { m_startUV = startUV; m_endUV = endUV; }

		//! Function to get the start UV
		/*!
		\return The characters start UV coordinates
		*/
		inline glm::vec2 getStartUV() { return m_startUV; }
		//! Function to get the end UV
		/*!
		\return The characters end UV coordinates
		*/
		inline glm::vec2 getEndUV() { return m_endUV; }
		//! Function to get the size
		/*!
		\return The size of the character
		*/
		inline glm::ivec2 getSize() { return m_size; }
		//! Function to get the bearing
		/*!
		\return The bearing
		*/
		inline glm::ivec2 getBearing() { return m_bearing; }
		//! Function to get the advance
		/*!
		\return The advance
		*/
		inline int getAdvance() { return m_iAdvance; }
	};
}