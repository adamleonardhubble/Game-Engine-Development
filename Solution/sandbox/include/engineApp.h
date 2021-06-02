#pragma once

#include <Engine.h>

// entry point
#include "include/independent/core/entrypoint.h"



class engineApp : public Engine::Application
{
private:
	Engine::JSONSetup m_setup; //!< The leel setup object
public:
	engineApp();
	~engineApp() override;
};
