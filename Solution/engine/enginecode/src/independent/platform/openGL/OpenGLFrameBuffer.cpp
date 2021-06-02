/** \file OpenGLFrameBuffer.cpp
*/

#include "engine_pch.h" // Include precompiled headers

#include "platform/openGL/OpenGLFrameBuffer.h" // Include 'OpenGLFrameBuffer.h' file

#include "core/application.h" // Include 'application.h' file

#include "core/core.h" // Include 'core.h' file

#include <glad/glad.h> // Include GLAD support

namespace Engine // Engine namespace
{
	OpenGLFrameBuffer::OpenGLFrameBuffer() // Create a frame buffer object instance
	{
		#pragma region Return Screen Resolution
		Engine::Application& application = Engine::Application::getInstance(); // Get application accessor instance (singleton class)
		glm::vec2 screenResolution = application.getScreenResolution(); // Store the current resolution of the application window
		#pragma endregion

		glGetIntegerv(GL_MAX_SAMPLES, &supportedMSAASamples); // Store the number of MSAA samples supproted by the GPU

		#pragma region Frame Buffer Initialisation
		glGenFramebuffers(1, &m_iMultiSampledFrameBufferID); // Create the MSAA frame buffer object
		glGenFramebuffers(1, &m_iFrameBufferID); // Create the default frame buffer object

		glGenFramebuffers(1, &m_iDepthMapFrameBufferID); // Create the shadow map frame buffer object
		
		glGenRenderbuffers(1, &m_iColourRenderBufferID); // Create the colour render buffer object
		glGenRenderbuffers(1, &m_iDepthRenderBufferID); // Create the depth render buffer object
		#pragma endregion

		glBindFramebuffer(GL_FRAMEBUFFER, m_iMultiSampledFrameBufferID); // Bind the frame buffer object

		#pragma region Colour Texture Attachment
		for (int i = 0; i != m_iColourAttachmentsApplied; i++) // For all of the colour attachments specified, do the following
		{
			glGenTextures(1, &m_iMSAAColourTextureAttachementID[i]); // Generate a colour texture attachment

			m_iMSAAColourTextureSlot[i] = Texture::assignTextureSlot(); // Return the current texture slot
			glActiveTexture(GL_TEXTURE0 + m_iMSAAColourTextureSlot[i]); // Set the current slot to the active one

			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_iMSAAColourTextureAttachementID[i]); // Bind the texture attachment

			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Texture parameter wrap (clamp to the edges of specified texture coordinates)
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Texture parameter wrap (clamp to the edges of specified texture coordinates)
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Texture parameter minify sampling (linear - returns the weighted average of the four texture elements that are closest to the specified texture coordinates)
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Texture parameter magnify sampling (linear - returns the weighted average of the four texture elements that are closest to the specified texture coordinates)

			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_iSamples, GL_RGBA, screenResolution.x, screenResolution.y, GL_TRUE);	// Configure the multisampled colour texture attachment (with alpha channel)
		}
		#pragma endregion

		#pragma region Depth Texture Attachment
		glGenTextures(1, &m_iMSAADepthTextureAttachmentID); // Generate a depth texture attachment

		m_iMSAADepthTextureSlot = Texture::assignTextureSlot(); // Return the current texture slot
		glActiveTexture(GL_TEXTURE0 + m_iMSAADepthTextureSlot); // Set the current slot to the active one

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_iMSAADepthTextureAttachmentID); // Bind the texture attachment

		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Texture parameter wrap (clamp to the edges of specified texture coordinates)
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Texture parameter wrap (clamp to the edges of specified texture coordinates)
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Texture parameter minify sampling (linear - returns the weighted average of the four texture elements that are closest to the specified texture coordinates)
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Texture parameter magnify sampling (linear - returns the weighted average of the four texture elements that are closest to the specified texture coordinates)

		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_iSamples, GL_DEPTH_COMPONENT, screenResolution.x, screenResolution.y, GL_TRUE); // Configure the multisampled depth texture attachment
		#pragma endregion

		#pragma region Attach Texture Attachments
		for (int i = 0; i != m_iColourAttachmentsApplied; i++) // For all of the colour attachments specified, do the following
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, colourBuffers[i], GL_TEXTURE_2D_MULTISAMPLE, m_iMSAAColourTextureAttachementID[i], 0); // Attach colour texture to frame buffer
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_iMSAADepthTextureAttachmentID, 0); // Attach depth texture to frame buffer

		glDrawBuffers(4, colourBuffers); // Draw all of the colour attachments specified for the frame buffer
		#pragma endregion

		#pragma region Render Buffer Objects
		glBindRenderbuffer(GL_RENDERBUFFER, m_iColourRenderBufferID);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_iSamples, GL_RGBA, screenResolution.x, screenResolution.y); // allocate storage for render buffer object
		
		glBindRenderbuffer(GL_RENDERBUFFER, m_iDepthRenderBufferID);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_iSamples, GL_DEPTH_COMPONENT, screenResolution.x, screenResolution.y); // allocate storage for render buffer object

		for (int i = 0; i != m_iColourAttachmentsApplied; i++) // For all of the colour attachments specified, do the following
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, colourBuffers[i], GL_RENDERBUFFER, m_iColourRenderBufferID); // attach MS render buffer object to framebuffer
		}
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iDepthRenderBufferID); // attach MS render buffer object to framebuffer
		#pragma endregion

		#pragma region Check Frame Buffer Completion
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) // Check frame buffer completion (setup not successful)
		{
			LOG_TRACE("MSFBO Setup Error: MSAAFrameBufferID [{0}]", m_iMultiSampledFrameBufferID); // Log the frame buffer was not setup
		}
		#pragma endregion

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer object

		glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBufferID); // Bind the frame buffer object

		#pragma region Colour Texture Attachment
		for (int i = 0; i != m_iColourAttachmentsApplied; i++) // For all of the colour attachments specified, do the following
		{
			glGenTextures(1, &m_iColourTextureAttachmentID[i]); // Generate a colour texture attachment

			m_iColourTextureSlot[i] = Texture::assignTextureSlot(); // Return the current texture slot
			glActiveTexture(GL_TEXTURE0 + m_iColourTextureSlot[i]); // Set the current slot to the active one

			glBindTexture(GL_TEXTURE_2D, m_iColourTextureAttachmentID[i]); // Bind the texture attachment

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Texture parameter wrap (clamp to the edges of specified texture coordinates)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Texture parameter wrap (clamp to the edges of specified texture coordinates)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Texture parameter minify sampling (linear - returns the weighted average of the four texture elements that are closest to the specified texture coordinates)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Texture parameter magnify sampling (linear - returns the weighted average of the four texture elements that are closest to the specified texture coordinates)

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenResolution.x, screenResolution.y, 0, GL_RGBA, GL_UNSIGNED_INT, NULL); // Configure the colour texture attachment (with alpha channel)
		}
		#pragma endregion

		#pragma region Depth Texture Attachment
		glGenTextures(1, &m_iDepthTextureAttachmentID); // Generate a depth texture attachment

		m_iDepthTextureSlot = Texture::assignTextureSlot(); // Return the current texture slot
		glActiveTexture(GL_TEXTURE0 + m_iDepthTextureSlot); // Set the current slot to the active one

		glBindTexture(GL_TEXTURE_2D, m_iDepthTextureAttachmentID); // Bind the texture attachment

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Texture parameter wrap (clamp to the edges of specified texture coordinates)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Texture parameter wrap (clamp to the edges of specified texture coordinates)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Texture parameter minify sampling (linear - returns the weighted average of the four texture elements that are closest to the specified texture coordinates)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Texture parameter magnify sampling (linear - returns the weighted average of the four texture elements that are closest to the specified texture coordinates)
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenResolution.x, screenResolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); // Configure the depth texture attachment
		#pragma endregion

		#pragma region Attach Texture Attachments
		for (int i = 0; i != m_iColourAttachmentsApplied; i++) // For all of the colour attachments specified, do the following
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, colourBuffers[i], GL_TEXTURE_2D, m_iColourTextureAttachmentID[i], 0); // Attach colour texture to frame buffer
		}
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iDepthTextureAttachmentID, 0); // Attach depth texture to frame buffer
		
		glDrawBuffers(4, colourBuffers); // Draw all of the colour attachments specified for the frame buffer
		#pragma endregion

		#pragma region Check Frame Buffer Completion
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) // Check frame buffer completion (setup not successful)
		{
			LOG_TRACE("FBO Setup Error: FrameBufferID [{0}]", m_iFrameBufferID); // Log the frame buffer was not setup
		}
		#pragma endregion	

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer object

		glBindFramebuffer(GL_FRAMEBUFFER, m_iDepthMapFrameBufferID); // Bind the frame buffer object

		#pragma region Depth Texture Attachment
		glGenTextures(1, &m_iDepthMapDepthTextureAttachmentID); // Generate a depth texture attachment

		m_iDepthMapDepthTextureSlot = Texture::assignTextureSlot(); // Return the current texture slot
		glActiveTexture(GL_TEXTURE0 + m_iDepthMapDepthTextureSlot); // Set the current slot to the active one

		glBindTexture(GL_TEXTURE_2D, m_iDepthMapDepthTextureAttachmentID); // Bind the frame buffer object

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // Texture parameter wrap (texture coordinates outside of specified range are given the specified border colour)  
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); // Texture parameter wrap (texture coordinates outside of specified range are given the specified border colour)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Texture parameter	minify sampling (nearest - returns the value of the texture element that is nearest to the centre of the pixel being textured)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Texture parameter	magnif sampling (nearest - returns the value of the texture element that is nearest to the centre of the pixel being textured)
		
		float borderColour[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Colour of the bordering texels (white)
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour); // Texture parameter border colour of the texture (white)

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_iShadowDepthMapWidth, m_iShadowDepthMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);	// Configure the depth texture attachment
		#pragma endregion

		#pragma region Attach Texture Attachments
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iDepthMapDepthTextureAttachmentID, 0); // Attach depth texture to frame buffer
		
		glDrawBuffer(GL_NONE); // Colour data is not rendered
		glReadBuffer(GL_NONE); // Colour data is not rendered
        #pragma endregion

		#pragma region Check Frame Buffer Completion
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) // Check frame buffer completion (setup not successful)
		{
			LOG_TRACE("FBO Setup Error: DepthMapFrameBufferID [{0}]", m_iDepthMapFrameBufferID); // Log the frame buffer was not setup
		}
		#pragma endregion	

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer object
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()	// Destroy the frame buffer instances
	{
		glDeleteFramebuffers(1, &m_iMultiSampledFrameBufferID);	// Delete the MSAA frame buffer object
		glDeleteFramebuffers(1, &m_iFrameBufferID);	// Delete the frame buffer object

		glDeleteFramebuffers(1, &m_iDepthMapFrameBufferID);	// Delete the depth map frame buffer object
		
		glDeleteRenderbuffers(1, &m_iColourRenderBufferID); // Delete the colour render buffer object
		glDeleteRenderbuffers(1, &m_iDepthRenderBufferID); // Delete the depth render buffer object

		// Delete textures and free texture slots
		for (int i = 0; i != m_iColourAttachmentsApplied; i++)
		{
			glDeleteTextures(1, &m_iMSAAColourTextureAttachementID[i]);
			Texture::clearTextureSlot(m_iMSAAColourTextureSlot[i]);
		}
		glDeleteTextures(1, &m_iMSAADepthTextureAttachmentID);
		Texture::clearTextureSlot(m_iMSAADepthTextureSlot);

		for (int i = 0; i != m_iColourAttachmentsApplied; i++)
		{
			glDeleteTextures(1, &m_iColourTextureAttachmentID[i]);
			Texture::clearTextureSlot(m_iColourTextureSlot[i]);
		}
		glDeleteTextures(1, &m_iDepthTextureAttachmentID);
		Texture::clearTextureSlot(m_iDepthTextureSlot);
		glDeleteTextures(1, &m_iDepthMapDepthTextureAttachmentID);
		Texture::clearTextureSlot(m_iDepthMapDepthTextureSlot);
	}

	void OpenGLFrameBuffer::defaultBind() // Bind the default frame buffer
	{
		Engine::Application& application = Engine::Application::getInstance(); // Get application accessor instance (singleton class)
		glm::vec2 screenResolution = application.getScreenResolution();	// Store the current resolution of the application window
		bool viewportActive = application.getWindow().isViewportActive(); // Return the viewport activeness of the window instance
		float aspectRatio = application.getWindow().getAspectRatio(); // Return the aspect ration of the window instance (viewport)

		if (viewportActive)	// If a viewport is active, do the following
		{
			if (m_bMSAAEnabled == true) // If MSAA is enabled, do the following
			{
				//LOG_CRITICAL("MSAA ENABLED! {0}", m_iSamples); // Output the inactiveness of MSAA and the samples currently set

				if (m_bMSAAEnabledUpdate == false || m_bWindowResized == true || m_bMSAASampleUpdate == true) // If the application window has been resized or MSAA has been enabled/ disabled or adjusted, do the following
				{
					glBindFramebuffer(GL_FRAMEBUFFER, m_iMultiSampledFrameBufferID); // Bind the MSAA frame buffer object

					#pragma region Update Colour Texture Attachments
					for (int i = 0; i != m_iColourAttachmentsApplied; i++) // For all of the colour attachments specified, do the following
					{
						glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_iMSAAColourTextureAttachementID[i]); // Bind the texture attachment
						glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_iSamples, GL_RGBA, screenResolution.x, screenResolution.y, GL_TRUE);	// Configure the multisampled colour texture attachment (with alpha channel)
						glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0); // Unbind the texture attachment
					}
					#pragma endregion

					#pragma region Update Depth Texture Attachment
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_iMSAADepthTextureAttachmentID); // Bind the MSAA texture attachment
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_iSamples, GL_DEPTH_COMPONENT, screenResolution.x, screenResolution.y, GL_TRUE); // Configure the multisampled depth texture attachment (with alpha channel)
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0); // Unbind the texture attachment
					#pragma endregion

					#pragma region Update Render Buffer Objects
					glBindRenderbuffer(GL_RENDERBUFFER, m_iColourRenderBufferID); // Bind the colour render buffer object 
					glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_iSamples, GL_RGBA, screenResolution.x, screenResolution.y); // Allocate storage for the colour render buffer object
					glBindRenderbuffer(GL_RENDERBUFFER, 0);	// Unbind the colour render buffer object

					glBindRenderbuffer(GL_RENDERBUFFER, m_iDepthRenderBufferID); // Bind the depth render buffer object
					glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_iSamples, GL_DEPTH_COMPONENT, screenResolution.x, screenResolution.y); // Allocate storage for the depth render buffer object
					glBindRenderbuffer(GL_RENDERBUFFER, 0);	// Unbind the depth render buffer object

					for (int i = 0; i != m_iColourAttachmentsApplied; i++) // For all of the colour attachments specified, do the following
					{
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, colourBuffers[i], GL_RENDERBUFFER, m_iColourRenderBufferID); // Attach the colour render buffer object to the MSAA frame buffer
					}

					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iDepthRenderBufferID); // Attach the depth render buffer object to the MSAA frame buffer
					#pragma endregion

					glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the MSAA frame buffer object
				}

				glBindFramebuffer(GL_FRAMEBUFFER, m_iMultiSampledFrameBufferID); // Bind the MSAA frame buffer object

				if (m_bWireframeActive == true)	// If wireframe mode is active, do the following
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Draw the outlines of primitives
				}
			}
			else if (m_bMSAAEnabled == false) // If MSAA is disabled, do the following
			{
				m_bMSAAEnabledUpdate = false; // Reset the MSAA enable update state, used for future enabling/ disabling or MSAA

				//LOG_TRACE("MSAA DISABLED! {0}", m_iSamples); // Output the activeness of MSAA and the samples currently set

				if (m_bWindowResized == true) // If the application window has been resized, do the following
				{
					glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBufferID); // Bind the frame buffer object

					#pragma region Update Viewport
					if (screenResolution.x > screenResolution.y * aspectRatio) // If the windows resolution is wider than it is taller, do the following
					{
						screenResolution.x = screenResolution.y * aspectRatio; // Set the screen resolution in the 'X' axis to the 'Y' dimension, multiplied by the native aspect ratio
					}
					else if (screenResolution.y > screenResolution.x / aspectRatio) // If the windows resolution is taller than it is wider, do the following
					{
						screenResolution.y = screenResolution.x / aspectRatio; // Set the screen resolution in the 'Y' axis to the 'X' dimension, divided by the native aspect ratio
					}

					glViewport(0, 0, screenResolution.x, screenResolution.y); // Update the viewport
					#pragma endregion

					#pragma region Update Colour Texture Attachments
					for (int i = 0; i != m_iColourAttachmentsApplied; i++) // For all of the colour attachments specified, do the following
					{
						glBindTexture(GL_TEXTURE_2D, m_iColourTextureAttachmentID[i]); // Bind the texture attachment
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenResolution.x, screenResolution.y, 0, GL_RGBA, GL_UNSIGNED_INT, NULL); // Configure the colour texture attachment (with alpha channel)
						glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture attachment
					}
					#pragma endregion

					#pragma region Update Depth Texture Attachment
					glBindTexture(GL_TEXTURE_2D, m_iDepthTextureAttachmentID); // Bind the texture attachment
					glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenResolution.x, screenResolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); // Configure the depth texture attachment
					glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture attachment
					#pragma endregion

					#pragma region Reattach Texture Attachments
					for (int i = 0; i != m_iColourAttachmentsApplied; i++) // For all of the colour attachments specified, do the following
					{
						glFramebufferTexture2D(GL_FRAMEBUFFER, colourBuffers[i], GL_TEXTURE_2D, m_iColourTextureAttachmentID[i], 0); // Attach colour texture to frame buffer
					}

					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iDepthTextureAttachmentID, 0); // Attach depth texture to frame buffer

					glDrawBuffers(4, colourBuffers); // Draw all of the colour attachments specified for the frame buffer
					#pragma endregion

					glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer object

					m_bWindowResized = false; // Reset the window resize state, used for future window resize events
				}

				glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBufferID); // Bind the frame buffer object

				if (m_bWireframeActive == true)	// If wireframe mode is active, do the following
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Draw the outlines of primitives
				}
			}
		}
		else // If a viewport is not active, do the following
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBufferID);

			if (m_bWireframeActive == true)	// If wireframe mode is active, do the following
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Draw the outlines of primitives
			}
		}
	}

	void OpenGLFrameBuffer::defaultUnbind() // Unbind the default frame buffer
	{
		Engine::Application& application = Engine::Application::getInstance(); // Get application accessor instance (singleton class)
		glm::vec2 screenResolution = application.getScreenResolution();	// Store the current resolution of the application window
		bool viewportActive = application.getWindow().isViewportActive(); // Return the viewport activeness of the window instance
		float aspectRatio = application.getWindow().getAspectRatio(); // Return the aspect ration of the window instance (viewport)

		m_screenResolution = glm::vec2(screenResolution.x, screenResolution.y); // Store the screens resolution (used for uniform variables - JSONLayer)

		if (viewportActive)	// If a viewport is active, do the following
		{
			if (m_bMSAAEnabled == true) // If MSAA is enabled, do the following
			{
				if (m_bWireframeActive == true)	// If wireframe mode is active, do the following
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill the interior of primitives (rasterise)
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer object

				#pragma region Update Viewport
				if (screenResolution.x > screenResolution.y * aspectRatio) // If the windows resolution is wider than it is taller, do the following
				{
					screenResolution.x = screenResolution.y * aspectRatio; // Set the screen resolution in the 'X' axis to the 'Y' dimension, multiplied by the native aspect ratio
				}
				else if (screenResolution.y > screenResolution.x / aspectRatio) // If the windows resolution is taller than it is wider, do the following
				{
					screenResolution.y = screenResolution.x / aspectRatio; // Set the screen resolution in the 'Y' axis to the 'X' dimension, divided by the native aspect ratio
				}

				glViewport(0, 0, screenResolution.x, screenResolution.y); // Update the viewport
				#pragma endregion

				#pragma region Blit MSAA Frame Buffer
				glBindFramebuffer(GL_READ_FRAMEBUFFER, m_iMultiSampledFrameBufferID); // Bind the MSAA frame buffer object (read data)
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_iFrameBufferID); // Bind the frame buffer object (write data)

				glBlitFramebuffer(0, 0, screenResolution.x, screenResolution.y, 0, 0, screenResolution.x, screenResolution.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
				#pragma endregion

				if (m_bMSAAEnabledUpdate == false || m_bWindowResized == true || m_bMSAASampleUpdate == true) // If the application window has been resized or MSAA has been enabled/ disabled or adjusted, do the following
				{
					glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBufferID); // Bind the frame buffer object

					#pragma region Update Colour Texture Attachment
					for (int i = 0; i != m_iColourAttachmentsApplied; i++) // For all of the colour attachments specified, do the following
					{
						glBindTexture(GL_TEXTURE_2D, m_iColourTextureAttachmentID[i]); // Bind the texture attachment
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenResolution.x, screenResolution.y, 0, GL_RGBA, GL_UNSIGNED_INT, NULL); // Configure the colour texture attachment (with alpha channel)
						glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture attachment
					}
					#pragma endregion

					#pragma region Update Depth Texture Attachment
					glBindTexture(GL_TEXTURE_2D, m_iDepthTextureAttachmentID); // Bind the texture attachment
					glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenResolution.x, screenResolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); // Configure the depth texture attachment
					glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture attachment
					#pragma endregion

					#pragma region Reattach Texture Attachments
					for (int i = 0; i != m_iColourAttachmentsApplied; i++) // For all of the colour attachments specified, do the following
					{
						glFramebufferTexture2D(GL_FRAMEBUFFER, colourBuffers[i], GL_TEXTURE_2D, m_iColourTextureAttachmentID[i], 0); // Attach colour texture to frame buffer
					}

					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iDepthTextureAttachmentID, 0); // Attach depth texture to frame buffer

					glDrawBuffers(4, colourBuffers); // Draw all of the colour attachments specified for the frame buffer
					#pragma endregion

					glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer object

					m_bMSAAEnabledUpdate = true; // Reset the MSAA enabled or disabled state, used for future MSAA toggling 
					m_bWindowResized = false; // Reset the window resize state, used for future window resize events
					m_bMSAASampleUpdate = false; // Reset the MSAA enable update state, used for future enabling/ disabling or MSAA
				}

				glBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBufferID); // Bind the frame buffer object

				if (m_bWireframeActive == true)	// If wireframe mode is active, do the following
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Draw the outlines of primitives
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill the interior of primitives (rasterise)
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer object
			}
			else if (m_bMSAAEnabled == false) // If MSAA is disabled, do the following
			{
				if (m_bWireframeActive == true)	// If wireframe mode is active, do the following
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill the interior of primitives (rasterise)
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer object
			}
		}
		else // If a viewport is not active, do the following
		{
			if (m_bWireframeActive == true)	// If wireframe mode is active, do the following
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill the interior of primitives (rasterise)
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer object
		}
	}

	void OpenGLFrameBuffer::shadowBind() // Bind the shadow map frame buffer
	{
		glViewport(0, 0, m_iShadowDepthMapWidth, m_iShadowDepthMapHeight); // Update the viewport

		glBindFramebuffer(GL_FRAMEBUFFER, m_iDepthMapFrameBufferID); // Bind the frame buffer object
	}

	void OpenGLFrameBuffer::shadowUnbind() // Unbind the shadow map frame buffer
	{
		Engine::Application& application = Engine::Application::getInstance(); // Get application accessor instance (singleton class)
		glm::vec2 screenResolution = application.getScreenResolution();	// Store the current resolution of the application window
		bool viewportActive = application.getWindow().isViewportActive(); // Return the viewport activeness of the window instance
		float aspectRatio = application.getWindow().getAspectRatio(); // Return the aspect ration of the window instance (viewport)

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer object

		#pragma region Update Viewport
		if (viewportActive)
		{
			if (screenResolution.x > screenResolution.y * aspectRatio) // If the windows resolution is wider than it is taller, do the following
			{
				screenResolution.x = screenResolution.y * aspectRatio; // Set the screen resolution in the 'X' axis to the 'Y' dimension, multiplied by the native aspect ratio
			}
			else if (screenResolution.y > screenResolution.x / aspectRatio) // If the windows resolution is taller than it is wider, do the following
			{
				screenResolution.y = screenResolution.x / aspectRatio; // Set the screen resolution in the 'Y' axis to the 'X' dimension, divided by the native aspect ratio
			}

			glViewport(0, 0, screenResolution.x, screenResolution.y); // Update the viewport
		}
		else
		{
			glViewport(0, 0, screenResolution.x, screenResolution.y);

			glEnable(GL_SCISSOR_TEST);

			glScissor(0, 0, screenResolution.x, screenResolution.y);

			glDisable(GL_SCISSOR_TEST);
		}
        #pragma endregion
	}

	void OpenGLFrameBuffer::increaseMSAA()
	{
		if (m_iSamples == supportedMSAASamples) // If the current number of samples being used by MSAA is the maximum number supported by the GPU, do the following
		{
			m_iSamples = 4;	// Set the number of samples being used by MSAA to '4'
		}
		else // Else if the current number of samples being used by MSAA is not the maximum number supported by the GPU, do the following
		{
			m_iSamples = m_iSamples * 2; // Set the number of samples being used by MSAA to the current sample size, doubled
		}
	}
	
	void OpenGLFrameBuffer::decreaseMSAA()
	{
		if (m_iSamples == 4) // If the current number of samples being used by MSAA is '4', do the following
		{
			m_iSamples = supportedMSAASamples; // Set the number of samples being used by MSAA to the maximum number of samples supported by the GPU
		}
		else // Else if the current number of samples being used by MSAA is not '4', do the following
		{
			m_iSamples = m_iSamples / 2; // Set the number of samples being used by MSAA to the current sample size, halved
		}
	}

	void OpenGLFrameBuffer::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e); // Create an event dispatcher instance

		dispatcher.dispatch<WindowResizeEvent>(std::bind(&FrameBuffer::onWindowResize, this, std::placeholders::_1)); // If the event type matches, call the corresponding function
	}

	bool OpenGLFrameBuffer::onWindowResize(WindowResizeEvent& e)
	{
		m_bWindowResized = true; // The window has been resized (update texture sizes)
		return false;
	}
}