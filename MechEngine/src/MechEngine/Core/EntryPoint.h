#pragma once
#ifdef ME_PLATFORM_WINDOWS

extern MechEngine::Application* MechEngine::CreateApplication();

int main(int argc, char** argv) {

	MechEngine::Log::Init();
	ME_CORE_TRACE("Initilized Engine Logger");
	ME_TRACE("Initilized Client Logger");
	auto app = MechEngine::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif // ME_PLATFORM_WINDOWS
