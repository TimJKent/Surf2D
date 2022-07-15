#pragma once
#ifdef SE_PLATFORM_WINDOWS

extern SurfEngine::Application* SurfEngine::CreateApplication();

int main(int argc, char** argv) {

	SurfEngine::Log::Init();
	SE_CORE_TRACE(SE_VERSION);
	SE_CORE_TRACE("Initilized Engine Logger");
	SE_TRACE("Initilized Client Logger");
	auto app = SurfEngine::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif // SE_PLATFORM_WINDOWS
