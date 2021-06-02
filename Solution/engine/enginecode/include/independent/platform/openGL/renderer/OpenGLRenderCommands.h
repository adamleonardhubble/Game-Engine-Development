/** \file OpenGLRenderCommands.h
\brief File containing class declerations for the open GL render commands
*/
#pragma once

#include "rendering/renderer/renderCommand.h"

namespace Engine
{
	/**
	\class OpenGLClearDepthColourBufferCommand
	\brief Open GL command to clear the colour buffer and the depth buffer
	*/
	class OpenGLClearDepthColourBufferCommand : public RenderCommand
	{
	private:
		bool m_keepAlive; //!< If the command will be used again
	public:
		//! Constructor
		/*!
		\param keepAlive If the command should be deleted after being used
		*/
		OpenGLClearDepthColourBufferCommand(bool keepAlive) : m_keepAlive(keepAlive) {}
		void action() override; //!< Action the command
		//! Function to check if the command needs to be deleted
		/*!
		\return A bool for if the command should be deleted
		*/
		bool toKill() override { return !m_keepAlive; }
	};

	/**
	\class OpenGLSetClearColourCommand
	\brief Open GL command to set the colour of the window
	*/
	class OpenGLSetClearColourCommand : public RenderCommand
	{
	private:
		float m_r; //!< Red channel value
		float m_g; //!< Green channel value
		float m_b; //!< Blue channel value
		float m_a; //!< Alpha channel value

		bool m_keepAlive; //!< If the command will be used again
	public:
		//! Constructor
		/*!
		\param r New value for the red channel
		\param g New value for the green channel
		\param b New value for the blue channel
		\param a New value for the alpha channel
		\param keepAlive If the command should be deleted after being used
		*/
		OpenGLSetClearColourCommand(float r, float g, float b, float a, bool keepAlive) : 
			m_r(r), m_g(g), m_b(b), m_a(a), m_keepAlive(keepAlive) {}
		void action() override; //!< Action the command
		//! Function to check if the command needs to be deleted
		/*!
		\return A bool for if the command should be deleted
		*/
		bool toKill() override { return !m_keepAlive; }
	};

	/**
	\class OpenGLSetDepthTestLessCommand
	\brief Open GL command to set the depth test value to less
	*/
	class OpenGLSetDepthTestLessCommand : public RenderCommand
	{
	private:
		bool m_bEnabled; //!< If turning on or off

		bool m_keepAlive; //!< If the command will be used again
	public:
		//! Constructor
		/*!
		\param enabled If turning on or off
		\param keepAlive If the command should be deleted after being used
		*/
		OpenGLSetDepthTestLessCommand(bool enabled, bool keepAlive) : m_bEnabled(enabled), m_keepAlive(keepAlive) {}
		void action() override; //!< Action the command
		//! Function to check if the command needs to be deleted
		/*!
		\return A bool for if the command should be deleted
		*/
		bool toKill() override { return !m_keepAlive; }
	};

	/**
	\class OpenGLSetBackfaceCullingCommand
	\brief Open GL command to turn backface culling on or off
	*/
	class OpenGLSetBackfaceCullingCommand : public RenderCommand
	{
	private:
		bool m_bEnabled; //!< If turning on or off

		bool m_keepAlive; //!< If the command will be used again
	public:
		//! Constructor
		/*!
		\param enabled If turning on or off
		\param keepAlive If the command should be deleted after being used
		*/
		OpenGLSetBackfaceCullingCommand(bool enabled, bool keepAlive) : m_bEnabled(enabled), m_keepAlive(keepAlive) {}
		void action() override; //!< Action the command
		//! Function to check if the command needs to be deleted
		/*!
		\return A bool for if the command should be deleted
		*/
		bool toKill() override { return !m_keepAlive; }
	};

	/**
	\class OpenGLClearColourBufferCommand
	\brief Open GL command to clear the colour buffer
	*/
	class OpenGLClearColourBufferCommand : public RenderCommand
	{
	private:
		bool m_keepAlive; //!< If the command will be used again
	public:
		//! Constructor
		/*!
		\param keepAlive If the command should be deleted after being used
		*/
		OpenGLClearColourBufferCommand(bool keepAlive) : m_keepAlive(keepAlive) {}
		void action() override; //!< Action the command
		//! Function to check if the command needs to be deleted
		/*!
		\return A bool for if the command should be deleted
		*/
		bool toKill() override { return !m_keepAlive; }
	};

	/**
	\class OpenGLSetOneMinusAlphaBlendingCommand
	\brief Open GL command to turn alpha blending on or off
	*/
	class OpenGLSetOneMinusAlphaBlendingCommand : public RenderCommand
	{
	private:
		bool m_bEnabled; //!< If turning on or off

		bool m_keepAlive; //!< If the command will be used again
	public:
		//! Constructor
		/*!
		\param enabled If turning on or off
		\param keepAlive If the command should be deleted after being used
		*/
		OpenGLSetOneMinusAlphaBlendingCommand(bool enabled, bool keepAlive) : m_bEnabled(enabled), m_keepAlive(keepAlive) {}
		void action() override; //!< Action the command
		//! Function to check if the command needs to be deleted
		/*!
		\return A bool for if the command should be deleted
		*/
		bool toKill() override { return !m_keepAlive; }
	};
}