/** \file frameBuffer.h
*/

#pragma once // Compile once

#include "events/event.h" // Include 'event.h' file

#include "events/windowEvents.h" // Include 'windowEvents.h' file

#include "rendering/texture.h" // Include 'texture.h' file

#include <glm/glm.hpp> // Include glm support

namespace Engine
{
	/**
	\class FrameBuffer
	\brief A base interface class for API specific frame buffer to use
	*/
	class FrameBuffer
	{
	public:	// Public members
		//! Destructor
		virtual ~FrameBuffer() {}

		//! Create a frame buffer and return a pointer to it
		/*!
		\return A pointer to the new frame buffer instance
		*/
		static FrameBuffer* create();
		
		virtual void defaultBind() = 0; //!< Bind the default frame buffer

		virtual void defaultUnbind() = 0; //!< Unbind the default frame buffer

		virtual void shadowBind() = 0; //!< Bind the shadow map frame buffer

		virtual void shadowUnbind() = 0; //!< Unbind the shadow map frame buffer

		//! Return the colour texture attachment ID
		/*!
		\return The colour texture attachment ID
		*/
		virtual unsigned int getColourTextureID(int colourBufferAmount) const = 0;

		//! Return the depth texture attachment ID 
		/*!
		\return The depth texture attachment ID 
		*/
		virtual unsigned int getDepthTextureID() const = 0;

		//! Return the colour texture slot occupied by the frame buffer
		/*!
		\return The colour texture slot occupied by the frame buffer
		*/
		virtual unsigned int getColourTextureSlot(int colourBufferAmount) const = 0;
		
		//! Return the depth texture slot occupied by the frame buffer
		/*!
		\return The depth texture slot occupied by the frame buffer
		*/
		virtual unsigned int getDepthTextureSlot() const = 0;

		//! Return the depth map texture slot occupied by the frame buffer
		/*!
		\return The depth map texture slot of the frame buffer texture
		*/
		virtual unsigned int getDepthMapTextureSlot() const = 0;
		
		//! Return the content area of the current display
		/*!
		\return The dimensions of the content area of the current display
		*/
		virtual glm::vec2 getScreenResolution() const = 0;

		virtual void toggleMSAA() = 0; //!< Determine whether MSAA is enabled

		//! Return the active state of MSAA
		/*!
		\return A boolean representing the activeness of MSAA
		*/
		virtual bool getMSAA() const = 0;

		virtual void increaseMSAA() = 0; //!< Increase the number of samples used by MSAA

		virtual void decreaseMSAA() = 0; //!< Decrease the number of samples used by MSAA

		virtual void updateSampleMSAA() = 0; //!< Update the sample size used by MSAA

		virtual void toggleWireframe() = 0; //!< Determine whether wireframe mode is enabled

		//! Dispatch frame buffer events
		/*!
		\param e A reference to the event class
		*/
		virtual void onEvent(Event& e) = 0;

		//! Handle frame buffer window resize events
		/*!
		\param e A reference to the window resize event class
		*/
		virtual bool onWindowResize(WindowResizeEvent& e) = 0;
	};
}
