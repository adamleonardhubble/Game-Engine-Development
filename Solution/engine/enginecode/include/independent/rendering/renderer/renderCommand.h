/** \file renderCommand.h
*/
#pragma once

namespace Engine
{
	/**
	\class RenderCommand
	\brief Base class for any render commands
	*/
	class RenderCommand
	{
	public:
		virtual void action() = 0; //!< Action the command
		//! Function to check if the command needs to be deleted
		/*!
		\return A bool for if the command should be deleted
		*/
		virtual bool toKill() = 0;

		//! Function to create a command to clear the colour buffer and the depth buffer
		/*!
		\param keepAlive If the command should be deleted after being used
		\return A pointer to the command
		*/
		static RenderCommand* clearDepthColourBufferCommand(bool keepAlive = false);
		//! Function to create a command to set the colour of the window
		/*!
		\param r New value for the red channel
		\param g New value for the green channel
		\param b New value for the blue channel
		\param a New value for the alpha channel
		\param keepAlive If the command should be deleted after being used
		\return A pointer to the command
		*/
		static RenderCommand* setClearColourCommand(float r, float g, float b, float a, bool keepAlive = false);
		//! Function to create a command to set the depth test value to less
		/*!
		\param enabled If turning on or off
		\param keepAlive If the command should be deleted after being used
		\return A pointer to the command
		*/
		static RenderCommand* setDepthTestLessCommand(bool enabled, bool keepAlive = false);
		//! Function to create a command to turn backface culling on or off
		/*!
		\param enabled If turning on or off
		\param keepAlive If the command should be deleted after being used
		\return A pointer to the command
		*/
		static RenderCommand* setBackfaceCullingCommand(bool enabled, bool keepAlive = false);
		//! Function to create a command to clear the colour buffer
		/*!
		\param keepAlive If the command should be deleted after being used
		\return A pointer to the command
		*/
		static RenderCommand* clearColourBufferCommand(bool keepAlive = false);
		//! Function to create a command to turn alpha blending on or off
		/*!
		\param enabled If turning on or off
		\param keepAlive If the command should be deleted after being used
		\return A pointer to the command
		*/
		static RenderCommand* setOneMinusAlphaBlendingCommand(bool enabled, bool keepAlive = false);
	};
}