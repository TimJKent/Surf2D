project "Box2D"
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"include/**.h"
	}

	includedirs
	{
		"include",
		"src"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations.Debug"
			defines "SE_DEBUG"
			runtime "Debug"
			symbols "on"

	filter "configurations.Release"
		defines "SE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations.Dist"
		defines "SE_DIST"
		runtime "Release"
		optimize "on"