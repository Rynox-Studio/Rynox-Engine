#include <Core/Application.h>
using namespace Rynox::Core;

#include <Common/Logger.h>

int main()
{
	Application* app = nullptr;

	try
	{
		app = new Application();

		if (!app->Initialize({}))
		{
			return EXIT_FAILURE;
		}

		app->Run();
	}
	catch (const std::exception& e)
	{
		RNX_LOG_ERROR("Exception: {}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
