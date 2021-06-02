/** \file OpenGLFrameBuffer.h
*/

#pragma once // Compile once

#include "rendering/frameBuffer.h" // Include 'frameBuffer.h' file

#include "systems/timer.h" // Include 'timer.h' file

#include "windows/window.h"	// Include 'window.h' file

#include "platform/GLFW/glfwWindow.h" // Include 'glfwWindow.h' file

namespace Engine
{
	/**
	\class OpenGLFrameBuffer
	\brief OpenGL specific frame buffer
	*/
	class OpenGLFrameBuffer : public FrameBuffer
	{
	private: // Private members
		unsigned int m_iFrameBufferID; //!< Frame buffer ID, used for frame buffering
		unsigned int m_iMultiSampledFrameBufferID; //!< Frame buffer ID, used for frame buffering

		unsigned int m_iDepthMapFrameBufferID; //!< Frame buffer ID, used for frame buffering

		unsigned int m_iShadowDepthMapWidth = 1024; //!< Shadow depth map texture width dimension
		unsigned int m_iShadowDepthMapHeight = 1024; //!< Shadow depth map texture height dimension

		unsigned int m_iColourRenderBufferID; //!< Colour render buffer ID, used for frame buffering
		unsigned int m_iDepthRenderBufferID; //!< Depth render buffer ID, used for frame buffering

		bool m_bMSAAEnabled = false; //!< Determine whether MSAA is enabled

		int supportedMSAASamples; //!< MSAA samples supported by the GPU

		unsigned int m_iSamples = 4; //!< Anti-aliasing samples applied to the MSAA frame buffer
		
		unsigned int m_iColourAttachmentsApplied = 4; //!< Colour attachments available for use of frame buffers, by shaders

		const GLenum colourBuffers[4]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 }; //!< Enumerator container, store the GL enumerators representing each colour attachement occupied by the frame buffer

		unsigned int m_iMSAAColourTextureAttachementID[4] = {}; //!< MSAA colour texture attachment ID, used for frame buffering
		unsigned int m_iColourTextureAttachmentID[4] = {}; //!< Colour texture attachment ID, used for frame buffering

		unsigned int m_iMSAADepthTextureAttachmentID; //!< MSAA depth texture attachment ID, used for frame buffering
		unsigned int m_iDepthTextureAttachmentID; //!< Depth texture attachment ID, used for frame buffering 

		unsigned int m_iDepthMapDepthTextureAttachmentID; //!< Depth map depth texture attachment ID, used for frame buffering
		
		unsigned int m_iMSAAColourTextureSlot[4] = {}; //!< MSAA colour texture slot, used for frame buffering
		unsigned int m_iColourTextureSlot[4] = {}; //!< Colour texture slot, used for frame buffering
		
		unsigned int m_iMSAADepthTextureSlot; //!< MSAA Depth texture slot, used for frame buffering
		unsigned int m_iDepthTextureSlot; //!< Depth texture slot, used for frame buffering

		unsigned m_iDepthMapDepthTextureSlot; //!< Depth map texture slot, used for frame buffering
		
		glm::vec2 m_screenResolution; //!< Content area of the current display, used for frame buffer uniform variables

		bool m_bWindowResized = false; //!< Determine whether the window has been resized

		bool m_bMSAAEnabledUpdate = false; //!< Determine whether MSAA has been enabled or disabled and therefore requires an update
		bool m_bMSAASampleUpdate = false; //!< Determine whether the samples used by MSAA has been increase or decrease

		bool m_bWireframeActive = false; //!< Determine whether wireframe mode has been enabled or disabled

	public: // Public members
		OpenGLFrameBuffer(); //!< Constructor, create frame buffer object instance

		~OpenGLFrameBuffer(); //!< Destructor, destroy frame buffer object instance

		void defaultBind() override; //!< Bind the default frame buffer

		void defaultUnbind() override; //!< Unbind the default frame buffer

		void shadowBind() override; //!< Bind the shadow map frame buffer

		void shadowUnbind() override; //!< Unbind the shadow map frame buffer
		
		//!	Return the colour texture attachment ID
		/*!
		\param colourBufferAmount An integer representing the colour texture attachement ID 
		\return The colour texture attachment ID
		*/
		inline unsigned int getColourTextureID(int colourBufferAmount) const override { return m_iColourTextureAttachmentID[colourBufferAmount]; }

		//!	Return the depth texture attachment ID
		/*!
		\return The depth texture attachment ID
		*/
		inline unsigned int getDepthTextureID() const override { return m_iDepthTextureAttachmentID; }

		//! Return the colour texture slot occupied by the frame buffer
		/*!
		\param colourBufferAmount An integer representing the colour texture slot
		\return The texture slot of the frame buffer texture
		*/
		inline unsigned int getColourTextureSlot(int colourBufferAmount) const override { return m_iColourTextureSlot[colourBufferAmount]; }
		
		//! Return the depth texture slot occupied by the frame buffer
		/*!
		\return The depth texture slot of the frame buffer texture
		*/
		inline unsigned int getDepthTextureSlot() const override { return m_iDepthTextureSlot; }

		//! Return the depth map texture slot occupied by the frame buffer
		/*!
		\return The depth map texture slot of the frame buffer texture
		*/
		inline unsigned int getDepthMapTextureSlot() const override { return m_iDepthMapDepthTextureSlot; }
		
		//! Return the content area of the current display
		/*!
		\return The dimensions of the content area of the current display
		*/
		inline glm::vec2 getScreenResolution() const override { return m_screenResolution; }
		
		inline void toggleMSAA() override { m_bMSAAEnabled = !m_bMSAAEnabled; } //!< Determine whether MSAA is enabled

		//! Return the active state of MSAA
		/*!
		\return A boolean representing the activeness of MSAA
		*/
		inline bool getMSAA() const override { return m_bMSAAEnabled; }

		virtual void increaseMSAA() override; //!< Increase the number of samples used by MSAA

		virtual void decreaseMSAA() override; //!< Decrease the number of samples used by MSAA

		inline void updateSampleMSAA() override { m_bMSAASampleUpdate = true; }	//!< Update the sample size used by MSAA

		virtual void toggleWireframe() override { m_bWireframeActive = !m_bWireframeActive; } //!< Determine whether wireframe mode is enabled

		//! Dispatch frame buffer events
		/*!
		\param e A reference to the event class
		*/
		virtual void onEvent(Event& e) override;

		//! Handle frame buffer window resize events
		/*!
		\param e A reference to the window resize event class
		*/
		virtual bool onWindowResize(WindowResizeEvent& e) override;
	};
}