workspace "SurfEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "SurfEngine/vendor/glfw/include"
IncludeDir["GLAD"] = "SurfEngine/vendor/glad/include"
IncludeDir["IMGUI"] = "SurfEngine/vendor/imgui"
IncludeDir["glm"] = "SurfEngine/vendor/glm"
IncludeDir["stb_image"] = "SurfEngine/vendor/stb_image"
IncludeDir["entt"] = "SurfEngine/vendor/entt/include"
IncludeDir["yaml_cpp"] = "SurfEngine/vendor/yaml-cpp/include"
IncludeDir["mono"] = "SurfEngine/vendor/mono"
IncludeDir["box2d"] = "SurfEngine/vendor/box2d"


include "SurfEngine/vendor/glfw"
include "SurfEngine/vendor/glad"
include "SurfEngine/vendor/imgui"
include "SurfEngine/vendor/yaml-cpp"
include "SurfEngine/vendor/box2d"

project "SurfEngine"
	location "SurfEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "sepch.h"
	pchsource "SurfEngine/src/sepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	defines{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.mono}/include/mono-2.0",
		"%{IncludeDir.mono}/include",
		"%{IncludeDir.box2d}/include"
	}

	libdirs{
		"%{IncludeDir.mono}/lib"
	}

	links
	{
		"Box2D",
		"GLFW",
		"GLAD",
		"IMGUI",
		"yaml-cpp",
		"opengl32.lib",
		"mono-2.0-sgen.lib"
	}


	filter "system:windows"
		systemversion "latest"

		defines{
			"IMGUI_IMPL_OPENGL_LOADER_GLAD",
			"SE_PLATFORM_WINDOWS",
			"SE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

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

project "EngineEditor"
		kind "ConsoleApp"
		location "EngineEditor"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		defines{
			"_CRT_SECURE_NO_WARNINGS"
		}

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp"
		}

		includedirs
		{
			"SurfEngine/vendor/spdlog/include",
			"SurfEngine/src",
			"SurfEngine/vendor",
			"%{IncludeDir.glm}",
			"%{IncludeDir.entt}",
			"%{IncludeDir.yaml_cpp}",
			"%{IncludeDir.mono}/include/mono-2.0",
			"%{IncludeDir.mono}/include"
		}

		links{
			"SurfEngine"
		}

		postbuildcommands{
			"xcopy \"$(SolutionDir)SurfEngine\\vendor\\mono\\bin\\mono-2.0-sgen.dll\" \"$(TargetDir)\" /Y",
			"xcopy \"$(SolutionDir)SurfEngine\\vendor\\mono\\lib\\mono\\4.5\" \"$(TargetDir)..\\lib\\mono\\4.5\\\" /Y"
		}

		filter "system:windows"
			systemversion "latest"

			defines{
				"SE_PLATFORM_WINDOWS"
			}

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
