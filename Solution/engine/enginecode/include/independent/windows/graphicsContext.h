/** \file graphicsContext.h
*/

#pragma once

/**
\class GraphicsContext
\brief Interface class for all graphics contexts. 

Will combine windows tech with graphics API i.e. OPENGLGLFW or WIN32DIRECTX.
Basically provides means for initialisation and swapchain implementation.
*/
class GraphicsContext
{
public:
	virtual void init() = 0; //!< Initialize object
	virtual void swapBuffers() = 0; //!< Swap the buffers, update the window
};